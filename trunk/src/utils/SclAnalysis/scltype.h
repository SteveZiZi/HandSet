/// @file
///     scltypes.h
///
/// @brief
///     用于SCL文件解析的自定义类型
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.1.22
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.02.01     |1.0.1.201  |增加SCL文件中DataTypeTemplates部分的解析数据的结构体定义  |
///    |2013.02.06     |1.0.2.206  |增加SCL文件中逻辑节点所绑定的外部信号（Inputs）的结构体定义  |
///    |2013.06.26     |1.0.3.626  |增加SCL文件中子网属性的结构体定义                         |
///

#ifndef SCL_ELEM_ATTRIB_H_

#define SCL_ELEM_ATTRIB_H_

#ifndef _T
#include <tchar.h>
#endif

//std::wstring
#include <string>
//MCD_CSTR_FILENAME
//MCD_CSTR
#include "src/utils/CMarkup/Markup.h"

#define SCL_CSTR_FILENAME MCD_CSTR_FILENAME   ///< 自定义文件路径类型，MCD_XX 为MarkUp中定义的类型
#define SCL_CSTR          MCD_CSTR            ///< 自定义元素标签类型
#define SCL_STR           std::wstring        ///< 自定义字符串类型
#define SCL_SPTINTF       swprintf_s
#define SCL_SSCANF        swscanf_s

///
/// @brief
///     IED（智能电子设备）的属性
///
typedef struct  
{
	SCL_STR iedName;          ///< IED实例的名称
	SCL_STR iedDesc;          ///< 该IED实例的描述
	SCL_STR manufactactuer;   ///< 制造商名的文字描述
	SCL_STR configVersion;    ///< IED配置的基本配置版本信息，仅标识IED的基本配置（由制造商规定/提供的能力）
}SCL_IED_ATTRIB;

///
/// @brief
///     AccessPoint（访问点）的属性
///
typedef struct
{
	SCL_STR apName;           ///< IED内的访问点实例，应在IED内保持唯一。
	SCL_STR apDesc;           ///< 对该访问点的文字描述
	bool    clock;            ///< 辨识该IED是否为所在子网上的主时钟。缺省为“false”，非主时钟。
	bool    router;           ///< 表示该IED是否有路由器功能。缺省为“false”，无路由器功能。
}SCL_ACCESS_POINT_ATTRIB;

///
/// @brief
///     Server（服务器）的属性
///
typedef struct
{
	SCL_STR serverDesc;       ///< 服务器的文字描述
	SCL_STR timeout;          ///< 超时时间，单位为s。如果已启动的处理在该时间内未完成（如选择一定值组），则取消该处理并复归
}SCL_SERVER_ATTRIB;

///
/// @brief
///     LDevice（逻辑设备）的属性
///
typedef struct
{
	SCL_STR ldInst;           ///< LDevice的实例号
	SCL_STR ldDesc;           ///< LDevice的文字描述
}SCL_LDEVICE_ATTRIB;

///
/// @brief
///     LN0（逻辑节点LLN0）的属性
///
typedef struct
{
	SCL_STR lnClass;          ///< IEC61850-7定义的逻辑节点类。这里固定为LLN0
	SCL_STR lnInst;           ///< 该逻辑节点类的实例号。对LLN0，固定为空字符串。
	SCL_STR lnType;           ///< 该逻辑节点实例化的类型定义，引用LNodeType元素的定义
	SCL_STR lnDesc;           ///< 文字描述
}SCL_LN0_ATTRIB;

///
/// @brief
///     LN（LLN0外的其他逻辑节点）的属性
///
typedef struct
{
	SCL_STR lnPrefix;         ///< 逻辑节点前缀
	SCL_STR lnClass;          ///< IEC61850-7定义的逻辑节点类
	SCL_STR lnInst;           ///< 该逻辑节点类的实例号
	SCL_STR lnType;           ///< 该逻辑节点实例化的类型定义，引用LNodeType元素的定义
	SCL_STR lnDesc;           ///< 文字描述
}SCL_LN_ATTRIB;

