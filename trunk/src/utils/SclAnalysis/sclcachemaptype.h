#ifndef SCL_CAHCE_MAP_H_

#define SCL_CAHCE_MAP_H_

/// @file
///     sclcachemaptype.cpp
///
/// @brief
///     用于构建SCL文件解析数据缓存区的自定义类型
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.1.24
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:-------------------------------------------------------------------
///    |2013.02.01     |1.0.1.201  |增加LNMap，存放DOI信息                                             |
///    |2013.02.01     |1.0.1.201  |由于FCDA指向的DOI与FCDA不在同一层，解析会消耗时间，故将其拆分到LN中|
///    |2013.02.01     |1.0.1.201  |增加DataTypeTemplates部分，存放数据类型信息                        |
///    |2013.02.06     |1.0.2.206  |增加ExtRef Map部分，存放逻辑节点所绑定的外部信号                   |
///    |2013.06.01     |1.0.4.601  |由于索引为字符串的Map的顺序为字符串顺序，修改Cache中缓存数据类型所含有的数据属性的信息的Map的索引类型为int|
///    |2013.08.09     |1.0.5.809  |由于可能存在多个相同AppID，故更改SMV和GOOSE的通信参数信息的缓存结构为链表|
///

#include <map>

///
/// @brief
///     定义Cache中缓存FCDA信息的结构体及以FCDA的顺序号为key的MAP
///
typedef struct
{
	SCL_FCDA_ATTRIB       fcdaAttrib;     ///< FCDA元素的属性信息
// 	SCL_DOI_ATTRIB        doiAttrib;      ///< FCDA中doName所指向的数据的信息
// 	SCL_DAINDOTYPE_ATTRIB daAttrib;       ///< FCDA中daName所指向的数据属性的类型信息
}FCDA_INFO_CACHE;
typedef std::map < int, FCDA_INFO_CACHE * > FCDA_MAP;

///
/// @brief
///     定义Cache中缓存DataSet信息的结构体及以DataSetName为key的Map
///
typedef struct
{
	SCL_DATASET_ATTRIB datSetAttrib;      ///< DataSet元素的属性信息
	FCDA_MAP          *fcdaMap;           ///< DataSet中包含的FCDA的Map
}DATASET_INFO_CACHE;
typedef std::map < SCL_STR, DATASET_INFO_CACHE * > DATASET_MAP;


///
/// @brief
///     定义Cache缓存SMV控制块信息的Map，以控制块Name为key
///
typedef std::map < SCL_STR, SCL_SMVCTRL_ATTRIB > SMV_CTRL_MAP;

///
/// @brief
///     定义Cache中缓存GSE控制块信息的Map，以控制块Name为key
///
typedef std::map < SCL_STR, SCL_GSECTRL_ATTRIB > GSE_CTRL_MAP;

///
/// @brief
///     定义Cache中缓存数据（DO）实例的Map，以doName为key
///
typedef std::map < SCL_STR, SCL_DOI_ATTRIB > DOI_MAP;

///
/// @brief
///     定义Cache中缓存逻辑节点所绑定的外部信号的Map，以顺序号为key
///
typedef std::map < int, SCL_EXTREF_ATTRIB > EXTREF_MAP;

///
/// @brief
///     定义Cache中缓存LN0信息的结构体
///
typedef struct
{
	SCL_LN0_ATTRIB ln0Attrib;             ///< LN0元素的属性信息
	DATASET_MAP   *datasetMap;            ///< LN0中包含的DataSet的Map
	SMV_CTRL_MAP  *smvCtrlMap;            ///< LN0中包含的SampledValueControl的Map
	GSE_CTRL_MAP  *gseCtrlMap;            ///< LN0中包含的GSEControl的Map
	DOI_MAP       *doiMap;                ///< LLN0逻辑节点中定义的数据实例
	EXTREF_MAP    *extRefMap;             ///< LLN0所绑定的外部信号的Map
}LN0_INFO_CACHE;

///
/// @brief
///     定义Cache中缓存LN0之外的逻辑节点的信息的结构体及以lnName为key的Map
///
typedef struct
{
	SCL_LN_ATTRIB lnAttrib;               ///< 逻辑节点的属性信息
	DOI_MAP      *doiMap;                 ///< 逻辑节点中定义的数据实例
	EXTREF_MAP   *extRefMap;              ///< 逻辑节点所绑定的外部信号的Map
}LN_INFO_CACHE;
typedef std::map < SCL_STR, LN_INFO_CACHE * > LN_MAP;

///
/// @brief
///     定义Cache中缓存LDevice信息的结构体及以ldInst为key的Map
///
typedef struct
{
	SCL_ACCESS_POINT_ATTRIB apAttrib;     ///< LDevice元素所连接的访问点的属性
	SCL_LDEVICE_ATTRIB      ldAttrib;     ///< LDevice元素的属性
	LN0_INFO_CACHE         *ln0InfoCache; ///< LDevice中指向LN0的指针
	LN_MAP                 *lnMap;        ///< LDevice中LLN0之外的逻辑节点的Map
}LDEVICE_INFO_CACHE;
typedef std::map < SCL_STR, LDEVICE_INFO_CACHE * > LDEVICE_MAP;

