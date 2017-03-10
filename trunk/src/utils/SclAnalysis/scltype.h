/// @file
///     scltypes.h
///
/// @brief
///     ����SCL�ļ��������Զ�������
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.1.22
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.02.01     |1.0.1.201  |����SCL�ļ���DataTypeTemplates���ֵĽ������ݵĽṹ�嶨��  |
///    |2013.02.06     |1.0.2.206  |����SCL�ļ����߼��ڵ����󶨵��ⲿ�źţ�Inputs���Ľṹ�嶨��  |
///    |2013.06.26     |1.0.3.626  |����SCL�ļ����������ԵĽṹ�嶨��                         |
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

#define SCL_CSTR_FILENAME MCD_CSTR_FILENAME   ///< �Զ����ļ�·�����ͣ�MCD_XX ΪMarkUp�ж��������
#define SCL_CSTR          MCD_CSTR            ///< �Զ���Ԫ�ر�ǩ����
#define SCL_STR           std::wstring        ///< �Զ����ַ�������
#define SCL_SPTINTF       swprintf_s
#define SCL_SSCANF        swscanf_s

///
/// @brief
///     IED�����ܵ����豸��������
///
typedef struct  
{
	SCL_STR iedName;          ///< IEDʵ��������
	SCL_STR iedDesc;          ///< ��IEDʵ��������
	SCL_STR manufactactuer;   ///< ������������������
	SCL_STR configVersion;    ///< IED���õĻ������ð汾��Ϣ������ʶIED�Ļ������ã��������̹涨/�ṩ��������
}SCL_IED_ATTRIB;

///
/// @brief
///     AccessPoint�����ʵ㣩������
///
typedef struct
{
	SCL_STR apName;           ///< IED�ڵķ��ʵ�ʵ����Ӧ��IED�ڱ���Ψһ��
	SCL_STR apDesc;           ///< �Ը÷��ʵ����������
	bool    clock;            ///< ��ʶ��IED�Ƿ�Ϊ���������ϵ���ʱ�ӡ�ȱʡΪ��false��������ʱ�ӡ�
	bool    router;           ///< ��ʾ��IED�Ƿ���·�������ܡ�ȱʡΪ��false������·�������ܡ�
}SCL_ACCESS_POINT_ATTRIB;

///
/// @brief
///     Server����������������
///
typedef struct
{
	SCL_STR serverDesc;       ///< ����������������
	SCL_STR timeout;          ///< ��ʱʱ�䣬��λΪs������������Ĵ����ڸ�ʱ����δ��ɣ���ѡ��һ��ֵ�飩����ȡ���ô�������
}SCL_SERVER_ATTRIB;

///
/// @brief
///     LDevice���߼��豸��������
///
typedef struct
{
	SCL_STR ldInst;           ///< LDevice��ʵ����
	SCL_STR ldDesc;           ///< LDevice����������
}SCL_LDEVICE_ATTRIB;

///
/// @brief
///     LN0���߼��ڵ�LLN0��������
///
typedef struct
{
	SCL_STR lnClass;          ///< IEC61850-7������߼��ڵ��ࡣ����̶�ΪLLN0
	SCL_STR lnInst;           ///< ���߼��ڵ����ʵ���š���LLN0���̶�Ϊ���ַ�����
	SCL_STR lnType;           ///< ���߼��ڵ�ʵ���������Ͷ��壬����LNodeTypeԪ�صĶ���
	SCL_STR lnDesc;           ///< ��������
}SCL_LN0_ATTRIB;

///
/// @brief
///     LN��LLN0��������߼��ڵ㣩������
///
typedef struct
{
	SCL_STR lnPrefix;         ///< �߼��ڵ�ǰ׺
	SCL_STR lnClass;          ///< IEC61850-7������߼��ڵ���
	SCL_STR lnInst;           ///< ���߼��ڵ����ʵ����
	SCL_STR lnType;           ///< ���߼��ڵ�ʵ���������Ͷ��壬����LNodeTypeԪ�صĶ���
	SCL_STR lnDesc;           ///< ��������
}SCL_LN_ATTRIB;

///
/// @brief
///     DataSet�����ݼ���һ�����߼��ڵ��ж��壩������
///
typedef struct
{
	SCL_STR dataSetName;      ///< �����ݼ�������
	SCL_STR	dataSetDesc;      ///< �����ݼ�����������
	int     fcdaNum;          ///< �����ݼ�������FCDA����Ŀ
}SCL_DATASET_ATTRIB;

