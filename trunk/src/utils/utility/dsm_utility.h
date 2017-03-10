/**
 * \file
 *      dsm_utility.h
 *
 * \brief
 *      dsm��������ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/3/13
 */

#ifndef DSM_UTILITY_H__
#define DSM_UTILITY_H__

#include "src/service/datalayer/dsmdataLayer.h"


/**
 *      ��ȡģ��·���ַ���
 *
 * \return
 *      CString     ģ���·���ַ���
 * \note
 *      ����x:\aaaaa\bbbb\ccc.exe�򷵻�x:\aaaaa\bbbb\
 */
extern const CString DSM_GetAppDir();

/**
 *      ��ȡ����ʱ����Ŀ¼
 *
 *  \return
 *      CString     ��������ʱ�����ļ�Ŀ¼
 */
extern const CString DSM_GetRuntimeDir();

/**
 *      ����������ת���Ӵ�������
 */
extern void DSM_ParentToChild(CWnd* pChild, CRect& rcRect);

/**
 *      ��ȡ�ļ���չ��
 *
 *  \note
 *      "x:\\sample\\test.xml"  -->     "xml"
 */
extern LPCTSTR DSM_GetFileExtension(LPCTSTR pFile);

/**
 *      �޸Ĳ˵�����
 *
 *  \param[in]
 *      hMenu           �˵����
 *  \param[in]
 *      uIDItem         item��ʶ����λ�ã�ȡ����fByPosition
 *  \param[in]
 *      fByPosition     ָ��uIDItem��λ����Ϣ���Ǳ�ʶ
 *                      TRUE:   uIDItem��ֵ��λ������
 *                      FALSE:  uIDItem��ֵ�������ʶ
 *  \param[in]
 *      csVal           ���ַ���
 */
extern void DSM_SetMenuString(HMENU hMenu, 
                              UINT uIDItem, 
                              BOOL fByPosition,
                              CString csVal);

/**
 *      �ַ���ת����
 */
extern bool DSM_Str2Float(CString csStr, float& rfVal, int nInteger = 5, int nDecimal = 3);

/**
 *      �ַ���ת����
 */
extern bool DSM_Str2Double(CString csStr, double& rdVal, int nInteger = 5, int nDecimal = 3);

/**
 *      ���ݶ˿����ͱ�ʶ��ȡ�˿ڵľ�������
 *
 *  \param[in]
 *      ePort           �˿�����ö��ֵ
 */
extern CString DSM_GetPortDesc(DsmPortName ePort);

/**
 *      ���ݿ��ƿ��ȡ���ƿ������������
 *
 *  \param[in]
 *      pCtrlBlk        ���ƿ���Ϣ
 *  \return
 *      CString         ���ؿ��ƿ�����������Ϣ
 *
 *  \note
 *      ���ص���Ϣ��appid[ldname] + port����ʽ
 *
 */
extern CString DSM_GetCtrlBlkInfo(CBaseDetectItem* pCtrlBlk);

/**
 *      ��ȡ92��goose���Ƶ�����
 *
 *  \param[in]
 *      pCtrlBlk        ���ƿ���Ϣ
 *  \return
 *      CString         ����������ƿ��������Ϣ
 *
 *  \note
 *      ���ص���ϢҪô��scl�е���������scl�е�����Ϊ�գ��򷵻�MAC��Ϣ
 */
extern const CString DSM_GetNetCtrlBlkDesc(CBaseDetectItem* pCtrlBlk);

/**
 *      ת����ǣ���һ�����Ϊ��׼��ת���������ֵΪ��ָ���ķ�Χ��
 *
 *  \param[in]
 *      fAbsPhaseMark   ��׼���
 *  \param[in]
 *      fAbsPhase       ��ת�������
 *  \param[in]
 *      fMin            ��Ƿ�Χ��Сֵ
 *  \param[in]
 *      fMax            ��Ƿ�Χ���ֵ
 *  \return
 *      float            ת��������
 */
//extern float DSM_GetRelativePhase(float fAbsPhaseMark, float fAbsPhase, float fMin = -180.0f, float fMax = 180.0f);
extern float DSM_GetRelativePhase( float fAbsPhaseMark, float fAbsPhase, int nDecimalMed = 6, float fMin = -180.0f, float fMax = 180.0f );

/**
 *      �ж�·���Ƿ���SD����
 *
 *  \param[in]
 *      csPath          ���жϵ�·��
 *  \return
 *      bool            ��SD�·���true������SD·���·���false
 */
extern bool DSM_PathInSD(CString csPath);


/**
 *      �ж�ָ��·���Ƿ����
 *
 * \param[in]
 *      csPath          ̽��ָ����·���Ƿ����
 *  \return
 *      bool            Ŀ¼���ڷ���true�����򷵻�false
 */
extern bool DSM_SDPathIsExist(CString csPath);

/**
 *      ����Ŀ¼
 *
 *  \param[in]
 *      csPath          ������Ŀ¼·��
 *  \return
 *      bool            ����Ŀ¼�ɹ�����true��ʧ�ܷ���false
 */
extern bool DSM_CreatePath(CString csPath);


/**
 *      ����SD����Ŀ¼
 *
 *  \param[in]
 *      csPath          ������Ŀ¼·��
 *  \return
 *      bool            ����Ŀ¼�ɹ�����true��ʧ�ܷ���false
 */
extern bool DSM_CreateSDPath(CString csPath);

/**
 *      ��ȡһ�����õ�SD��·��
 *
 *  \param[out]
 *      rcsSDPath       SD����������Ŀ¼
 *  \return
 *      bool            ��ȡSD��Ŀ¼�ɹ�����true��ʧ�ܷ���false
 */
extern bool DSM_GetSDDriver(CString& rcsSDPath);

/**
 *      �ж�ָ���ļ��Ƿ����
 *
 *  \param[in]
 *      rcsFile         �ļ�·��
 *  \return
 *      bool            �ļ����ڷ���true�������ڷ���false
 */
extern bool DSM_IsFileExist(const CString& rcsFile);

/**
 *      ���ö����Ӵ��ڽ���
 */
extern void DSM_SetZFocus(HWND hWnd);



#endif // DSM_UTILITY_H__
