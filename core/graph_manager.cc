/*
 * @Author: mander.liu
 * @Date: 2022-10-11 11:04:08 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-13 19:49:52
 */

#include "graph_manager.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>

#include <glog/logging.h>

namespace hawking {
namespace graph {

bool read_proto_buf(const std::string &conf_path,
                    google::protobuf::Message *graph_conf) {
    int fd = open(conf_path.data(), O_RDONLY);
    if (fd == -1) {
        LOG(ERROR) << "proto File not found: " << conf_path.c_str();
        return -1;
    }

    google::protobuf::io::FileInputStream input(fd);
    input.SetCloseOnDelete(true);
    bool ret = google::protobuf::TextFormat::Parse(&input, graph_conf);
    return ret;
}

bool GraphManager::Init(const std::string &conf_path) {
    GraphConfs graph_confs;
    if (!read_proto_buf(conf_path, &graph_confs)) {
        LOG(ERROR) << "Failed load proto configure from " << conf_path;
        return false;
    }

    for (int i = 0; i < graph_confs.graph_confs_size(); ++i) {
        const std::string &graph_name = graph_confs.graph_confs(i).name();
        std::unique_ptr<Graph> graph(new Graph());
        if (!graph) {
            LOG(ERROR) << "Failed create graph for " << graph_name;
            return false;
        }

        if (!graph->Init(graph_confs.graph_confs(i))) {
            LOG(ERROR) << "Failed init graph for " << graph_name;
            return false;
        }

        LOG(INFO) << "graph_name: " << graph_name;
        graph->Print();
        graph_dict_.emplace(graph_name, std::move(graph));
    }

    return true;
}

const Graph *GraphManager::GetGraphByGraphName(const std::string &graph_name) const {
    auto iter = graph_dict_.find(graph_name);
    if (iter == graph_dict_.end()) {
        return nullptr;
    }

    return iter->second.get();
}

bool GraphManager::Reload() {
    // TODO
    return true;
}

}
}