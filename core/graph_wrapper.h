/*
 * @Author: mikey.zhaopeng 
 * @Date: 2022-10-11 11:21:07 
 * @Last Modified by: mikey.zhaopeng
 * @Last Modified time: 2022-10-11 15:50:39
 */

#pragma once

#include <string>

#include "graph.h"

namespace hawking {
namespace graph {

class DagInstance;
class GraphWrapper {
public:
    GraphWrapper() = default;
    ~GraphWrapper() = default;

    bool Init(const GraphConf &graph_conf);
    void Reset();

    const std::string &Name() { return name_; }

    GraphInstance *FetchGraphInstance(const std::string &service_name,
                                      const int64_t request_id);
    void ReturnGraphInstance(GraphInstance *graph_instance);

private:
    Graph graph_;
    std::string name_;
};

}
}