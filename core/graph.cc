/*
 * @Author: mander.liu
 * @Date: 2022-10-11 11:53:09 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 12:16:19
 */

#include "graph.h"

#include <unordered_set>

#include <glog/logging.h>

namespace hawking {
namespace graph {

Graph::~Graph() {
    Reset();
}

void Graph::Reset() {
    graph_name_ = "";
    stage_vec_.clear();
}

const GraphStage *Graph::GetStage(int32_t stage_index) const {
    if (static_cast<size_t>(stage_index) >= stage_vec_.size()) {
        return nullptr;
    }

    return stage_vec_[stage_index].get();
}

void Graph::Print() {
    // stage(parallel group)
    for (size_t stage_index = 0 ; stage_index < stage_vec_.size(); ++stage_index) {
        std::string paralell_group = "[";
        // node
        for (size_t node_index = 0;
             node_index < stage_vec_[stage_index]->nodes.size();
             ++node_index) {
            paralell_group += stage_vec_[stage_index]->nodes[node_index]->GetName();

            // op(serial group)
            const std::vector<std::string> *op_name_list_ =
                stage_vec_[stage_index]->nodes[node_index]->GetOpList();
            paralell_group += "(";
            for (size_t op_index = 0;
                 op_index < op_name_list_->size();
                 ++op_index) {
                paralell_group += op_name_list_->at(op_index);
                paralell_group += ",";
            }
            paralell_group.back() = ')';

            paralell_group += ",";
        }
        paralell_group.back() = ']';
        LOG(INFO) << paralell_group;
    }
}

bool Graph::topologic_sort(const GraphConf &graph_conf) {
    // 1. name2id + in_degree
    std::unordered_map<std::string, int32_t> name_id_dict;
    std::vector<int32_t> in_degree_vec(graph_conf.nodes_size());
    bool has_init_node = false;
    for (int32_t i = 0; i < graph_conf.nodes_size(); ++i) {
        const DagNode &cur_node = graph_conf.nodes(i);
        name_id_dict.insert(std::make_pair(cur_node.name(), i));
        in_degree_vec[i] = cur_node.dependencies_size();
        if (!has_init_node && !in_degree_vec[i]) {
            has_init_node = true;
        }
    }

    if (!has_init_node) {
        LOG(ERROR) << "no independent node exists.";
        return false;
    }

    // 2. depend set
    std::unordered_map<int32_t, std::unordered_set<int32_t>> depend_edge_dict;
    for (int32_t i = 0; i < graph_conf.nodes_size(); ++i) {
        const DagNode &cur_node = graph_conf.nodes(i);
        // try fill depend set
        for (int32_t depend_index = 0;
             depend_index < cur_node.dependencies_size();
             ++depend_index) {
            const std::string &depend_name =
                cur_node.dependencies(depend_index).name();
            auto iter_name2id = name_id_dict.find(depend_name);
            if (iter_name2id == name_id_dict.end()) {
                LOG(ERROR) << "node[" << cur_node.name() << "]'s depend"
                           << depend_name << " not found.";
                return false;
            }

            const int32_t &depend_id = iter_name2id->second;
            depend_edge_dict[depend_id].insert(i);
        }
    }

    std::unordered_set<int32_t> processed_set;
    while (1) {
        std::unique_ptr<GraphStage> cur_stage = std::make_unique<GraphStage>();
        std::unordered_set<int32_t> stage_node_id_set;

        // loop for look for zero-in_degree node
        for (int32_t i = 0; i < graph_conf.nodes_size(); ++i) {
            // ignore processed
            auto iter_processed = processed_set.find(i);
            if (iter_processed != processed_set.end()) {
                continue;
            }

            const DagNode &cur_node = graph_conf.nodes(i);
            if (!in_degree_vec[i]) {
                std::unique_ptr<GraphNode> node =
                    std::make_unique<GraphNode>(cur_node.name(), i);
                for (int32_t op_index = 0;
                     op_index < cur_node.op_group_size();
                     ++op_index) {
                    node->AppendOp(cur_node.op_group(op_index));
                }
                cur_stage->nodes.push_back(std::move(node));

                stage_node_id_set.insert(i);
                processed_set.insert(i);
            }
        }

        // all is finish
        if (stage_node_id_set.empty()) {
            break;
        }

        // update others in_degree
        for (auto &node : cur_stage->nodes) {
            auto iter_depend_edge = depend_edge_dict.find(node->GetNodeId());
            if (iter_depend_edge != depend_edge_dict.end()) {
                for (auto iter = depend_edge_dict[node->GetNodeId()].begin(); 
                     iter != depend_edge_dict[node->GetNodeId()].end();
                     ++iter) {
                    in_degree_vec[*iter]--;
                }
            }
        }

        // update stage_vec
        stage_vec_.push_back(std::move(cur_stage));
    }

    return true;
}

bool Graph::Init(const GraphConf &graph_conf) {
    graph_name_ = graph_conf.name();
    return topologic_sort(graph_conf);
}

}
}
