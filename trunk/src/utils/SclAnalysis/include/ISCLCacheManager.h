#ifndef SCL_REDUCER_H_

#define SCL_REDUCER_H_

#include <string>
#include "sclanalysis.h"

// WinCE��Ĭ��ʹ�öԿ����ڴ��Ƿ�����SCL�ļ������ļ��
#ifdef WINCE
#ifndef MEMORY_ENOUGH_CHECK
#define MEMORY_ENOUGH_CHECK
#endif // MEMORY_ENOUGH_CHECK
#endif // WINCE

class ISCLCacheManager
{
public:
	ISCLCacheManager();
	virtual ~ISCLCacheManager();

	///
	/// @brief
	///     ����SMV��Ϣ����������SMV Outputs��Inputs������
	///
	/// @param[in]
	///     bInputsOpt - �Ƿ����Inputs��Ϣ
	///
	virtual void EnableSmvAnalysis(bool bInputsOpt = false) = 0;

	///
	/// @brief
	///     ��ֹSMV��Ϣ����������SMV Outputs��Inputs������
	///
	virtual void DisableSmvAnalysis() = 0;

	///
	/// @brief
	///     ����Goose��Ϣ����������Goose Outputs��Inputs������
	///
	/// @param[in]
	///     bInputsOpt - �Ƿ����Inputs��Ϣ
	///
	virtual void EnableGseAnalysis(bool bInputsOpt = false) = 0;

	///
	/// @brief
	///     ��ֹGoose��Ϣ����������Goose Outputs��Inputs������
	///
	virtual void DisableGseAnalysis() = 0;

	///
	/// @brief
	///     ����SCL�����ļ�����������Ӧ�Ľ���
	///
	/// @param[in]
	///     strFile - SCL�����ļ���(��·��)
	///
	/// @return
	///     0 - �����ɹ��� -1 - ����ʧ��
	///
	virtual int LoadSCL( const std::wstring &strFile ) = 0;

	///
	/// @brief
	///     ж�ش򿪵�SCL�����ļ����ͷŽ�������
	///
	virtual void UnloadSCL(void) = 0;

	///
	/// @brief
	///     ��SCL�ļ���ʽ�������������
	///
	/// @param[in]
	///     strFile - Ҫ���浽��SCL�ļ���(��·��)
	///
	/// @return
	///     0 - �����ɹ��� -1 - ����ʧ��
	///
	virtual int SaveSCL( const std::wstring &strFile ) = 0;
};

///
/// @brief
///     SCL�ļ���������ʵ��
///
/// @return
///     SCL�ļ���������ʵ��ָ��
///
SCLANALYSISDLL_API ISCLCacheManager *CreateSCLCacheManagerImpl( void );

///
/// @brief
///     �ͷ�SCL�ļ���������ʵ��
///
/// @param[in]
///      objPtr - ָ��SCL�ļ���������ʵ����ָ��
///
/// @return
///     
///
SCLANALYSISDLL_API void  RelaseSCLCacheManagerImpl(ISCLCacheManager *objPtr);

#endif
