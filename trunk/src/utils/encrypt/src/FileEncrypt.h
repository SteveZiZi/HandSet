/// @file
///
/// @brief
///		可对文件进行加密解密的实现代码
///
/// @note
///		可对任意文件进行加密，但只能对
///		特定格式的加密文件进行解密
///
/// Copyright (c) 2013 IUnknownBase Inc.
///
/// 作者:
///
///     chao 2013.7.9
///
/// 修改历史: V1.0
///
#pragma once

///
/// @brief
///      加密解密结果
///
enum EncryptResult
{
	ER_OK = 0,		///< 加密/解密成功
	ER_ERR = -1		///< 加密/解密失败
};


class CFileEncrypt
{
public:
	CFileEncrypt(void);
	~CFileEncrypt(void);
	EncryptResult EncryptEncodeFile(LPCTSTR  srcfile, LPCTSTR  dstfile);
	EncryptResult EncryptDecodeFile(LPCTSTR  srcfile, LPCTSTR  dstfile);
private:
	void EnDecodeData( BYTE * pSrcData, int nSrcLen, BYTE * pDstData,BYTE * pCodeKey );
	void EncryptInit(BYTE * pEncodeKey);
	bool EncryptStrEql(const BYTE * pFirByte, const BYTE * pSecByte, int nLen);
};
