/// @file
///
/// @brief
///		�ɶ��ļ����м��ܽ��ܵ�ʵ�ִ���
///
/// @note
///		�ɶ������ļ����м��ܣ���ֻ�ܶ�
///		�ض���ʽ�ļ����ļ����н���
///
/// Copyright (c) 2013 IUnknownBase Inc.
///
/// ����:
///
///     chao 2013.7.9
///
/// �޸���ʷ: V1.0
///
#pragma once

///
/// @brief
///      ���ܽ��ܽ��
///
enum EncryptResult
{
	ER_OK = 0,		///< ����/���ܳɹ�
	ER_ERR = -1		///< ����/����ʧ��
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
