/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 15:15:05
 */
#pragma once

#include "graph_instance.h"

struct Filter1_Op_Output {
    int a;
};

class Filter1_Op : public hawking::graph::Op<Filter1_Op_Output> {
public:
    DECLARE_OP(Filter1_Op)

    bool Implement() override {
        // LOG(INFO) << "Filter1_Op doing!";
        return true;
    }
};