///
/// @brief
///     FCDA�����ݼ��ĳ�Ա���ڲ����õ�ַ��������
///
typedef struct
{
	int     idxSeq;           ///< ��FCDA��DataSet�е�˳���
	SCL_STR ldInst;           ///< ��FCDA��ָ��������������߼��豸��ʵ����
	SCL_STR lnPrefix;         ///< ��FCDA��ָ��������������߼��ڵ��ǰ׺
	SCL_STR lnClass;          ///< ��FCDA��ָ��������������߼��ڵ���߼��ڵ���
	SCL_STR lnInst;           ///< ��FCDA��ָ��������������߼��ڵ��ʵ����
	SCL_STR doName;           ///< ��FCDA��ָ������ݣ�DOI��������
	SCL_STR daName;           ///< ��FCDA��ָ����������ԣ�DAI��������
	SCL_STR fc;               ///< ��FCDA��ָ������ݵĹ���Լ��
}SCL_FCDA_ATTRIB;

///
/// @brief
///     DOI������ʵ����������
///
typedef struct 
{
	SCL_STR doName;           ///< �����ݵ�����
	SCL_STR doDesc;           ///< �����ݵ���������
//	SCL_STR accessControl;    ///< �����ݵķ��ʿ��ƶ���
//	SCL_STR ix;               ///< ��������������Ԫ�ص��±�
	SCL_STR dUVal;            ///< ����������������dU��ֵ
	SCL_STR dUSAddr;          ///< ����������������dU��sAddr����ֵ
}SCL_DOI_ATTRIB;

///
/// @brief
///     DAI���������ԣ�������
///
typedef struct
{
	SCL_STR daName;           ///< ���������Ե�����
	SCL_STR daDesc;           ///< ���������Ե���������
	SCL_STR sAddr;            ///< ���������ԵĶ̵�ַ�����û�����
	SCL_STR daBType;          ///< ��������������IEC61850-7���ֹ涨�Ļ�����������
//	SCL_STR valKind;          ///< ����������ƣ����ʾ���̽׶θ�ֵ�ĺ���
//	SCL_STR ix;               ///< ����������DAIԪ�ص��±�
//	SCL_STR daVal;            ///< Ĭ��ֵ
}SCL_DAI_ATTRIB;

///
/// @brief
///     �߼��ڵ����󶨵��ⲿ�ź�(Inputs)��ĳһ���ߣ�ExtRef���Ľṹ�嶨��
///
typedef struct
{
	int     idxSeq;           ///< �ⲿ�ź���Inputs�е�˳��ţ���0��ʼ
	SCL_STR desc;             ///< �Ը����ߣ��󶨣�����������
	SCL_STR iedName;          ///< �ⲿ�ź����ڵ�IED������
	SCL_STR ldInst;           ///< �ⲿ�ź������߼��豸��ʵ����
	SCL_STR lnPrefix;         ///< �ⲿ�ź������߼��ڵ��ǰ׺
	SCL_STR lnClass;          ///< �ⲿ�ź������߼��ڵ��������߼��ڵ���
	SCL_STR lnInst;           ///< �ⲿ�ź������߼��ڵ��ʵ����
	SCL_STR doName;           ///< �ⲿ�ź�����ʶ��DO������
	SCL_STR daName;           ///< �ⲿ�ź�����ʶ��DA�����ơ���ֵΪ��ʱ����ʾ�ⲿ�ź�ΪDO�������������ԡ�
	SCL_STR intAddr;          ///< �ⲿ�ź����󶨵��ڲ���ַ
}SCL_EXTREF_ATTRIB;

