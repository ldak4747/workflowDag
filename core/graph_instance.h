/*
 * @Author: mander.liu
 * @Date: 2022-10-11 13:21:37 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 16:35:10
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <bthread/bthread.h>

#include "graph.h"
#include "op.h"

namespace hawking {
namespace graph {

void *execute(void *args);
GraphInstance *FetchGraphInstance();
void ReturnGraphInstance(GraphInstance *graph_instance);

struct GraphNodeInstance {
    OpBase *op = nullptr;
    DagNode *node = nullptr;
};

struct GraphStageInstance {
    std::vector<GraphNodeInstance *> node_vec;
    std::string name;
};

class GraphInstance {
public:
    GraphInstance() = default;
    ~GraphInstance() { Reset(); }

    bool Init(const std::string &graph_name,
              const google::protobuf::Message *request,
              google::protobuf::Message *response);
    void Reset();

    void SetRequestId(int64_t request_id = -1) { request_id_ = request_id; }
    int64_t GetRequestId() { return request_id_; }
    std::unordered_map<std::string, OpBase *> *GetOpDict() {
        return &op_dict_;
    }

    bool Execute(int64_t requestid);
    bool ExecuteOneStage(const GraphStage &stage);
    bool RegisterOpInstance(const GraphNode *node);
    OpBase *GetOp(const std::string &op_name);
    const google::protobuf::Message *GetRequest() {
        return request_;
    }
    google::protobuf::Message *GetResponse() {
        return response_;
    }
    void AppendTrace(const butil::IOBuf &op_trace) {
        tracer_.append(op_trace);
    }

private:
    int64_t request_id_;
    const google::protobuf::Message *request_;
    google::protobuf::Message *response_;
    const Graph *graph_;
    std::unordered_map<std::string, OpBase *> op_dict_;
    butil::IOBuf tracer_;
};

class NodeContext {
public:
    NodeContext() = default;
    NodeContext(const GraphNode *node, GraphInstance *graph_instance) {
        Load(node, graph_instance);
    }
    ~NodeContext() {
        node_ = nullptr;
        graph_instance_ = nullptr;
    }

    void Load(const GraphNode *node, GraphInstance *graph_instance) {
        node_ = node;
        graph_instance_ = graph_instance;
    }
    const GraphNode *GetNode() { return node_; }
    GraphInstance *GetGraphInstance() { return graph_instance_; }
    bthread_t *Bid() { return &bid_; }

private:
    bthread_t bid_;
    const GraphNode *node_ = nullptr;
    GraphInstance *graph_instance_ = nullptr;  
};

#

}
}