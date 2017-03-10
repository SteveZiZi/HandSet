#include "StdAfx.h"
#include "FileEncrypt.h"
#include<afx.h>
#define KEY_LEN  256	///< ��Կ�ĳ���	
#define RANDOM_ROUND 256	///< ������Կ�������
#define DATA_SIZE 1024  ///< һ���Զ�д�ļ��Ĵ�С
#define  MAG_LEN  7		///< �����ļ��ı�ʶ�ų���
#define  VER_LEN  1	    ///< �����ļ��İ汾�ų���
const BYTE g_Magic[] = "ENCLITE";		///< �����ļ��ı�ʶ	
const BYTE g_Ver = 0;		///< �ļ��İ汾��

CFileEncrypt::CFileEncrypt(void)
{
}

CFileEncrypt::~CFileEncrypt(void)
{
}

///
/// @brief
///      ����ģ��ĳ�ʼ��������256λ�����������Կ
///
void CFileEncrypt::EncryptInit(BYTE * pEncodeKey)
{
	int i;
	for(i = 0; i < KEY_LEN / 2; i++)
	{
		pEncodeKey[i] = KEY_LEN / 2 - i - 1;
		pEncodeKey[i + KEY_LEN / 2] = i + KEY_LEN / 2;
	}
	srand((unsigned)GetTickCount());
	for(i = 0; i < RANDOM_ROUND; i++)
	{
		int nFirstRandom = rand() % KEY_LEN;
		int nSecondRandom = rand() % KEY_LEN;
		int nTemp = pEncodeKey[nFirstRandom];
		pEncodeKey[nFirstRandom] = pEncodeKey[nSecondRandom];
		pEncodeKey[nSecondRandom] = nTemp;
	}
	
}


///
/// @brief
///		��ָ���ļ�����, �������ܺ���ļ�dstfile
/// @param[LPCTSTR]
///		srcfile - Ҫ���ܵ��ļ�
/// @param[LPCTSTR]
///		dstfile - ���ܺ���ļ�
/// @return
///		��������ɹ�����ER_OK��ʧ���򷵻�ER_ERR
///
EncryptResult CFileEncrypt::EncryptEncodeFile( LPCTSTR srcfile, LPCTSTR dstfile )
{
	// ������ȡҪ���ܵ��ļ�����
	CFile encrypt_srcFile;
	// ����д������ļ�
	CFile encrypt_dstFile;
	///< ������Կ
	BYTE encode_key[KEY_LEN];
	///< ������Կ
	BYTE decode_key[KEY_LEN];
	//�ж��Ƿ��ܹ����ļ������ʧ���򷵻�ʧ�ܱ��
	if(!encrypt_srcFile.Open(srcfile, CFile::modeRead | CFile::shareDenyNone))
	{
		return ER_ERR;
	}
	if(!encrypt_dstFile.Open(dstfile, CFile::modeWrite | CFile::modeCreate 
		| CFile::shareExclusive))
	{
		return ER_ERR;
	}
	//����������Կ
	EncryptInit(encode_key);
	//���ݲ����ļ�����Կ���ɽ�����Կ
	for(int i = 0; i < KEY_LEN; i++)
	{
		decode_key[encode_key[i]] = i;
	}
	try
	{
		//д������ļ��ı�ʶ
		encrypt_dstFile.Write(g_Magic, MAG_LEN);
		//д������ļ��İ汾��
		encrypt_dstFile.Write(&g_Ver, VER_LEN);
		//�ѽ�����Կд������ļ���
		encrypt_dstFile.Write(decode_key, KEY_LEN);
	}
	catch (CFileException* e)
	{
		e->Delete();
		encrypt_srcFile.Close();
		encrypt_dstFile.Close();
		return ER_ERR;
	}
	BYTE readBuff[DATA_SIZE];
	BYTE writeBuff[DATA_SIZE];
	UINT nRead = DATA_SIZE;
	//������ʵ�����ݳ���
	int nRealRead;
	//����������ݿɶ�
	while((nRealRead = encrypt_srcFile.Read(readBuff, nRead)) != 0)
	{
		//�Ѷ��������Ľ��м���
		EnDecodeData(readBuff, nRealRead, writeBuff, encode_key);
		try
		{
			//д������
			encrypt_dstFile.Write(writeBuff, nRealRead);
		}
		//���д�ļ��в����쳣��д������������������
		catch (CFileException* e)
		{
			e->Delete();
			encrypt_srcFile.Close();
			encrypt_dstFile.Close();
			return ER_ERR;
		}
	}
	encrypt_srcFile.Close();
	encrypt_dstFile.Close();
	//���سɹ����
	return ER_OK;
}