///
/// @brief
///     SampledValueControl������ֵ���ƿ飩������
///
typedef struct 
{
	SCL_STR cbName;           ///< ���ƿ�����
	SCL_STR cbDesc;           ///< ���ƿ����������
	SCL_STR confRev;          ///< ���ƿ�����ð汾��
	SCL_STR smvID;            ///< ���ƿ��ID��һ��Ϊ��LDName/LNName.SMVCBName��
	SCL_STR dataSet;          ///< Ӧ��SV���ķ��͵����ݼ�������
	SCL_STR smpRate;          ///< ����IEC61850-7-2����Ĳ�������
	SCL_STR nofASDU;          ///< ����SV������ASDU������
	bool    multicast;        ///< ��ʾ���õ��ǶಥSMV�����ǵ���SMV����
	bool    smvOptReftTm;     ///< SMV�������Ƿ����ˢ��ʱ��
	bool    smvOptSmpRate;    ///< SMV�������Ƿ����������
	bool    smvOptSmpSync;    ///< SMV�������Ƿ����ͬ������
	bool    smvOptSecurity;   ///< SMV�������Ƿ������ȫ��
	bool    smvOptDataRef;    ///< SMV�������Ƿ����Ӧ��SV���ķ��͵����ݼ������ã����ƣ�
}SCL_SMVCTRL_ATTRIB;

///
/// @brief
///     ������SubNetwork��������
///
typedef struct
{
	SCL_STR name;             ///< ����������
 	SCL_STR type;             ///< ����������
// 	SCL_STR desc;             ///< ����
}SCL_SUBNETWORK_ATTRIB;

///
/// @brief
///     ������������SubNetwork���ϵķ��ʵ������
///
typedef struct
{
	SCL_STR iedName;          ///< �����������ϵķ��ʵ�������IED������
	SCL_STR apName;           ///< �����������ϵķ��ʵ������
}SCL_CONNECTEDAP_ATTRIB;

///
/// @brief
///     GSEControl��GSE���ƿ飩������
///
typedef struct
{
	SCL_STR cbName;           ///< ���ƿ������
	SCL_STR cbDesc;           ///< ���ƿ����������
	SCL_STR confRev;          ///< ���ƿ�����ư汾��
	SCL_STR gooseID;          ///< ���ƿ�ID
	SCL_STR dataSet;          ///< Ӧ��GOOSE/GSSE���ķ��͵����ݼ�������
	SCL_STR type;             ///< ���ƿ����ͣ��С�GOOSE���͡�GSSE������
}SCL_GSECTRL_ATTRIB;

///
/// @brief
///     SVM�����ַ�Լ����Ƹô������Ŀ��ƿ���Ϣ
///
typedef struct
{
	SCL_STR macAddr;          ///< ���ʷ��ʵ�ֵַ��Ϊ6�鱻-�ֿ���2���ַ����ַ�Ϊ0~9��A~F
	SCL_STR appID;            ///< Ӧ�ñ�ʶ��Ϊ4����0~9��A~F��ɵ��ַ�
	SCL_STR vlanID;           ///< VLAN ID��Ϊ3��0~9��A~F��ɵ��ַ�
	SCL_STR vlanPriority;     ///< VLAN �û����ȼ���Ϊ0~7�ĵ��ַ�
	SCL_STR ldInst;           ///< ���Ƹô������Ŀ��ƿ��������߼��豸��ʵ����
	SCL_STR cbName;           ///< ���Ƹô������Ŀ��ƿ������
}SCL_SMV_ATTRIB;

///
/// @brief
///     GOOSE/GSSE�����ַ�Լ����Ƹô������Ŀ��ƿ���Ϣ
///
typedef struct
{
	SCL_STR macAddr;          ///< ���ʷ��ʵ�ֵַ��Ϊ6�鱻-�ֿ���2���ַ����ַ�Ϊ0~9��A~F
	SCL_STR appID;            ///< Ӧ�ñ�ʶ��Ϊ4����0~9��A~F��ɵ��ַ�
	SCL_STR vlanID;           ///< VLAN ID��Ϊ3��0~9��A~F��ɵ��ַ�
	SCL_STR vlanPriority;     ///< VLAN �û����ȼ���Ϊ0~7�ĵ��ַ�
	SCL_STR minTimeVal;       ///< GOOSE���ݱ�λ������������ʱ����
	SCL_STR minTimeUnit;      ///< minTimeʱ�����ĵ�λ
	SCL_STR maxTimeVal;       ///< ����ʱ��T0
	SCL_STR maxTimeUnit;      ///< ����ʱ��ĵ�λ
	SCL_STR ldInst;           ///< ���Ƹô������Ŀ��ƿ��������߼��豸��ʵ����
	SCL_STR cbName;           ///< ���Ƹô������Ŀ��ƿ������
}SCL_GSE_ATTRIB;

