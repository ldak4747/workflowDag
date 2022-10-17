/*
 * @Author: mikey.zhaopeng 
 * @Date: 2022-10-11 13:42:10 
 * @Last Modified by: mikey.zhaopeng
 * @Last Modified time: 2022-10-11 18:39:48
 */

#pragma once

#include <string>

#include "graph_instance.h"

namespace hawking {
namespace graph {

class Output {
public:
    Output() = default;
    virtual ~Output();

    void Init();
    void Reset();

    uint32_t id() { return id_; }
    const std::string &OpName() { return *op_name_p_; }

    void Append2DagInstance(DagInstance *dag_instance);

    virtual void ClearData() = 0;
    virtual void *GetData() const = 0;
    virtual const void *GetData() const = 0;
    virtual std::string debug_string() const = 0;

private:
    uint32_t id_;
    const std::string *op_name_p_;
};

template<class T>
class OpOutput : public Output {
public:
    OpOutput() = default;
    virtual ~OpOutput();

    virtual void ClearData() override;
    virtual void *GetData() override;
    virtual const void *GetData() override;
    virtual std::string debug_string() override const;

private:
    T data_;
};

template<>
class OpOutput<google::protobuf::Message> : public Output {
public:
    OpOutput<google::protobuf::Message>() : data_(nullptr) {}
    virtual ~OpOutput<google::protobuf::Message>() {
        data_ = nullptr;
    }

    virtual void ClearData() override;
    virtual void *GetData() const override;
    virtual const void *GetData() const override;
    virtual std::string debug_string() const override;

private:
    const google::protobuf::Message *data_;
};

}
}