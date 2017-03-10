/// @file
///
/// @brief
///     �������ʹ�õĹ������Ͷ���
///     ���������򣬸�ģ��ʹ�õ�ͨ�����ͣ���ֵһ��/�������ͣ�����֡���͵�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.7.12
///
/// �޸���ʷ��
///

#ifndef _DSM_PUB_DATA_TYPES_H
#define _DSM_PUB_DATA_TYPES_H


///
/// @brief
///    ��������֡(����)����
///
enum enumFrameType{
    FRAME_TYPE_UNKOWN = 0,                                              ///< ����֡����δ֪
    FRAME_TYPE_NET_GOOSE,                                               ///< ����goose����֡
    FRAME_TYPE_NET_9_1,                                                 ///< ����9-1����֡��12��ͨ��
	FRAME_TYPE_NET_9_1_EX,                                              ///< ����9-1��չ����֡�����Ͼ�������綨�壬���12��ͨ����
    FRAME_TYPE_NET_9_2,                                                 ///< ����9-2����֡
    FRAME_TYPE_NET_1588,                                                ///< ����1588����
    FRAME_TYPE_FT3_NANRUI,                                              ///< ����ʹ�ñ�׼֡��56�ֽڰ汾����12��ͨ��
    FRAME_TYPE_FT3_GUOWANG,                                             ///< ����ʹ����չ֡��22��ͨ��
	FRAME_TYPE_FT3_STD52,                                               ///< FT3��׼֡��52�ֽڰ汾����12��ͨ��
	FRAME_TYPE_FT3_ONEPHASE_CTORVT,                                     ///< �������/��ѹ������FT3����֡��5��ͨ��
	FRAME_TYPE_FT3_TRIPHASE_CT,                                         ///< �������������FT3����֡��9��ͨ��
	FRAME_TYPE_FT3_TRIPHASE_VT_2010,                                    ///< �����ѹ������FT3����֡��2010�棩��6��ͨ��
	FRAME_TYPE_FT3_TRIPHASE_VT_2011,                                    ///< �����ѹ������FT3����֡��2011���Ժ�İ汾����6��ͨ��
	FRAME_TYPE_FT3_TRIPHASE_CTORVT                                      ///< �������/��ѹ������FT3����֡��15��ͨ��
};

///
/// @brief
///    ����ͨ��������Ϣ������smv��gooseͨ������
///
enum enumChannelType{
    CHANNEL_TYPE_UNKOWN  = 0,                                           ///< ͨ������δ֪
    //smvͨ������
    CHANNEL_TYPE_SMV_CURRENT,                                           ///< ͨ������ΪSMV����
    CHANNEL_TYPE_SMV_VOLTAGE,                                           ///< ͨ������ΪSMV��ѹ
    CHANNEL_TYPE_SMV_TIME,                                              ///< ͨ������ΪSMVʱ��(ָ9-2�е���ʱ)
    //gooseͨ������
    CHANNEL_TYPE_GOOSE_STRUCT,                                          ///< �ṹ
    CHANNEL_TYPE_GOOSE_POINT,                                           ///< ����
    CHANNEL_TYPE_GOOSE_DOUBLE_POINT,                                    ///< ˫��
    CHANNEL_TYPE_GOOSE_QUALITY,                                         ///< Ʒ��
    CHANNEL_TYPE_GOOSE_TIME,                                            ///< ͨ������Ϊgooseʱ����Ϣ
    CHANNEL_TYPE_GOOSE_FLOAT,                                           ///< ������
    CHANNEL_TYPE_GOOSE_INT8,                                            ///< �з���8λ����
    CHANNEL_TYPE_GOOSE_INT16,                                           ///< �з���16λ����
    CHANNEL_TYPE_GOOSE_INT24,                                           ///< �з���24λ����
    CHANNEL_TYPE_GOOSE_INT32,                                           ///< �з���32λ����
    CHANNEL_TYPE_GOOSE_UINT8,                                           ///< �޷���8λ����
    CHANNEL_TYPE_GOOSE_UINT16,                                          ///< �޷���16λ����
    CHANNEL_TYPE_GOOSE_UINT24,                                          ///< �޷���24λ����
    CHANNEL_TYPE_GOOSE_UINT32                                           ///< �޷���32λ����
};

///
/// @brief
///    ����ͨ���Ǳ����������ݻ��ǲ�������
///
enum enumChannelValueType
{
    CHANNEL_VALUE_TYPE_UNKNOWN = 0,                                     ///< ͨ����������Ϊδ֪
    CHANNEL_VALUE_TYPE_PROTECT,                                         ///< ͨ����������Ϊ����ֵ
    CHANNEL_VALUE_TYPE_MEASURE                                          ///< ͨ����������Ϊ����ֵ
};

///
/// @brief
///    ����ֵ����������һ��/����ֵ,
///
enum enumSmvValueType
{
    SMV_VALUE_TYPE_ORIGINANL = 0,                                       ///< ��������Ϊԭʼֵ
    SMV_VALUE_TYPE_PRIMARY,                                             ///< ��������Ϊһ��ֵ
    SMV_VALUE_TYPE_SECONDARY                                            ///< ��������Ϊ����ֵ
};

#endif //_DSM_PUB_DATA_TYPES_H