///
/// @brief
///     �ͻ�/�����������ַ
///
typedef struct
{
	SCL_STR ip;                ///< IP��ַ
	SCL_STR ipSubnet;          ///< TCP/IP������������
	SCL_STR	ipGateway;         ///< TCP/IP���ĵ�һ�����ص�ַ
	SCL_STR osiNsap;           ///< OSI�����ַ
	SCL_STR	osiTsel;           ///< OSI�����ѡ���
	SCL_STR	osiSsel;           ///< OSI�Ự��ѡ���
	SCL_STR	osiPsel;           ///< OSI��ʾ��ѡ���
	SCL_STR osiApTitle;        ///< OSI ACSEӦ������ֵ
	SCL_STR osiApInvoke;       ///< OSI ACSEӦ�õ���ID
	SCL_STR	osiAeQualifier;    ///< OSI ACSE AE�޶���
	SCL_STR	osiAeInvoke;       ///< OSI ACSE AE����ID
}SCL_ADDR_ATTRIB;

///
/// @brief
///     �߼��ڵ����͵�������Ϣ
///
typedef struct
{
	SCL_STR id;                ///< ���߼��ڵ����͵�ID
	SCL_STR desc;              ///< ������Ϣ
//	SCL_STR iedType;           ///< ���߼��ڵ���������IED��������IED�豸����,������
	SCL_STR lnClass;           ///< ���߼��ڵ�����Э��涨�Ļ�����
}SCL_LNTYPE_ATTRIB;

///
/// @brief
///     �߼��ڵ�������ĳһDO��������Ϣ
///
typedef struct
{
	SCL_STR name;              ///< �߼��ڵ�������ĳDATA����
	SCL_STR type;              ///< ������DOType��ID
	SCL_STR desc;              ///< ������Ϣ
//	SCL_STR accessControl;     ///< ��DO�ķ��ʿ��ƣ�������
//	SCL_STR transient;         ///< Ϊtrueʱ��ʾʹ��˲�䶨�壬������
}SCL_DOINLNTYPE_ATTRIB;

///
/// @brief
///     DOType��������Ϣ
///
typedef struct
{
	SCL_STR id;                ///< DOType��ID
//	SCL_STR iedType;           ///< ��DOType������IED�����ͣ�������
	SCL_STR cdc;               ///< ��DOType����Э��涨�Ļ�������������
}SCL_DOTYPE_ATTRIB;

///
/// @brief
///     DOType��ĳһDA��������Ϣ
///
typedef struct
{
	SCL_STR name;              ///< �����Ե�����
	SCL_STR desc;              ///< ����˵��
	SCL_STR fc;                ///< ����Լ��
	SCL_STR bType;             ///< �����ԵĻ���������
	SCL_STR type;              ///< �������������Զ������ͣ�bType=Enum��Structʱ
}SCL_DAINDOTYPE_ATTRIB;

///
/// @brief
///     DAType��������Ϣ
///
typedef struct
{
	SCL_STR id;                ///< DAType��ID
//	SCL_STR iedType;           ///< ��DOType������IED�����ͣ�������
	SCL_STR desc;              ///< ��������
}SCL_DATYPE_ATTRIB;

///
/// @brief
///     DAType��ĳһBDA��������Ϣ
///
typedef struct
{
	SCL_STR name;              ///< ����
//	SCL_STR desc;              ///< ����˵��
//	SCL_STR sAddr;             ///< ��ѡ�̵�ַ
	SCL_STR bType;             ///< �����ԵĻ���������
	SCL_STR type;              ///< �������������Զ������ͣ�bType=Enum��Structʱ
//	SCL_STR count;             ///< ������Ϊ����ʱ����ʾ����Ԫ�ص���Ŀ
//	SCL_STR valKind;           ///< ������ν��ͱ�������ֵ
}SCL_BDAINDATYPE_ATTRIB;

///
/// @brief
///     EnumType��ĳһEnumVal��������Ϣ
///
typedef struct
{
	SCL_STR id;                ///< EnumVal������EnumType��ID
	SCL_STR desc;              ///< EnumVal������EnumType������
	SCL_STR ord;               ///< EnumVal�ı�ǩֵ
	SCL_STR enumVal;           ///< EnumVal��ʵ�ʺ���
}SCL_ENUMVAL_ATTRIB;

#endif
