/*
 * @Author: mander.liu
 * @Date: 2022-10-11 10:52:10 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-13 19:43:16
 */

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "graph.h"

namespace hawking {
namespace graph {

bool read_proto_buf(const std::string &conf_path,
                    google::protobuf::Message *graph_conf);

class Graph;
class GraphManager {
public:
    static GraphManager &Instance() {
        static GraphManager graph_manager;
        return graph_manager;
    }

    bool Init(const std::string &conf_path);

    // get graph
    const Graph *GetGraphByGraphName(const std::string &graph_name) const;

    // reload
    bool Reload();

private:
    GraphManager() = default;

private:
    std::unordered_map<std::string, std::unique_ptr<Graph>> graph_dict_;
};

}
}