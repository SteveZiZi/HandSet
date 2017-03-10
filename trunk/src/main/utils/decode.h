/**
 * \file
 *      decode.h
 *
 * \brief
 *      �����������ݵ�ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/11/18
 */


#pragma once

class Sav92Msg;
class Sav91Msg;
class GooseMsg;
class NanRuiFT3Msg;
class GuoWangFT3Msg; 

/**
 *      ����smv92Э��
 */
extern void DSM_Decode92( CTreeV& rTree, Sav92Msg* pMsg92, enumFrameType type, const uint8_t* pBuff, int bufflen );

/**
 *      ����smv91Э��
 */
extern void DSM_Decode91( CTreeV& rTree, Sav91Msg* pMsg91, enumFrameType type, const uint8_t* pBuff, int bufflen );

/**
 *      ����ft3����
 */
extern void DSM_DecodeNanruiFT3( CTreeV& rTree, NanRuiFT3Msg* pMsgNanRui, enumFrameType type, const uint8_t* pBuff, int bufflen );

/**
 *      ����ft3����
 */
extern void DSM_DecodeGuowangFT3( CTreeV& rTree, GuoWangFT3Msg* pMsgGuoWang, enumFrameType type, const uint8_t* pBuff, int bufflen );

/**
 *      ����gooseЭ������
 */
extern void DSM_DecodeGoose( CTreeV& rTree, GooseMsg* pGooseMsg, enumFrameType type, const uint8_t* pBuff, int bufflen );
