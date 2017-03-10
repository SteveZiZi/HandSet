#include "StdAfx.h"
#include "FileEncrypt.h"
#include<afx.h>
#define KEY_LEN  256	///< 密钥的长度	
#define RANDOM_ROUND 256	///< 产生密钥的随机度
#define DATA_SIZE 1024  ///< 一次性读写文件的大小
#define  MAG_LEN  7		///< 加密文件的标识号长度
#define  VER_LEN  1	    ///< 加密文件的版本号长度
const BYTE g_Magic[] = "ENCLITE";		///< 加密文件的标识	
const BYTE g_Ver = 0;		///< 文件的版本号

CFileEncrypt::CFileEncrypt(void)
{
}

CFileEncrypt::~CFileEncrypt(void)
{
}

///
/// @brief
///      进行模块的初始化，产生256位的随机加密密钥
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
///		对指定文件加密, 产生加密后的文件dstfile
/// @param[LPCTSTR]
///		srcfile - 要加密的文件
/// @param[LPCTSTR]
///		dstfile - 加密后的文件
/// @return
///		如果操作成功返回ER_OK，失败则返回ER_ERR
///
EncryptResult CFileEncrypt::EncryptEncodeFile( LPCTSTR srcfile, LPCTSTR dstfile )
{
	// 用来读取要加密的文件数据
	CFile encrypt_srcFile;
	// 用来写入加密文件
	CFile encrypt_dstFile;
	///< 加密密钥
	BYTE encode_key[KEY_LEN];
	///< 解密密钥
	BYTE decode_key[KEY_LEN];
	//判断是否能够打开文件，如果失败则返回失败标记
	if(!encrypt_srcFile.Open(srcfile, CFile::modeRead | CFile::shareDenyNone))
	{
		return ER_ERR;
	}
	if(!encrypt_dstFile.Open(dstfile, CFile::modeWrite | CFile::modeCreate 
		| CFile::shareExclusive))
	{
		return ER_ERR;
	}
	//产生加密密钥
	EncryptInit(encode_key);
	//根据产生的加密密钥生成解密密钥
	for(int i = 0; i < KEY_LEN; i++)
	{
		decode_key[encode_key[i]] = i;
	}
	try
	{
		//写入加密文件的标识
		encrypt_dstFile.Write(g_Magic, MAG_LEN);
		//写入加密文件的版本号
		encrypt_dstFile.Write(&g_Ver, VER_LEN);
		//把解密密钥写入加密文件中
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
	//读到的实际数据长度
	int nRealRead;
	//如果还有数据可读
	while((nRealRead = encrypt_srcFile.Read(readBuff, nRead)) != 0)
	{
		//把读到的明文进行加密
		EnDecodeData(readBuff, nRealRead, writeBuff, encode_key);
		try
		{
			//写入密文
			encrypt_dstFile.Write(writeBuff, nRealRead);
		}
		//如果写文件中产生异常如写磁盘已满则会进入下面
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
	//返回成功标记
	return ER_OK;
}

///
/// @brief
///		对指定文件解密, 产生解密后的文件dstfile
/// @param[LPCTSTR]
///		srcfile - 要解密的文件
/// @param[LPCTSTR]
///		dstfile - 解密后的文件
/// @return
///		如果操作成功返回ER_OK，失败则返回ER_ERR
///
EncryptResult CFileEncrypt::EncryptDecodeFile( LPCTSTR srcfile, LPCTSTR dstfile )
{
	// 用来读取要解密的文件数据
	CFile encrypt_srcFile;
	// 用来写入解密后的文件数据
	CFile encrypt_dstFile;
	//用来保存解密密钥
	BYTE decode_key[KEY_LEN];
	//判断是否能够打开文件，如果失败则返回失败标记
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
	//先读取文件标识
	nMsgLen = encrypt_srcFile.Read(chMagic, MAG_LEN);
	//读取文件版本号
	nVerLen = encrypt_srcFile.Read(&chVer, VER_LEN);
	//如果解密文件的文件标识和版本号错误
	if(nMsgLen != MAG_LEN || nVerLen != VER_LEN || 
		!EncryptStrEql(chMagic, g_Magic, MAG_LEN) || chVer != g_Ver)
	{
		encrypt_srcFile.Close();
		encrypt_dstFile.Close();
		//返回失败标记
		return ER_ERR;
	}
	//如果读取的解密密钥长度错误
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
		//把读到的密文进行解密
		EnDecodeData(readBuff, nRealRead, writeBuff, decode_key);
		try
		{
			//把明文写进文件
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
///		根据密钥对数据进行加解密
///      
/// @param[BYTE *]
///		pSrcData - 要加解密的数据
/// @param[int]
///		nSrcLen - 加解密数据的长度
/// @param[BYTE *]
///		pDstData - 加解密后的数据	
/// @param[BYTE *]
///		pCodeKey - 加解密的密钥	
///
void CFileEncrypt::EnDecodeData( BYTE * pSrcData, int nSrcLen, BYTE * pDstData,BYTE * pCodeKey )
{
	while(nSrcLen--)
	{
		//设置断言，检测指针是否为空
		ASSERT(NULL != pSrcData);
		ASSERT(NULL != pDstData);
		//根据加解密密钥对数据进行映射
		(*pDstData++) = pCodeKey[(*pSrcData++)];
	}
}

///
/// @brief
///		判断两个BYTE数组是否相同
///      
/// @param[pFirByte]
///		pFirByte - 要判断的第一个数组
/// @param[BYTE *]
///		pSecByte - 要判断的第二个数组
/// @param[int]
///		nLen - 要判断的数据长度
///
bool CFileEncrypt::EncryptStrEql(const BYTE * pFirByte, const BYTE * pSecByte, int nLen)
{
	while(nLen--)
	{
		ASSERT(NULL != pFirByte);
		ASSERT(NULL != pSecByte);
		//判断数组的每个数是否相同
		if((*pFirByte++) != (*pSecByte++))
		{
			return FALSE;
		}
	}
	return TRUE;
}
