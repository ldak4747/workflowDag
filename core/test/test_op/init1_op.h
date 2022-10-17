/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 15:42:13
 */
#pragma once

#include "hawking_server.pb.h"
#include "graph_instance.h"

struct RetrieveContext {
    std::unordered_set<int64_t> filter_id_set;
    std::vector<std::string> udf_need_column_list;
};

class Init1_Op : public hawking::graph::Op<RetrieveContext> {
public:
    DECLARE_OP(Init1_Op)

    bool Implement() override {
        const hawking::server::HawkingUserRequest *request = 
            (const hawking::server::HawkingUserRequest *)GetRequest();
        
        if (!request) {
            return false;
        }

        RetrieveContext *retrieve_context = GetOutput();
        std::unordered_set<int64_t> &filter_id_set = retrieve_context->filter_id_set;
        filter_id_set.clear();
        for (int i = 0; i < request->short_time_expose_filter_size(); ++i) {
            filter_id_set.insert(request->short_time_expose_filter(i));
        }

        return true;
    }
};