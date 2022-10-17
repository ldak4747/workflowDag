/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 15:15:25
 */
#pragma once

#include "graph_instance.h"

struct PreFilter_Op_Output {
    int a;
};

class PreFilter_Op : public hawking::graph::Op<PreFilter_Op_Output> {
public:
    DECLARE_OP(PreFilter_Op)

    bool Implement() override {
        // LOG(INFO) << "PreFilter_Op doing!";
        return true;
    }
};