///
/// @brief
///     DataSet（数据集，一般在逻辑节点中定义）的属性
///
typedef struct
{
	SCL_STR dataSetName;      ///< 该数据集的名称
	SCL_STR	dataSetDesc;      ///< 该数据集的文字描述
	int     fcdaNum;          ///< 该数据集包含的FCDA的数目
}SCL_DATASET_ATTRIB;

///
/// @brief
///     FCDA（数据集的成员的内部引用地址）的属性
///
typedef struct
{
	int     idxSeq;           ///< 该FCDA在DataSet中的顺序号
	SCL_STR ldInst;           ///< 该FCDA所指向的数据所属的逻辑设备的实例号
	SCL_STR lnPrefix;         ///< 该FCDA所指向的数据所属的逻辑节点的前缀
	SCL_STR lnClass;          ///< 该FCDA所指向的数据所属的逻辑节点的逻辑节点类
	SCL_STR lnInst;           ///< 该FCDA所指向的数据所属的逻辑节点的实例号
	SCL_STR doName;           ///< 该FCDA所指向的数据（DOI）的名称
	SCL_STR daName;           ///< 该FCDA所指向的数据属性（DAI）的名称
	SCL_STR fc;               ///< 该FCDA所指向的数据的功能约束
}SCL_FCDA_ATTRIB;

///
/// @brief
///     DOI（数据实例）的属性
///
typedef struct 
{
	SCL_STR doName;           ///< 该数据的名称
	SCL_STR doDesc;           ///< 该数据的文字描述
//	SCL_STR accessControl;    ///< 该数据的访问控制定义
//	SCL_STR ix;               ///< 数据类型中数据元素的下标
	SCL_STR dUVal;            ///< 该数据中数据属性dU的值
	SCL_STR dUSAddr;          ///< 该数据中数据属性dU的sAddr属性值
}SCL_DOI_ATTRIB;

///
/// @brief
///     DAI（数据属性）的属性
///
typedef struct
{
	SCL_STR daName;           ///< 该数据属性的名称
	SCL_STR daDesc;           ///< 该数据属性的文字描述
	SCL_STR sAddr;            ///< 该数据属性的短地址，由用户定义
	SCL_STR daBType;          ///< 该数据属性所属IEC61850-7部分规定的基本数据类型
//	SCL_STR valKind;          ///< 如果给定名称，则表示工程阶段该值的含义
//	SCL_STR ix;               ///< 数据类型中DAI元素的下标
//	SCL_STR daVal;            ///< 默认值
}SCL_DAI_ATTRIB;

///
/// @brief
///     逻辑节点所绑定的外部信号(Inputs)中某一连线（ExtRef）的结构体定义
///
typedef struct
{
	int     idxSeq;           ///< 外部信号在Inputs中的顺序号，从0开始
	SCL_STR desc;             ///< 对该连线（绑定）的文字描述
	SCL_STR iedName;          ///< 外部信号所在的IED的名称
	SCL_STR ldInst;           ///< 外部信号所在逻辑设备的实例号
	SCL_STR lnPrefix;         ///< 外部信号所在逻辑节点的前缀
	SCL_STR lnClass;          ///< 外部信号所在逻辑节点所属的逻辑节点类
	SCL_STR lnInst;           ///< 外部信号所在逻辑节点的实例号
	SCL_STR doName;           ///< 外部信号所标识的DO的名称
	SCL_STR daName;           ///< 外部信号所标识的DA的名称。其值为空时，表示外部信号为DO的所有数据属性。
	SCL_STR intAddr;          ///< 外部信号所绑定的内部地址
}SCL_EXTREF_ATTRIB;

