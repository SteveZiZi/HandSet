#ifndef SCL_CAHCE_MAP_H_

#define SCL_CAHCE_MAP_H_

/// @file
///     sclcachemaptype.cpp
///
/// @brief
///     ���ڹ���SCL�ļ��������ݻ��������Զ�������
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.1.24
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:-------------------------------------------------------------------
///    |2013.02.01     |1.0.1.201  |����LNMap�����DOI��Ϣ                                             |
///    |2013.02.01     |1.0.1.201  |����FCDAָ���DOI��FCDA����ͬһ�㣬����������ʱ�䣬�ʽ����ֵ�LN��|
///    |2013.02.01     |1.0.1.201  |����DataTypeTemplates���֣��������������Ϣ                        |
///    |2013.02.06     |1.0.2.206  |����ExtRef Map���֣�����߼��ڵ����󶨵��ⲿ�ź�                   |
///    |2013.06.01     |1.0.4.601  |��������Ϊ�ַ�����Map��˳��Ϊ�ַ���˳���޸�Cache�л����������������е��������Ե���Ϣ��Map����������Ϊint|
///    |2013.08.09     |1.0.5.809  |���ڿ��ܴ��ڶ����ͬAppID���ʸ���SMV��GOOSE��ͨ�Ų�����Ϣ�Ļ���ṹΪ����|
///

#include <map>

///
/// @brief
///     ����Cache�л���FCDA��Ϣ�Ľṹ�弰��FCDA��˳���Ϊkey��MAP
///
typedef struct
{
	SCL_FCDA_ATTRIB       fcdaAttrib;     ///< FCDAԪ�ص�������Ϣ
// 	SCL_DOI_ATTRIB        doiAttrib;      ///< FCDA��doName��ָ������ݵ���Ϣ
// 	SCL_DAINDOTYPE_ATTRIB daAttrib;       ///< FCDA��daName��ָ����������Ե�������Ϣ
}FCDA_INFO_CACHE;
typedef std::map < int, FCDA_INFO_CACHE * > FCDA_MAP;

///
/// @brief
///     ����Cache�л���DataSet��Ϣ�Ľṹ�弰��DataSetNameΪkey��Map
///
typedef struct
{
	SCL_DATASET_ATTRIB datSetAttrib;      ///< DataSetԪ�ص�������Ϣ
	FCDA_MAP          *fcdaMap;           ///< DataSet�а�����FCDA��Map
}DATASET_INFO_CACHE;
typedef std::map < SCL_STR, DATASET_INFO_CACHE * > DATASET_MAP;


///
/// @brief
///     ����Cache����SMV���ƿ���Ϣ��Map���Կ��ƿ�NameΪkey
///
typedef std::map < SCL_STR, SCL_SMVCTRL_ATTRIB > SMV_CTRL_MAP;

///
/// @brief
///     ����Cache�л���GSE���ƿ���Ϣ��Map���Կ��ƿ�NameΪkey
///
typedef std::map < SCL_STR, SCL_GSECTRL_ATTRIB > GSE_CTRL_MAP;

///
/// @brief
///     ����Cache�л������ݣ�DO��ʵ����Map����doNameΪkey
///
typedef std::map < SCL_STR, SCL_DOI_ATTRIB > DOI_MAP;

///
/// @brief
///     ����Cache�л����߼��ڵ����󶨵��ⲿ�źŵ�Map����˳���Ϊkey
///
typedef std::map < int, SCL_EXTREF_ATTRIB > EXTREF_MAP;

///
/// @brief
///     ����Cache�л���LN0��Ϣ�Ľṹ��
///
typedef struct
{
	SCL_LN0_ATTRIB ln0Attrib;             ///< LN0Ԫ�ص�������Ϣ
	DATASET_MAP   *datasetMap;            ///< LN0�а�����DataSet��Map
	SMV_CTRL_MAP  *smvCtrlMap;            ///< LN0�а�����SampledValueControl��Map
	GSE_CTRL_MAP  *gseCtrlMap;            ///< LN0�а�����GSEControl��Map
	DOI_MAP       *doiMap;                ///< LLN0�߼��ڵ��ж��������ʵ��
	EXTREF_MAP    *extRefMap;             ///< LLN0���󶨵��ⲿ�źŵ�Map
}LN0_INFO_CACHE;

///
/// @brief
///     ����Cache�л���LN0֮����߼��ڵ����Ϣ�Ľṹ�弰��lnNameΪkey��Map
///
typedef struct
{
	SCL_LN_ATTRIB lnAttrib;               ///< �߼��ڵ��������Ϣ
	DOI_MAP      *doiMap;                 ///< �߼��ڵ��ж��������ʵ��
	EXTREF_MAP   *extRefMap;              ///< �߼��ڵ����󶨵��ⲿ�źŵ�Map
}LN_INFO_CACHE;
typedef std::map < SCL_STR, LN_INFO_CACHE * > LN_MAP;

///
/// @brief
///     ����Cache�л���LDevice��Ϣ�Ľṹ�弰��ldInstΪkey��Map
///
typedef struct
{
	SCL_ACCESS_POINT_ATTRIB apAttrib;     ///< LDeviceԪ�������ӵķ��ʵ������
	SCL_LDEVICE_ATTRIB      ldAttrib;     ///< LDeviceԪ�ص�����
	LN0_INFO_CACHE         *ln0InfoCache; ///< LDevice��ָ��LN0��ָ��
	LN_MAP                 *lnMap;        ///< LDevice��LLN0֮����߼��ڵ��Map
}LDEVICE_INFO_CACHE;
typedef std::map < SCL_STR, LDEVICE_INFO_CACHE * > LDEVICE_MAP;

