/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 15:15:19
 */
#pragma once

#include "graph_instance.h"

struct PostFilter_Op_Output {
    int a;
};

class PostFilter_Op : public hawking::graph::Op<PostFilter_Op_Output> {
public:
    DECLARE_OP(PostFilter_Op)

    bool Implement() override {
        // LOG(INFO) << "PostFilter_Op doing!";
        return true;
    }
};