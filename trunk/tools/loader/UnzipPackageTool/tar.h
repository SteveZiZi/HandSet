/**********************************************Copyright (c)**********************************************
**                               GuangZhou XuanTong Electric Technology Co.,LTD.
**                                    
**
**                                     http://www.xtoee.com.cn
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:              untgz.h
** Latest modified Date:   2012-12-20    17:50:29
** Latest Version:         
** Description:            
**                         
**--------------------------------------------------------------------------------------------------------
** Create by:              WeiXianping
** Create date:            2012-12-20    17:50:29
** Version:                
** Description:            
**                         
**                         
**--------------------------------------------------------------------------------------------------------
** Modified by:            
** Modified date:          
** Version:                
** Description:            
**                         
*********************************************************************************************************/
#ifndef __UNTGZ_H__
#define __UNTGZ_H__



#ifdef __cplusplus
extern "C" {
#endif

#ifdef UNTGZ_DLL
#define UNTGZEXTERN __declspec(dllexport)
#else
#define UNTGZEXTERN
#endif

    /*! \fn         UNTGZEXTERN int extract( const TCHAR* desPath,
    /*              const TCHAR* srcfile )
    *   \brief      解压tar格式包，支持tar、.tgz和.tar.gz文件格式到指定目录
    *   \param      desPath 解压后文件存放目录（绝对路径），该目录必须存在
    /*                      传NULL时会解压到系统根目录(慎重)
    *   \param      srcfile 需要解压的文件（绝对路径）
    *   \return     0表示解压成功，非0表示解压失败
    */
    extern UNTGZEXTERN int extract( const TCHAR* desPath, const TCHAR* srcfile );

    /*! \fn         UNTGZEXTERN int tar( const TCHAR* srcfile,
    /*              const TCHAR* desfile )
    *   \brief      打包指定文件或目录到tar格式文件，如果是目录则打包该
    *               目录下所有文件和子目录
    *   \param      srcPath 需要打包文件的文件或父目录（绝对路径）
    *   \param      desfile 打包后的文件（绝对路径）
    *   \return     0表示打包成功，非0表示打包失败
    */
    extern UNTGZEXTERN int tar( const TCHAR* srcfile, const TCHAR* desfile );

    
    /*! \fn         UNTGZEXTERN int untar( const TCHAR* srcfile,
    /*              const TCHAR* desfile )
    *   \brief      解包tar文件到目录
    *   \param      desPath 解包后文件存放目录（绝对路径），该目录必须存在
    *   \param      srcfile 需要解包的文件（绝对路径）
    *   \return     0表示解包成功，非0表示解包失败
    */
    extern UNTGZEXTERN int untar( const TCHAR* desPath, const TCHAR* srcfile );

    /*! \fn         UNTGZEXTERN int tgz( const TCHAR* srcfile,
    /*              const TCHAR* desfile )
    *   \brief      压缩指定文件或目录到tar.gz格式文件，如果是目录则压缩该
    *               目录下所有文件和子目录
    *   \param      srcPath 需要压缩文件的文件或父目录（绝对路径）
    *   \param      desfile 压缩后的文件（绝对路径）
    *   \return     0表示压缩成功，非0表示压缩失败
    */
    extern UNTGZEXTERN int tgz( const TCHAR* srcfile, const TCHAR* desfile );
    
    /*! \fn         UNTGZEXTERN int untgz( const TCHAR* desPath,
    /*              const TCHAR* srcfile )
    *   \brief      解压tar格式包，支持tar、.tgz和.tar.gz文件格式到指定目录
    *   \param      desPath 解压后文件存放目录（绝对路径），
    /*                      传NULL时解压到系统根目录(慎重)
    *   \param      srcfile 需要解压的文件（绝对路径）
    *   \return     0表示解压成功，非0表示解压失败
    */
    extern UNTGZEXTERN int untgz( const TCHAR* desPath, const TCHAR* srcfile );
    
    /*! \fn         UNTGZEXTERN int zip( const TCHAR* srcfile,
    /*              const TCHAR* desfile )
    *   \brief      压缩指定目录下所有文件和目录到zip格式文件
    *   \param      srcPath 需要压缩的目录（绝对路径）
    *   \param      desfile 压缩后的文件（绝对路径）
    *   \return     0表示压缩成功，非0表示压缩失败
    */
    extern UNTGZEXTERN int zip( const TCHAR* srcfile, const TCHAR* desfile );
    
    /*! \fn         UNTGZEXTERN int unzip( const TCHAR* desPath,
    /*              const TCHAR* srcfile )
    *   \brief      解压zip格式文件到指定目录
    *   \param      desPath 解压后文件存放目录（绝对路径），该目录必须存在，
    /*                      传NULL时会解压到系统根目录(慎重)
    *   \param      srcfile 需要解压的文件（绝对路径）
    *   \return     0表示解压成功，非0表示解压失败
    */
   extern UNTGZEXTERN int unzip( const TCHAR* desPath, const TCHAR* srcfile );

#ifdef __cplusplus
}
#endif

#endif // __UNTGZ_H__