///
/// @brief
///		��ָ���ļ�����, �������ܺ���ļ�dstfile
/// @param[LPCTSTR]
///		srcfile - Ҫ���ܵ��ļ�
/// @param[LPCTSTR]
///		dstfile - ���ܺ���ļ�
/// @return
///		��������ɹ�����ER_OK��ʧ���򷵻�ER_ERR
///
EncryptResult CFileEncrypt::EncryptDecodeFile( LPCTSTR srcfile, LPCTSTR dstfile )
{
	// ������ȡҪ���ܵ��ļ�����
	CFile encrypt_srcFile;
	// ����д����ܺ���ļ�����
	CFile encrypt_dstFile;
	//�������������Կ
	BYTE decode_key[KEY_LEN];
	//�ж��Ƿ��ܹ����ļ������ʧ���򷵻�ʧ�ܱ��
	if(!encrypt_srcFile.Open(srcfile, CFile::modeRead | CFile::shareDenyNone))
	{
		return ER_ERR;
	}
	if(!encrypt_dstFile.Open(dstfile, CFile::modeWrite | CFile::modeCreate
		| CFile::shareExclusive))
	{
		return ER_ERR;
	}
	BYTE chMagic[MAG_LEN];
	BYTE chVer;
	int nMsgLen;
	int nVerLen;
	//�ȶ�ȡ�ļ���ʶ
	nMsgLen = encrypt_srcFile.Read(chMagic, MAG_LEN);
	//��ȡ�ļ��汾��
	nVerLen = encrypt_srcFile.Read(&chVer, VER_LEN);
	//��������ļ����ļ���ʶ�Ͱ汾�Ŵ���
	if(nMsgLen != MAG_LEN || nVerLen != VER_LEN || 
		!EncryptStrEql(chMagic, g_Magic, MAG_LEN) || chVer != g_Ver)
	{
		encrypt_srcFile.Close();
		encrypt_dstFile.Close();
		//����ʧ�ܱ��
		return ER_ERR;
	}
	//�����ȡ�Ľ�����Կ���ȴ���
	if(KEY_LEN != encrypt_srcFile.Read(decode_key, KEY_LEN))
	{
		encrypt_srcFile.Close();
		encrypt_dstFile.Close();
		return ER_ERR;
	};
	BYTE readBuff[DATA_SIZE];
	BYTE writeBuff[DATA_SIZE];
	UINT nRead = DATA_SIZE;
	int nRealRead;
	while((nRealRead = encrypt_srcFile.Read(readBuff, nRead)) != 0)
	{
		//�Ѷ��������Ľ��н���
		EnDecodeData(readBuff, nRealRead, writeBuff, decode_key);
		try
		{
			//������д���ļ�
			encrypt_dstFile.Write(writeBuff, nRealRead);
		}
		catch (CFileException* e)
		{
			e->Delete();
			encrypt_srcFile.Close();
			encrypt_dstFile.Close();
			return ER_ERR;
		}
	}
	encrypt_srcFile.Close();
	encrypt_dstFile.Close();
	return ER_OK;
}


///
/// @brief
///		������Կ�����ݽ��мӽ���
///      
/// @param[BYTE *]
///		pSrcData - Ҫ�ӽ��ܵ�����
/// @param[int]
///		nSrcLen - �ӽ������ݵĳ���
/// @param[BYTE *]
///		pDstData - �ӽ��ܺ������	
/// @param[BYTE *]
///		pCodeKey - �ӽ��ܵ���Կ	
///
void CFileEncrypt::EnDecodeData( BYTE * pSrcData, int nSrcLen, BYTE * pDstData,BYTE * pCodeKey )
{
	while(nSrcLen--)
	{
		//���ö��ԣ����ָ���Ƿ�Ϊ��
		ASSERT(NULL != pSrcData);
		ASSERT(NULL != pDstData);
		//���ݼӽ�����Կ�����ݽ���ӳ��
		(*pDstData++) = pCodeKey[(*pSrcData++)];
	}
}

///
/// @brief
///		�ж�����BYTE�����Ƿ���ͬ
///      
/// @param[pFirByte]
///		pFirByte - Ҫ�жϵĵ�һ������
/// @param[BYTE *]
///		pSecByte - Ҫ�жϵĵڶ�������
/// @param[int]
///		nLen - Ҫ�жϵ����ݳ���
///
bool CFileEncrypt::EncryptStrEql(const BYTE * pFirByte, const BYTE * pSecByte, int nLen)
{
	while(nLen--)
	{
		ASSERT(NULL != pFirByte);
		ASSERT(NULL != pSecByte);
		//�ж������ÿ�����Ƿ���ͬ
		if((*pFirByte++) != (*pSecByte++))
		{
			return FALSE;
		}
	}
	return TRUE;
}