///
/// @brief
///     定义Cache中缓存IED信息的结构体及以iedName为key的Map
///
typedef struct
{
	SCL_IED_ATTRIB iedAttrib;             ///< IED元素的属性信息
	LDEVICE_MAP   *lDeviceMap;            ///< IED中包含的LDevice的Map
}IED_INFO_CACHE;
typedef std::map < SCL_STR, IED_INFO_CACHE * > IED_MAP;

///
/// @brief
///     定义Cache中缓存SMV通信参数信息的结构体及以AppID为key的Map，与控制块一一对应
///
typedef struct _SMV_COMMN_INFO_CACHE
{
	SCL_CONNECTEDAP_ATTRIB cnntAPAttrib;  ///< 某IED在子网上的访问点的引用
	SCL_SMV_ATTRIB         smvAttrib;     ///< IED中某SMV控制块所控制服务的通信参数及控制块引用
	_SMV_COMMN_INFO_CACHE *pNextCache;    ///< 指向下一个SMV通信参数信息缓存的指针
}SMV_COMMN_INFO_CACHE;
typedef std::map < unsigned short, SMV_COMMN_INFO_CACHE * > SMV_COMMN_MAP;

///
/// @brief
///     定义Cache中缓存G00SE/GSE通信参数信息的结构体及以AppID为Key的Map，与控制块一一对应
///
typedef struct _GSE_COMMN_INFO_CACHE
{
	SCL_CONNECTEDAP_ATTRIB cnntAPAttrib;  ///< 某IED在子网上的访问点的引用
	SCL_GSE_ATTRIB         gseAttrib;     ///< IED中某GSE控制块所控制服务的通信参数及控制块引用
	_GSE_COMMN_INFO_CACHE *pNextCache;    ///< 指向下一个GSE通信参数信息缓存的指针
}GSE_COMMN_INFO_CACHE;
typedef std::map < unsigned short, GSE_COMMN_INFO_CACHE * > GSE_COMMN_MAP;

///
/// @brief
///     定义Cache中缓存某一子网的通信参数信息的结构体及以subNetwork的Name为Key的Map
///
typedef struct
{
	SCL_SUBNETWORK_ATTRIB subNetAttrib;
	SMV_COMMN_MAP        *smvCommnMap;
	GSE_COMMN_MAP        *gseCommnMap; 
}SUBNETWORK_INFO_CACHE;
typedef std::map < SCL_STR ,SUBNETWORK_INFO_CACHE * > SUBNETWORK_MAP;

///
/// @brief
///     定义Cache中缓存逻辑节点类型所含有的数据的Map，以doName为Key
///
typedef std::map < SCL_STR, SCL_DOINLNTYPE_ATTRIB > DO_MAP;

///
/// @brief
///     定义Cache中缓存逻辑节点类型信息的结构体及以lnType的ID为key的Map
///
typedef struct
{
	SCL_LNTYPE_ATTRIB lnTypeAttrib;       ///< 逻辑节点类型的属性信息
	DO_MAP           *doMap;              ///< 逻辑节点类型所含有的数据的Map
}LNTYPE_INFO_CACHE;
typedef std::map < SCL_STR, LNTYPE_INFO_CACHE * > LNTYPE_MAP;

///
/// @brief
///     定义Cache中缓存数据类型所含有的数据属性的信息的Map，以da的顺序号为key
///
typedef std::map < int, SCL_DAINDOTYPE_ATTRIB > DA_MAP;

///
/// @brief
///     定义Cache中缓存数据类型信息的结构体及以doType的ID为key的Map
///
typedef struct
{
	SCL_DOTYPE_ATTRIB doTypeAttrib;       ///< 数据类型的属性信息
	DA_MAP           *daMap;              ///< 数据类型所含有的数据属性的信息的Map
}DOTYPE_INFO_CACHE;
typedef std::map < SCL_STR, DOTYPE_INFO_CACHE * > DOTYPE_MAP;

///
/// @brief
///     定义Cache中缓存数据属性所含有的基本数据信息的Map，以bda的顺序号为key
///
typedef std::map < int, SCL_BDAINDATYPE_ATTRIB > BDA_MAP; 

///
/// @brief
///     定义Cache中缓存数据属性类型信息的结构体及以DaType的ID为Key的Map
///
typedef struct
{
	SCL_DATYPE_ATTRIB daTypeAttrib;       ///< 数据属性类型的属性信息
	BDA_MAP          *bdaMap;             ///< 数据属性所含有的基本数据信息的Map
}DATYPE_INFO_CACHE;
typedef std::map < SCL_STR, DATYPE_INFO_CACHE * > DATYPE_MAP;

///
/// @brief
///     定义Cache中缓存整个SCL文件中所关注信息的结构体
///
typedef struct
{
	IED_MAP        *iedMap;                ///< SCL文件中所有IED组成的Map
//	SUBNETWORK_MAP *subNetMap;             ///< SCL文件中所有子网的通信参数信息的Map
	SMV_COMMN_MAP  *smvCommnMap;           ///< SCL文件中所有SMV控制块所控制服务的通信参数信息的Map
	GSE_COMMN_MAP  *gseCommnMap;           ///< SCL文件中所有GSE控制块所控制服务的通信参数信息的Map
	LNTYPE_MAP     *lnTypeMap;             ///< SCL文件中所有逻辑节点类型信息的Map
	DOTYPE_MAP     *doTypeMap;             ///< SCL文件中所有数据类型信息的Map
	DATYPE_MAP     *daTypeMap;             ///< SCL文件中所有数据属性类型信息的Map
}SCL_MAP;

#endif