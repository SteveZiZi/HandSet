/**
 * \file
 *      dsm_utility.h
 *
 * \brief
 *      dsm帮助函数头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/13
 */

#ifndef DSM_UTILITY_H__
#define DSM_UTILITY_H__

#include "src/service/datalayer/dsmdataLayer.h"


/**
 *      获取模块路径字符串
 *
 * \return
 *      CString     模块的路径字符串
 * \note
 *      比如x:\aaaaa\bbbb\ccc.exe则返回x:\aaaaa\bbbb\
 */
extern const CString DSM_GetAppDir();

/**
 *      获取运行时配置目录
 *
 *  \return
 *      CString     返回运行时配置文件目录
 */
extern const CString DSM_GetRuntimeDir();

/**
 *      父窗口坐标转到子窗口坐标
 */
extern void DSM_ParentToChild(CWnd* pChild, CRect& rcRect);

/**
 *      获取文件扩展名
 *
 *  \note
 *      "x:\\sample\\test.xml"  -->     "xml"
 */
extern LPCTSTR DSM_GetFileExtension(LPCTSTR pFile);

/**
 *      修改菜单标题
 *
 *  \param[in]
 *      hMenu           菜单句柄
 *  \param[in]
 *      uIDItem         item标识或者位置，取决于fByPosition
 *  \param[in]
 *      fByPosition     指定uIDItem是位置信息还是标识
 *                      TRUE:   uIDItem的值是位置索引
 *                      FALSE:  uIDItem的值是命令标识
 *  \param[in]
 *      csVal           新字符串
 */
extern void DSM_SetMenuString(HMENU hMenu, 
                              UINT uIDItem, 
                              BOOL fByPosition,
                              CString csVal);

/**
 *      字符串转浮点
 */
extern bool DSM_Str2Float(CString csStr, float& rfVal, int nInteger = 5, int nDecimal = 3);

/**
 *      字符串转浮点
 */
extern bool DSM_Str2Double(CString csStr, double& rdVal, int nInteger = 5, int nDecimal = 3);

/**
 *      根据端口类型标识获取端口的具体描述
 *
 *  \param[in]
 *      ePort           端口类型枚举值
 */
extern CString DSM_GetPortDesc(DsmPortName ePort);

/**
 *      根据控制块获取控制块组合描述描述
 *
 *  \param[in]
 *      pCtrlBlk        控制块信息
 *  \return
 *      CString         返回控制块的组合描述信息
 *
 *  \note
 *      返回的信息以appid[ldname] + port的形式
 *
 */
extern CString DSM_GetCtrlBlkInfo(CBaseDetectItem* pCtrlBlk);

/**
 *      获取92或goose控制的描述
 *
 *  \param[in]
 *      pCtrlBlk        控制块信息
 *  \return
 *      CString         返回网络控制块的描述信息
 *
 *  \note
 *      返回的信息要么是scl中的描述，若scl中的描述为空，则返回MAC信息
 */
extern const CString DSM_GetNetCtrlBlkDesc(CBaseDetectItem* pCtrlBlk);

/**
 *      转换相角，以一个相角为基准，转换其他相角值为至指定的范围内
 *
 *  \param[in]
 *      fAbsPhaseMark   基准相角
 *  \param[in]
 *      fAbsPhase       待转换的相角
 *  \param[in]
 *      fMin            相角范围最小值
 *  \param[in]
 *      fMax            相角范围最大值
 *  \return
 *      float            转换后的相角
 */
//extern float DSM_GetRelativePhase(float fAbsPhaseMark, float fAbsPhase, float fMin = -180.0f, float fMax = 180.0f);
extern float DSM_GetRelativePhase( float fAbsPhaseMark, float fAbsPhase, int nDecimalMed = 6, float fMin = -180.0f, float fMax = 180.0f );

/**
 *      判断路径是否在SD卡下
 *
 *  \param[in]
 *      csPath          待判断的路径
 *  \return
 *      bool            在SD下返回true，不在SD路径下返回false
 */
extern bool DSM_PathInSD(CString csPath);


/**
 *      判断指定路径是否存在
 *
 * \param[in]
 *      csPath          探测指定的路径是否存在
 *  \return
 *      bool            目录存在返回true，否则返回false
 */
extern bool DSM_SDPathIsExist(CString csPath);

/**
 *      创建目录
 *
 *  \param[in]
 *      csPath          待创建目录路径
 *  \return
 *      bool            创建目录成功返回true，失败返回false
 */
extern bool DSM_CreatePath(CString csPath);


/**
 *      创建SD卡下目录
 *
 *  \param[in]
 *      csPath          待创建目录路径
 *  \return
 *      bool            创建目录成功返回true，失败返回false
 */
extern bool DSM_CreateSDPath(CString csPath);

/**
 *      获取一个可用的SD卡路径
 *
 *  \param[out]
 *      rcsSDPath       SD卡的驱动器目录
 *  \return
 *      bool            获取SD卡目录成功返回true，失败返回false
 */
extern bool DSM_GetSDDriver(CString& rcsSDPath);

/**
 *      判断指定文件是否存在
 *
 *  \param[in]
 *      rcsFile         文件路径
 *  \return
 *      bool            文件存在返回true，不存在返回false
 */
extern bool DSM_IsFileExist(const CString& rcsFile);

/**
 *      设置顶层子窗口焦点
 */
extern void DSM_SetZFocus(HWND hWnd);



#endif // DSM_UTILITY_H__
