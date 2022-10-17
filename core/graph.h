/*
 * @Author: mander.liu
 * @Date: 2022-10-11 11:38:57 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-13 19:44:17
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "service_configuration.pb.h"

namespace hawking {
namespace graph {

class GraphNode {
public:
    explicit GraphNode(const std::string &name, const int32_t id) : 
        node_id_(id), name_(name) {}
    
    const int32_t &GetNodeId() const { return node_id_; }
    const std::string &GetName() const { return name_; }
    const std::vector<std::string> *GetOpList() const { return &op_name_list_; }
    void AppendOp(const std::string &op) {
        op_name_list_.push_back(op);
    }

private:
    int32_t node_id_;
    std::string name_;
    std::vector<std::string> op_name_list_;
};

class GraphStage {
public:
    GraphStage() = default;
    std::vector<std::unique_ptr<GraphNode>> nodes;
};

class Graph {
public:
    Graph() = default;
    virtual ~Graph();

    const std::string &GetName() { return graph_name_; }
    bool Init(const GraphConf &graph_conf);
    void Reset();
    void Print();

    size_t StageSize() const { return stage_vec_.size(); }

    const GraphStage *GetStage(int32_t stage_index) const;

private:
    bool topologic_sort(const GraphConf &graph_conf);

private:
    std::string graph_name_;
    std::vector<std::unique_ptr<GraphStage>> stage_vec_;
};

}
}