///
/// @brief
///     ����Cache�л���IED��Ϣ�Ľṹ�弰��iedNameΪkey��Map
///
typedef struct
{
	SCL_IED_ATTRIB iedAttrib;             ///< IEDԪ�ص�������Ϣ
	LDEVICE_MAP   *lDeviceMap;            ///< IED�а�����LDevice��Map
}IED_INFO_CACHE;
typedef std::map < SCL_STR, IED_INFO_CACHE * > IED_MAP;

///
/// @brief
///     ����Cache�л���SMVͨ�Ų�����Ϣ�Ľṹ�弰��AppIDΪkey��Map������ƿ�һһ��Ӧ
///
typedef struct _SMV_COMMN_INFO_CACHE
{
	SCL_CONNECTEDAP_ATTRIB cnntAPAttrib;  ///< ĳIED�������ϵķ��ʵ������
	SCL_SMV_ATTRIB         smvAttrib;     ///< IED��ĳSMV���ƿ������Ʒ����ͨ�Ų��������ƿ�����
	_SMV_COMMN_INFO_CACHE *pNextCache;    ///< ָ����һ��SMVͨ�Ų�����Ϣ�����ָ��
}SMV_COMMN_INFO_CACHE;
typedef std::map < unsigned short, SMV_COMMN_INFO_CACHE * > SMV_COMMN_MAP;

///
/// @brief
///     ����Cache�л���G00SE/GSEͨ�Ų�����Ϣ�Ľṹ�弰��AppIDΪKey��Map������ƿ�һһ��Ӧ
///
typedef struct _GSE_COMMN_INFO_CACHE
{
	SCL_CONNECTEDAP_ATTRIB cnntAPAttrib;  ///< ĳIED�������ϵķ��ʵ������
	SCL_GSE_ATTRIB         gseAttrib;     ///< IED��ĳGSE���ƿ������Ʒ����ͨ�Ų��������ƿ�����
	_GSE_COMMN_INFO_CACHE *pNextCache;    ///< ָ����һ��GSEͨ�Ų�����Ϣ�����ָ��
}GSE_COMMN_INFO_CACHE;
typedef std::map < unsigned short, GSE_COMMN_INFO_CACHE * > GSE_COMMN_MAP;

///
/// @brief
///     ����Cache�л���ĳһ������ͨ�Ų�����Ϣ�Ľṹ�弰��subNetwork��NameΪKey��Map
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
///     ����Cache�л����߼��ڵ����������е����ݵ�Map����doNameΪKey
///
typedef std::map < SCL_STR, SCL_DOINLNTYPE_ATTRIB > DO_MAP;

///
/// @brief
///     ����Cache�л����߼��ڵ�������Ϣ�Ľṹ�弰��lnType��IDΪkey��Map
///
typedef struct
{
	SCL_LNTYPE_ATTRIB lnTypeAttrib;       ///< �߼��ڵ����͵�������Ϣ
	DO_MAP           *doMap;              ///< �߼��ڵ����������е����ݵ�Map
}LNTYPE_INFO_CACHE;
typedef std::map < SCL_STR, LNTYPE_INFO_CACHE * > LNTYPE_MAP;

///
/// @brief
///     ����Cache�л����������������е��������Ե���Ϣ��Map����da��˳���Ϊkey
///
typedef std::map < int, SCL_DAINDOTYPE_ATTRIB > DA_MAP;

///
/// @brief
///     ����Cache�л�������������Ϣ�Ľṹ�弰��doType��IDΪkey��Map
///
typedef struct
{
	SCL_DOTYPE_ATTRIB doTypeAttrib;       ///< �������͵�������Ϣ
	DA_MAP           *daMap;              ///< �������������е��������Ե���Ϣ��Map
}DOTYPE_INFO_CACHE;
typedef std::map < SCL_STR, DOTYPE_INFO_CACHE * > DOTYPE_MAP;

///
/// @brief
///     ����Cache�л����������������еĻ���������Ϣ��Map����bda��˳���Ϊkey
///
typedef std::map < int, SCL_BDAINDATYPE_ATTRIB > BDA_MAP; 

///
/// @brief
///     ����Cache�л�����������������Ϣ�Ľṹ�弰��DaType��IDΪKey��Map
///
typedef struct
{
	SCL_DATYPE_ATTRIB daTypeAttrib;       ///< �����������͵�������Ϣ
	BDA_MAP          *bdaMap;             ///< �������������еĻ���������Ϣ��Map
}DATYPE_INFO_CACHE;
typedef std::map < SCL_STR, DATYPE_INFO_CACHE * > DATYPE_MAP;

///
/// @brief
///     ����Cache�л�������SCL�ļ�������ע��Ϣ�Ľṹ��
///
typedef struct
{
	IED_MAP        *iedMap;                ///< SCL�ļ�������IED��ɵ�Map
//	SUBNETWORK_MAP *subNetMap;             ///< SCL�ļ�������������ͨ�Ų�����Ϣ��Map
	SMV_COMMN_MAP  *smvCommnMap;           ///< SCL�ļ�������SMV���ƿ������Ʒ����ͨ�Ų�����Ϣ��Map
	GSE_COMMN_MAP  *gseCommnMap;           ///< SCL�ļ�������GSE���ƿ������Ʒ����ͨ�Ų�����Ϣ��Map
	LNTYPE_MAP     *lnTypeMap;             ///< SCL�ļ��������߼��ڵ�������Ϣ��Map
	DOTYPE_MAP     *doTypeMap;             ///< SCL�ļ�����������������Ϣ��Map
	DATYPE_MAP     *daTypeMap;             ///< SCL�ļ���������������������Ϣ��Map
}SCL_MAP;

#endif