///
/// @brief
///     SampledValueControl（采样值控制块）的属性
///
typedef struct 
{
	SCL_STR cbName;           ///< 控制块名称
	SCL_STR cbDesc;           ///< 控制块的文字描述
	SCL_STR confRev;          ///< 控制块的配置版本号
	SCL_STR smvID;            ///< 控制块的ID，一般为“LDName/LNName.SMVCBName”
	SCL_STR dataSet;          ///< 应被SV报文发送的数据集的名称
	SCL_STR smpRate;          ///< 符合IEC61850-7-2定义的采样速率
	SCL_STR nofASDU;          ///< 代表SV报文中ASDU的数量
	bool    multicast;        ///< 表示采用的是多播SMV服务还是单播SMV服务
	bool    smvOptReftTm;     ///< SMV报文中是否包含刷新时间
	bool    smvOptSmpRate;    ///< SMV报文中是否包含采样率
	bool    smvOptSmpSync;    ///< SMV报文中是否包含同步采样
	bool    smvOptSecurity;   ///< SMV报文中是否包含安全项
	bool    smvOptDataRef;    ///< SMV报文中是否包含应被SV报文发送的数据集的引用（名称）
}SCL_SMVCTRL_ATTRIB;

///
/// @brief
///     子网（SubNetwork）的属性
///
typedef struct
{
	SCL_STR name;             ///< 子网的名称
 	SCL_STR type;             ///< 子网的类型
// 	SCL_STR desc;             ///< 描述
}SCL_SUBNETWORK_ATTRIB;

///
/// @brief
///     连接在子网（SubNetwork）上的访问点的引用
///
typedef struct
{
	SCL_STR iedName;          ///< 连接在子网上的访问点所属的IED的名称
	SCL_STR apName;           ///< 连接在子网上的访问点的名称
}SCL_CONNECTEDAP_ATTRIB;

///
/// @brief
///     GSEControl（GSE控制块）的属性
///
typedef struct
{
	SCL_STR cbName;           ///< 控制块的名称
	SCL_STR cbDesc;           ///< 控制块的文字描述
	SCL_STR confRev;          ///< 控制块的配制版本号
	SCL_STR gooseID;          ///< 控制块ID
	SCL_STR dataSet;          ///< 应被GOOSE/GSSE报文发送的数据集的名称
	SCL_STR type;             ///< 控制块类型，有“GOOSE”和“GSSE”两类
}SCL_GSECTRL_ATTRIB;

///
/// @brief
///     SVM传输地址以及控制该传输服务的控制块信息
///
typedef struct
{
	SCL_STR macAddr;          ///< 介质访问地址值，为6组被-分开的2个字符，字符为0~9和A~F
	SCL_STR appID;            ///< 应用标识，为4个由0~9和A~F组成的字符
	SCL_STR vlanID;           ///< VLAN ID，为3个0~9和A~F组成的字符
	SCL_STR vlanPriority;     ///< VLAN 用户优先级，为0~7的单字符
	SCL_STR ldInst;           ///< 控制该传输服务的控制块所属的逻辑设备的实例号
	SCL_STR cbName;           ///< 控制该传输服务的控制块的名称
}SCL_SMV_ATTRIB;

///
/// @brief
///     GOOSE/GSSE传输地址以及控制该传输服务的控制块信息
///
typedef struct
{
	SCL_STR macAddr;          ///< 介质访问地址值，为6组被-分开的2个字符，字符为0~9和A~F
	SCL_STR appID;            ///< 应用标识，为4个由0~9和A~F组成的字符
	SCL_STR vlanID;           ///< VLAN ID，为3个0~9和A~F组成的字符
	SCL_STR vlanPriority;     ///< VLAN 用户优先级，为0~7的单字符
	SCL_STR minTimeVal;       ///< GOOSE数据变位后立即补发的时间间隔
	SCL_STR minTimeUnit;      ///< minTime时间间隔的单位
	SCL_STR maxTimeVal;       ///< 心跳时间T0
	SCL_STR maxTimeUnit;      ///< 心跳时间的单位
	SCL_STR ldInst;           ///< 控制该传输服务的控制块所属的逻辑设备的实例号
	SCL_STR cbName;           ///< 控制该传输服务的控制块的名称
}SCL_GSE_ATTRIB;

