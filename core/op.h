/*
 * @Author: mander.liu
 * @Date: 2022-10-11 14:53:31 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 18:08:33
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glog/logging.h>
#include <google/protobuf/message.h>
#include <brpc/traceprintf.h>
#include <butil/iobuf.h>
#include <butil/object_pool.h>

namespace hawking {
namespace graph {

class GraphInstance;
class OpBase {
public:
    OpBase() = default;
    virtual ~OpBase() { Reset(); }
    void Reset() {
        graph_instance_ = nullptr;
        implemented_ = false;
        tracer_.buf().clear();
    }

    int64_t GetRequestId();
    std::unordered_map<std::string, OpBase *> *GetOpDict();
    virtual bool Implement() {
        LOG(ERROR) << "must call user Op instance";
        CHECK(false);
        return false;
    }
    bool Process();
    bool GetImplemented() { return implemented_; }
    void SetImplemented() { implemented_ = true; }
    void SetGraphInstance(GraphInstance *graph_instance) {
        graph_instance_ = graph_instance;
    }
    const google::protobuf::Message *GetRequest() const;
    google::protobuf::Message *GetResponse();

    template<class Arg, class ...Args>
    void Trace(const Arg &arg, const Args &...args) {
        tracer_ << arg;
        Trace(args...);
    }
    butil::IOBuf &GetTrace() {
        return tracer_.buf();
    }
    bool IsTraceEmpty() {
        return tracer_.buf().empty();
    }

    GraphInstance *graph_instance_;

private:
    void Trace() {
        tracer_ << std::endl;
    }

    bool implemented_;
    butil::IOBufBuilder tracer_;
};

template<class DataType>
class Op : public OpBase {
public:
    Op() = default;
    virtual ~Op() = default;

    DataType *GetOutput() {
        return &data_;
    }

    template<class DATA_TYPE>
    DATA_TYPE *GetOpOutput(const std::string &op_name) {
        Op<DATA_TYPE> *op =
            dynamic_cast<Op<DATA_TYPE> *>(graph_instance_->GetOp(op_name));
        if (!op) {
            return nullptr;
        }

        return op->GetImplemented() ? op->GetOutput() : nullptr;
    }

private:
    DataType data_;
};

class OpBaseTool {
public:
    virtual OpBase *FetchOp() = 0;
    virtual void ReturnOp(OpBase *op) = 0;
};

template<class OpName>
class OpTool : public OpBaseTool {
public:
    static OpTool<OpName> &Instance() {
        static OpTool<OpName> optool;
        return optool;
    }

    OpName *FetchOp() override {
        return butil::get_object<OpName>();
    }

    void ReturnOp(OpBase *op) override {
        op->Reset();
        butil::return_object<OpName>(dynamic_cast<OpName *>(op));
    }
};

class OpManager {
public:
    OpManager() = default;
    ~OpManager() = default;

    static OpManager &Instance() {
        static OpManager op_manager;
        return op_manager;
    }

    template<class OpName>
    void RegisterOp(const std::string &op_name) {
        if (op_name.empty()) {
            return;
        }

        auto iter = optool_dict_.find(op_name);
        if (iter == optool_dict_.end()) {
            optool_dict_[op_name] = &(OpTool<OpName>::Instance());
        }
    }

    OpBase *GetOp(const std::string &op_name);
    void ReturnOp(const std::string &op_name, OpBase *op);

private:
    std::unordered_map<std::string, OpBaseTool *> optool_dict_;
};

#define REGISTER_OP(OP_NAME)                                        \
  hawking::graph::OpManager::Instance().RegisterOp<OP_NAME>(#OP_NAME)

#define DECLARE_OP(OP_NAME)             \
    OP_NAME() { REGISTER_OP(OP_NAME); } \
    static OP_NAME OP_NAME##_instance;

#define DEFINE_OP(OP_NAME) OP_NAME OP_NAME::OP_NAME##_instance

}
}