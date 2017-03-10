在前面的SCL文件解析库的基础上，优化解析过程，减少需要缓存的数据，基本实现SCL文件的删减功能。

SCL解析保留的内容：
1、Communication->SubNetwork->ConnectedAP中的全部SMV和GOOSE（通信参数）；
2、IED->AccessPoint->Server->LDevice中的LN0->GSEControl/SampledValueControl；
3、IED中与GSEControl/SampledValueControl相关的数据集及各数据项（FCDA）所指向的DOI；
4、IED中与GSEControl/SampledValueControl相关的Inputs及Inputs->ExtRef所指向的DOI；
   注：《IEC61850应用入门》：一般在每个装置的LLN0的Inputs部分定义该装置的Goose和采样值输入连线信息，且输入虚端子的逻辑节点的前缀一般含有“GOIN”和“SVIN”，P226~P277和P301~302。
5、IED、LDevice、LN0、LN、DataSet等的一些属性，如desc、name等，具体见scltypes.h的定义。
6、DataTypeTemplates部分，全部的LNType、DOType、DAType，但各部分并非取全部的内容，具体见scltypes.h的定义。

目前测试结果如下：

旧版本解析结果：
解析许继发过来的SCD文件（347k）：耗时：300~400描述；内存为原来的1.5倍。
解析5M的SCD文件：                耗时：5~6s。
解析10M的SCD文件：               耗时：10s。

新版本解析结果：
解析许继发过来的SCD文件（347k）：耗时：255~300ms；内存：216K(+Inputs) \208K(-Inputs)。
解析5120k的SCD文件：             耗时：2.6s；     内存：2666K(+Inputs)\2666K(-Inputs)。
解析10147k的SCD文件：            耗时：4.0s；     内存：2673K(+Inputs)\2673K(-Inputs)。

SCL文件删减结果：
220kV桐子湾变201303081044.scd（34.6M）：3.01M（+Inputs）、2.13M（-Inputs）；
2012102401leigu.scd（29.9M）：          4.47M（+Inputs）、4.23M（-Inputs）；
WC1016B.scd（5M）：                     564K（+Inputs） 、564K（-Inputs）。
WC1016B_2.scd（10147K）：               564K（+Inputs） 、564K（-Inputs）。

使用简化后的SCL文件进行解析，需要的时间比原文件少，解析后的内存与原文件解析后的内存相差不大，大概为简化后文件的3~4倍，如下：
220kV桐子湾变201303081044.scd（2.13M）：耗时：6.9s； 内存：6774K 。
2012102401leigu.scd（4.23M）：          耗时：13.7s；内存：16160K。
WC1016B.scd（564K）：                   耗时：1.3s； 内存：2608K 。