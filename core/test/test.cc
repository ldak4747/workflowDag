/*
 * @Author: mander.liu
 * @Date: 2022-10-11 18:29:21 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 17:16:03
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <thread>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "graph_manager.h"
#include "graph_instance.h"
#include "hawking_server.pb.h"

DECLARE_string(log_dir);

TEST(graph, parse_test) {
    hawking::graph::GraphManager::Instance().Init("/workflowgraph/core/test/sample.prototxt");
}

TEST(graph, graph_instance_test) {
    hawking::graph::GraphManager::Instance().Init("/workflowgraph/core/test/sample.prototxt");

    // fetch graph instance
    hawking::graph::GraphInstance *graph_instance = hawking::graph::FetchGraphInstance();
    CHECK(graph_instance != nullptr);

    // run
    const hawking::server::HawkingUserRequest request;
    hawking::server::HawkingResponse response;
    graph_instance->Init("test_service_1", &request, &response);
    graph_instance->Execute(0);

    // return graph instance(return op instance meanwhile)
    hawking::graph::ReturnGraphInstance(graph_instance);
}

TEST(graph, parallel_execute_graph_test) {
    FLAGS_log_dir = "./log";
    google::InitGoogleLogging("parallel_execute_graph_test");
    hawking::graph::GraphManager::Instance().Init("/workflowgraph/core/test/sample.prototxt");

    std::vector<std::thread> paralell_group;
    paralell_group.reserve(1000);
    for (int i = 0; i < 1000; ++i) {
        paralell_group.emplace_back([i](){
            // fetch graph instance
            hawking::graph::GraphInstance *graph_instance = hawking::graph::FetchGraphInstance();
            CHECK(graph_instance != nullptr);

            // run
            std::string graph_name = "test_service_1";
            if (i % 2) {
                graph_name = "test_service_2";
            }
            hawking::server::HawkingUserRequest request;
            hawking::server::HawkingResponse response;
            for (int64_t filter_index = 0; filter_index < i; ++filter_index) {
                request.add_short_time_expose_filter(filter_index);
            }

            graph_instance->Init(graph_name, &request, &response);
            graph_instance->Execute(i);

            // return graph instance(return op instance meanwhile)
            hawking::graph::ReturnGraphInstance(graph_instance);
        });
    }

    for (auto &th : paralell_group) {
        th.join();
    }
}

int main (int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}