///
/// @brief
///     客户/服务器传输地址
///
typedef struct
{
	SCL_STR ip;                ///< IP地址
	SCL_STR ipSubnet;          ///< TCP/IP集的子网掩码
	SCL_STR	ipGateway;         ///< TCP/IP集的第一跳网关地址
	SCL_STR osiNsap;           ///< OSI网络地址
	SCL_STR	osiTsel;           ///< OSI传输层选择符
	SCL_STR	osiSsel;           ///< OSI会话层选择符
	SCL_STR	osiPsel;           ///< OSI表示层选择符
	SCL_STR osiApTitle;        ///< OSI ACSE应用名称值
	SCL_STR osiApInvoke;       ///< OSI ACSE应用调用ID
	SCL_STR	osiAeQualifier;    ///< OSI ACSE AE限定语
	SCL_STR	osiAeInvoke;       ///< OSI ACSE AE调用ID
}SCL_ADDR_ATTRIB;

///
/// @brief
///     逻辑节点类型的属性信息
///
typedef struct
{
	SCL_STR id;                ///< 该逻辑节点类型的ID
	SCL_STR desc;              ///< 描述信息
//	SCL_STR iedType;           ///< 该逻辑节点类型所属IED的制造商IED设备类型,不常用
	SCL_STR lnClass;           ///< 该逻辑节点所属协议规定的基本类
}SCL_LNTYPE_ATTRIB;

///
/// @brief
///     逻辑节点类型中某一DO的属性信息
///
typedef struct
{
	SCL_STR name;              ///< 逻辑节点类型中某DATA名称
	SCL_STR type;              ///< 所属的DOType的ID
	SCL_STR desc;              ///< 描述信息
//	SCL_STR accessControl;     ///< 该DO的访问控制，不常用
//	SCL_STR transient;         ///< 为true时表示使用瞬变定义，不常用
}SCL_DOINLNTYPE_ATTRIB;

///
/// @brief
///     DOType的属性信息
///
typedef struct
{
	SCL_STR id;                ///< DOType的ID
//	SCL_STR iedType;           ///< 该DOType所属的IED的类型，不常用
	SCL_STR cdc;               ///< 该DOType所属协议规定的基本公共数据类
}SCL_DOTYPE_ATTRIB;

///
/// @brief
///     DOType中某一DA的属性信息
///
typedef struct
{
	SCL_STR name;              ///< 该属性的名称
	SCL_STR desc;              ///< 文字说明
	SCL_STR fc;                ///< 功能约束
	SCL_STR bType;             ///< 该属性的基本本类型
	SCL_STR type;              ///< 该属性所属的自定义类型，bType=Enum或Struct时
}SCL_DAINDOTYPE_ATTRIB;

///
/// @brief
///     DAType的属性信息
///
typedef struct
{
	SCL_STR id;                ///< DAType的ID
//	SCL_STR iedType;           ///< 该DOType所属的IED的类型，不常用
	SCL_STR desc;              ///< 文字描述
}SCL_DATYPE_ATTRIB;

///
/// @brief
///     DAType中某一BDA的属性信息
///
typedef struct
{
	SCL_STR name;              ///< 名称
//	SCL_STR desc;              ///< 文字说明
//	SCL_STR sAddr;             ///< 可选短地址
	SCL_STR bType;             ///< 该属性的基本本类型
	SCL_STR type;              ///< 该属性所属的自定义类型，bType=Enum或Struct时
//	SCL_STR count;             ///< 该属性为数组时，表示数组元素的数目
//	SCL_STR valKind;           ///< 决定如何解释被给出的值
}SCL_BDAINDATYPE_ATTRIB;

///
/// @brief
///     EnumType中某一EnumVal的属性信息
///
typedef struct
{
	SCL_STR id;                ///< EnumVal所属的EnumType的ID
	SCL_STR desc;              ///< EnumVal所属的EnumType的描述
	SCL_STR ord;               ///< EnumVal的标签值
	SCL_STR enumVal;           ///< EnumVal的实际含义
}SCL_ENUMVAL_ATTRIB;

#endif
