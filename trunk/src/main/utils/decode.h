/**
 * \file
 *      decode.h
 *
 * \brief
 *      解析报文数据的头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
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
 *      解析smv92协议
 */
extern void DSM_Decode92( CTreeV& rTree, Sav92Msg* pMsg92, enumFrameType type, const uint8_t* pBuff, int bufflen );

/**
 *      解析smv91协议
 */
extern void DSM_Decode91( CTreeV& rTree, Sav91Msg* pMsg91, enumFrameType type, const uint8_t* pBuff, int bufflen );

/**
 *      解析ft3南瑞
 */
extern void DSM_DecodeNanruiFT3( CTreeV& rTree, NanRuiFT3Msg* pMsgNanRui, enumFrameType type, const uint8_t* pBuff, int bufflen );

/**
 *      解析ft3国网
 */
extern void DSM_DecodeGuowangFT3( CTreeV& rTree, GuoWangFT3Msg* pMsgGuoWang, enumFrameType type, const uint8_t* pBuff, int bufflen );

/**
 *      解码goose协议数据
 */
extern void DSM_DecodeGoose( CTreeV& rTree, GooseMsg* pGooseMsg, enumFrameType type, const uint8_t* pBuff, int bufflen );
