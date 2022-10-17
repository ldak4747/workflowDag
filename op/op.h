/*
 * @Author: mander.liu
 * @Date: 2022-10-10 15:18:17 
 * @Last Modified by: mikey.zhaopeng
 * @Last Modified time: 2022-10-10 15:38:30
 */

#pragma once

namespace hawking {
namespace graph {

class Op {
public:
    Op() = default;
    virtual ~Op() = default;

    int init(Bus *bus, Dag *dag, uint32_t id,
             const std::string &name, const std::string &type,
             void *conf,
             const std::vector<std::string> &address,
             const uint64_t log_id);
    int deinit();

    int check_time(const char *tag);

    Channel *mutable_depend_channel(const std::string &op);
    const Channel *get_depend_channel(const std::string &op) const ;

    template<class T>
    T *mutable_depend_argument(const std::string &op) {
        Channel *channel = mutable_depend_channel(op);
        if (!channel) {
            LOG(ERROR) << "cannot get mutable channel of " << op << " in " << name_;
            return nullptr;
        }

        OpChannel<T> *op_channel = dynamic_cast<OpChannle<T> *>(channel);
        if (!op_channel) {
            LOG(ERROR) << "Cannot dynamic cast channel of op:" << this->name()
                       << " to type: " << typeid(T).name();
            return nullptr;
        }

        return op_channel->data();
    }

    template<class T>
    const T *get_depend_argument(const std::string &op) const {
        const Channel *channel = get_depend_channel(op);
        if (!channel) {
            LOG(ERROR) << "cannot get read-only channel of " << op << " in " << name_;
            return nullptr;
        }

        const OpChannel<T> *op_channel = dynamic_cast<const OpChannle<T> *>(channel);
        if (!op_channel) {
            LOG(ERROR) << "Cannot dynamic cast const channel of op:" << this->name()
                       << " to type: " << typeid(T).name();
            return nullptr;
        }

        return op_channel->data();
    }

    google::protobuf::Message* mutable_message();
    const google::protobuf::Message* get_message() const;

    template<class T>
    T *mutable_data() {
        Channel *channel = mutable_channel();
        return (dynamic_cast<OpChannel<T> *>(channel))->data();
    }

    template<class T>
    const T *get_data() const {
        const Channel *channel = get_channel();
        return (dynamic_cast<const OpChannel<T> *>(channel))->data();
    }

    
};

}
}