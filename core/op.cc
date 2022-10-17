/*
 * @Author: mander.liu 
 * @Date: 2022-10-13 12:05:43 
 * @Last Modified by: mander.liu
 * @Last Modified time: 2022-10-16 16:35:32
 */

 #include "op.h"

 #include "graph_instance.h"

namespace hawking {
namespace graph {

const google::protobuf::Message *OpBase::GetRequest() const {
    return graph_instance_->GetRequest();
}

google::protobuf::Message *OpBase::GetResponse() {
    return graph_instance_->GetResponse();
}

bool OpBase::Process() {
    bool ret = Implement();
    SetImplemented();
    return ret;
}

int64_t OpBase::GetRequestId() {
    if (!graph_instance_) {
        return -1;
    }
    return graph_instance_->GetRequestId();
}

std::unordered_map<std::string, OpBase *> *OpBase::GetOpDict() {
    if (!graph_instance_) {
        return nullptr;
    }

    return graph_instance_->GetOpDict();
}

OpBase *OpManager::GetOp(const std::string &op_name) {
    auto iter = optool_dict_.find(op_name);
    if (iter == optool_dict_.end()) {
        return nullptr;
    }

    OpBaseTool *optool = iter->second;
    return optool->FetchOp();
}

void OpManager::ReturnOp(const std::string &op_name, OpBase *op) {
    if (!op) {
        return;
    }

    auto iter = optool_dict_.find(op_name);
    if (iter == optool_dict_.end()) {
        return;
    }

    OpBaseTool *optool = iter->second;
    optool->ReturnOp(op);
}

}
}