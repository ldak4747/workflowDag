/*
 * @Author: mander.liu
 * @Date: 2022-10-11 11:30:26 
 * @Last Modified by: mikey.zhaopeng
 * @Last Modified time: 2022-10-11 15:52:29
 */

#include "graph.h"

#include <glog/logging.h>

namespace hawking {
namespace graph {

bool GraphWrapper::Init(const GraphConf &graph_conf) {
    name_ = graph_conf.name();
    return graph_.init(graph_conf);
}

void Graph::Reset() {
    return;
}

DagInstance *GraphWrapper::FetchGraphInstance(const std::string &service_name,
                                              const int64_t request_id) {
    GraphInstance *graph_instance = butil::get_object<GraphInstance>();
    if (!graph_instance) {
        LOG(ERROR) << "request_id[" << request_id
                   << "] fetch graph_instance nullptr.";
    }
    return graph_instance;
}

void GraphWrapper::ReturnGraphInstance(GraphInstance *graph_instance) {
    graph_instance->Reset();
    butil::get_object<GraphInstance>(graph_instance);
}

}
}