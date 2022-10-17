/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 15:15:07
 */
#pragma once

#include "graph_instance.h"

struct Filter2_Op_Output {
    int a;
};

class Filter2_Op : public hawking::graph::Op<Filter2_Op_Output> {
public:
    DECLARE_OP(Filter2_Op)

    bool Implement() override {
        // LOG(INFO) << "Filter2_Op doing!";
        return true;
    }
};