/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 15:15:30
 */
#pragma once

#include "graph_instance.h"

struct Udf_Op_Output {
    int a;
};

class Udf_Op : public hawking::graph::Op<Udf_Op_Output> {
public:
    DECLARE_OP(Udf_Op)

    bool Implement() override {
        // LOG(INFO) << "Udf_Op doing!";
        return true;
    }
};