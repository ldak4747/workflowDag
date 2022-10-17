/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 15:15:16
 */
#pragma once

#include "graph_instance.h"

struct PostAnn_Op_Output {
    int a;
};

class PostAnn_Op : public hawking::graph::Op<PostAnn_Op_Output> {
public:
    DECLARE_OP(PostAnn_Op)

    bool Implement() override {
        // LOG(INFO) << "PostAnn_Op doing!";
        return true;
    }
};