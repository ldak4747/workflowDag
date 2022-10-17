/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 15:15:22
 */
#pragma once

#include "graph_instance.h"

struct PreAnn_Op_Output {
    int a;
};

class PreAnn_Op : public hawking::graph::Op<PreAnn_Op_Output> {
public:
    DECLARE_OP(PreAnn_Op)

    bool Implement() override {
        // LOG(INFO) << "PreAnn_Op doing!";
        return true;
    }
};