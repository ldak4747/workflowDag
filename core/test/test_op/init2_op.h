/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 15:15:13
 */
#pragma once

#include "graph_instance.h"

struct Init2_Op_Output {
    // nothing
};

class Init2_Op : public hawking::graph::Op<Init2_Op_Output> {
public:
    DECLARE_OP(Init2_Op)

    bool Implement() override {
        return true;
    }
};