依托brpc框架，只需要8个文件即实现的dag业务开发框架.

【依赖】
brpc
c++17

【源文件】
都在core目录下，4个h文件，4个cc文件
    a、graph_manager.h/graph_manager.cc：负责推导和维护你的组图的单例。应在服务启动前调用单例的Init方法。正常解析后，其graph_dict_成员生成了
    b、graph.h/graph.cc：描述了每一张图，graph_name_表示图的名称是什么，stage_vec_表示存储了每个执行阶段都要执行什么，它是GraphManager的推导生成结果
    c、每个Graph保护若干个stage即GraphStage，每个GraphStage包含若干个Node即GraphNode
    d、每个GraphNode指定其包含哪些op即op_name_list_
    e、图的执行参考下面【组图】的举例

【组图】
在配置文件配置组图，组图文件依据configuration/service_configuration.proto的类型，规则是：
    a、GraphConfs代表你可以组多张图，对应不同的rpc请求，这样在接收请求处理时，选择自己需要执行哪张图即可
    b、每张图由一个GraphConf代表，name是图的名称，nodes是图中全部节点，每个节点是一个DagNode类型
    c、每个DagNode类型的节点，name指定了节点的名称，op_group指定了该节点都有哪些算子即op，dependencies指定了该节点的执行，依赖哪些其他节点
    d、每个节点内的op_group是串行执行的
    e、节点之间是串行还是并行执行，由你的图推导得出，比如两个节点B和C，都只是依赖节点A，那么在A运行后，B和C是会并行执行
    f、stage表示一个执行流程，不同stage是串行执行的，每个stage内部可以是1或多个节点，如果是多个节点则是并行执行

举例如core/test/sample.prototxt，组图配置文件是你的基于proto实现的组图的debugString()的输出。后续将提供更加简单的工具，无需手动生成组图配置文件
    a、init1和init2两节点，无任何依赖，所以它们并行执行，作为stage1
    b、ann节点，依赖init1和init2节点，所以它会在init1、init2节点执行结束后执行，作为stage2
    c、ann节点，内部有3个算子，它们是串行执行的
    d、同理，short_filter和long_filter两节点，它们都依赖ann节点，所以在ann节点执行后并行执行，它们内部的op是串行执行的，作为stage3
    f、同理，udf依赖short_filter和long_filiter两节点，所以在short_filter和long_filter两节点执行完成后执行，作为stage4
    g、同理，response依赖udf，所以在udf节点执行完成后执行，作为stage5

【graph_instance && op】
    GraphInstance是运行时概念，即收到请求后，根据该请求对应的组图，实例化产生的GraphInstance，作为图执行的context上下文
    op是具体自定义实现的算子，仅有两个要求：
        a、自定义op类，需继承hawking::graph::Op<自定义op类的输出数据结构>>
           如：
            struct RetrieveContext {
                std::unordered_set<int64_t> filter_id_set;
                std::vector<std::string> udf_need_column_list;
            };
            class {自定义op类名称} : public hawking::graph::Op<RetrieveContext>
        b、无需定义构造函数，只需在public区域中写上：DECLARE_OP({自定义op类名称})
        c、把具体执行的内容，写在自定义op类的implement函数中，正常则返回true，不符合预期返回false，返回false后续的执行将终止

【编译 && 运行】
    mkdir build && cd build && cmake .. && make -j
    会生成libhawking-graph.a和相关单测

【TODO】
    至少如下五项：
    a、将丰富单测
    b、将不足sample，给出一个完整的rpc服务的使用组图的例子
    c、增加从graph、stage、node、op各层级粒度的bvar统一埋点统计，包括默认实现好的耗时统计
    d、丰富trace功能的外部使用api
    e、补充生成组图的tool，避免自己手写组图的debugString()输出