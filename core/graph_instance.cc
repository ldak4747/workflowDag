/*
 * @Author: mander.liu
 * @Date: 2022-10-11 13:29:43 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 18:08:06
 */

#include "graph_instance.h"

#include <glog/logging.h>

#include "graph_manager.h"

namespace hawking {
namespace graph {

// for rpc server to fetch when rpc implement start
GraphInstance *FetchGraphInstance() {
    return butil::get_object<GraphInstance>();
}

// rpc server must call it after rpc implement finish
void ReturnGraphInstance(GraphInstance *graph_instance) {
    // 1. release resource  2. return object to object pool
    graph_instance->Reset();
    butil::return_object(graph_instance);
}

// call after FetchGraphInstance
bool GraphInstance::Init(const std::string &graph_name,
                         const google::protobuf::Message *request,
                         google::protobuf::Message *response) {
    // graph
    graph_ = GraphManager::Instance().GetGraphByGraphName(graph_name);
    if (!graph_) {
        LOG(ERROR) << "graph_name[" << graph_name << "] not found.";
        return false;
    }

    // input
    request_ = request;
    response_ = response;
    return true;
}

// called by ReturnGraphInstance
void GraphInstance::Reset() {
    // every op reset
    for (auto iter = op_dict_.begin(); iter != op_dict_.end(); ++iter) {
        OpManager::Instance().ReturnOp(iter->first, iter->second);
    }

    // reset graph-instance itself
    op_dict_.clear();
    request_ = nullptr;
    response_ = nullptr;
    graph_ = nullptr;
    tracer_.clear();
}

// call after GraphInstance::Init
bool GraphInstance::Execute(int64_t request_id) {
    SetRequestId(request_id);

    // execute every stage serialal
    bool ret = true;
    for (size_t stage_index = 0;
         stage_index < graph_->StageSize();
         ++stage_index) {
        const GraphStage &stage = *(graph_->GetStage(stage_index));
        if (!ExecuteOneStage(stage)) {
            LOG(ERROR) << "fail to execute stage[" << stage_index << "]";
            ret = false;
            break;
        }
    }

    // record trace
    for (auto iter = op_dict_.begin(); iter != op_dict_.end(); ++iter) {
        if (!iter->second->IsTraceEmpty()) {
            butil::IOBuf &op_trace = iter->second->GetTrace();
            AppendTrace(op_trace);
        }
    }

    return ret;
}

#define RESTORE_OP(OP_NAME) \
    dynamic_cast<#OP_NAME *>(opbase)

// Node level execute
void *execute(void *args) {
    NodeContext *node_context = (NodeContext *)args;
    CHECK(node_context != nullptr);
    const GraphNode *node = node_context->GetNode();
    GraphInstance *graph_instance = node_context->GetGraphInstance();

    // do every op of current node serialal
    // note: op could not be NULL because op only success with RegisterOpInstance
    // could run here
    for (size_t op_index = 0; op_index < node->GetOpList()->size(); ++op_index) {
        const std::string &op_name = node->GetOpList()->at(op_index);
        OpBase *op = graph_instance->GetOp(op_name);
        if (!op) {
            LOG(ERROR) << "fail to get op[" << op_name << "]";
            return nullptr;
        }
        if (!op->Process()) {
            LOG(ERROR) << "fail to execute op[" << op_name << "]";
            return nullptr;
        }
    }

    return nullptr;
}

// Stage level execute
bool GraphInstance::ExecuteOneStage(const GraphStage &stage) {
    // if only one node in current stage, do here
    size_t paralell_group_size = stage.nodes.size();
    if (1 == paralell_group_size) {
        const GraphNode *node = stage.nodes[0].get();
        NodeContext node_context(node, this);
        if (!RegisterOpInstance(node)) {
            return false;
        }
        execute((void *)&node_context);
        return true;
    }

    // if more than 1 node in current stage, do parallel
    bool ret = true;
    std::vector<NodeContext> node_context;
    node_context.reserve(paralell_group_size);
    for (size_t node_index = 0; node_index < paralell_group_size; ++node_index) {
        const GraphNode *node = stage.nodes[node_index].get();
        if (!RegisterOpInstance(node)) {
            return false;
        }
    }

    for (size_t node_index = 0; node_index < paralell_group_size; ++node_index) {
        const GraphNode *node = stage.nodes[node_index].get();
        node_context.emplace_back(node, this);
        if (0 != bthread_start_background(
            node_context[node_index].Bid(), nullptr,
            execute, (void *)&node_context[node_index])) {

            node_context.pop_back();
            LOG(ERROR) << "failed to create node bthread for " << node->GetName();
            ret = false;
            break;
        }
    }

    // join
    for (size_t bt_index = 0; bt_index < node_context.size(); ++bt_index) {
        bthread_join(*(node_context[bt_index].Bid()), nullptr);
    }

    return ret;
}

// fetch op object from object pool,
// set graph-instance to every op + register <op name, op object> in graph-instance
bool GraphInstance::RegisterOpInstance(const GraphNode *node) {
    for (size_t op_index = 0; op_index < node->GetOpList()->size(); ++op_index) {
        const std::string &op_name = node->GetOpList()->at(op_index);
        OpBase *op = OpManager::Instance().GetOp(op_name);
        if (!op) {
            LOG(ERROR) << "fetch op instance fail for " << op_name;
            return false;
        }

        op->SetGraphInstance(this);
        op_dict_.insert(std::make_pair(op_name, op));
    }

    return true;
}

OpBase *GraphInstance::GetOp(const std::string &op_name) {
    auto iter = op_dict_.find(op_name);
    if (iter == op_dict_.end()) {
        return nullptr;
    }

    return iter->second;
}

}
}