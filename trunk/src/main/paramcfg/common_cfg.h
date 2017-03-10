/**
 * \file
 *      common_cfg.h
 *
 * \brief
 *      smv公共资源定义文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/2
 */



#ifndef COMMON_CFG_H__
#define COMMON_CFG_H__

//////////////////////////////////////////////////////////////////////////
// 添加GOOSE（GOOSE发送 GOOSE接收）报表
// 
#if SU_FEI
#define DSM_BASIC_GOOSE_ADD_WIDTH_CHECKBOX      50
#define DSM_BASIC_GOOSE_ADD_WIDTH_APPID         100
#define DSM_BASIC_GOOSE_ADD_WIDTH_CHAN_COUNT    100
#define DSM_BASIC_GOOSE_ADD_WIDTH_CTRL_DESC     250
#define DSM_BASIC_GOOSE_ADD_WIDTH_PORT          100
#else

#define DSM_BASIC_GOOSE_ADD_WIDTH_CHECKBOX      48
#define DSM_BASIC_GOOSE_ADD_WIDTH_APPID         100
#define DSM_BASIC_GOOSE_ADD_WIDTH_CHAN_COUNT    83
#define DSM_BASIC_GOOSE_ADD_WIDTH_CTRL_DESC     207
#define DSM_BASIC_GOOSE_ADD_WIDTH_PORT          91

#endif

//////////////////////////////////////////////////////////////////////////
// 编辑GOOSE控制块报表
//
#if SU_FEI
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CAPTION      180
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CONTENT      420
#else
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CAPTION      180
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CONTENT      349
#endif

//////////////////////////////////////////////////////////////////////////

// 从全站配置中选择SMV
// 
#if SU_FEI
#define DSM_SEL_SMV_FROM_SCL_WIDTH_CHECKBOX      50
#define DSM_SEL_SMV_FROM_SCL_WIDTH_APPID         100
#define DSM_SEL_SMV_FROM_SCL_WIDTH_CHAN_COUNT    100
#define DSM_SEL_SMV_FROM_SCL_WIDTH_CTRL_DESC     250
#define DSM_SEL_SMV_FROM_SCL_WIDTH_PORT          100
#else

#define DSM_SEL_SMV_FROM_SCL_WIDTH_CHECKBOX      48
#define DSM_SEL_SMV_FROM_SCL_WIDTH_APPID         100
#define DSM_SEL_SMV_FROM_SCL_WIDTH_CHAN_COUNT    84
#define DSM_SEL_SMV_FROM_SCL_WIDTH_CTRL_DESC     207
#define DSM_SEL_SMV_FROM_SCL_WIDTH_PORT          91

#endif

// 从扫描列表中选择SMV
// 
#if SU_FEI
#define DSM_SEL_SMV_FROM_SCAN_WIDTH_CHECKBOX      50
#define DSM_SEL_SMV_FROM_SCAN_WIDTH_APPID         100
#define DSM_SEL_SMV_FROM_SCAN_WIDTH_CHAN_COUNT    100
#define DSM_SEL_SMV_FROM_SCAN_WIDTH_CTRL_DESC     250
#define DSM_SEL_SMV_FROM_SCAN_WIDTH_PORT          100

#else

#define DSM_SEL_SMV_FROM_SCAN_WIDTH_CHECKBOX      48
#define DSM_SEL_SMV_FROM_SCAN_WIDTH_APPID         100
#define DSM_SEL_SMV_FROM_SCAN_WIDTH_CHAN_COUNT    83
#define DSM_SEL_SMV_FROM_SCAN_WIDTH_CTRL_DESC     207
#define DSM_SEL_SMV_FROM_SCAN_WIDTH_PORT          91

#endif

//////////////////////////////////////////////////////////////////////////
// 手动试验- 试验结果 开入量动作
//
#define DSM_MANUAL_DI_ACTION_WIDTH_DI            79
#define DSM_MANUAL_DI_ACTION_WIDTH_ACTION_COUNT  100
#define DSM_MANUAL_DI_ACTION_WIDTH_ACTION1       140
#define DSM_MANUAL_DI_ACTION_WIDTH_ACTION2       140
#define DSM_MANUAL_DI_ACTION_WIDTH_ACTION3       140

#endif //COMMON_CFG_H__
