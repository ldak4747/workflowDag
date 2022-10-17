/*
 * @Author: mander.liu 
 * @Date: 2022-10-15 17:32:11 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 16:04:24
 */
#pragma once

#include <random>

#include "graph_instance.h"
#include "init1_op.h"

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_int_distribution<int64_t> disti(0, 1000);
std::uniform_real_distribution<float> distf(-1, 1);

struct Item {
    int64_t item_id;
    std::vector<float> embedding;
};

struct Ann_Op_Output {
    std::vector<Item> ann_result;
};

class Ann_Op : public hawking::graph::Op<Ann_Op_Output> {
public:
    DECLARE_OP(Ann_Op)

    bool Implement() override {
        RetrieveContext *retrieve_context =
            GetOpOutput<RetrieveContext>("Init1_Op");
        const std::unordered_set<int64_t> &filter_id_set =
            retrieve_context->filter_id_set;
        if (!retrieve_context) {
            LOG(INFO) << GetRequestId() << " retrieve_context is nullptr ";
        }
        

/*
        Ann_Op_Output *ann_output = GetOutput();
        ann_output->ann_result.clear();
        for (int i = 0; i < 500; ++i) {
            int64_t item_id = disti(eng);
            if (filter_id_set.find(item_id) == filter_id_set.end()) {
                Item item;
                item.item_id = item_id;

                std::vector<float> &embedding = item.embedding;
                embedding.reserve(16);
                for (int emb_idx = 0; emb_idx < 16; ++emb_idx) {
                    embedding.push_back(distf(eng));
                }

                ann_output->ann_result.push_back(std::move(item));
            }
        }
*/
        return true;
    }
};