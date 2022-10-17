/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 15:15:27
 */
#pragma once

#include "graph_instance.h"

struct Response_Op_Output {
    int a;
};

class Response_Op : public hawking::graph::Op<Response_Op_Output> {
public:
    DECLARE_OP(Response_Op)

    bool Implement() override {
        // LOG(INFO) << "Response_Op doing!";
        return true;
    }
};