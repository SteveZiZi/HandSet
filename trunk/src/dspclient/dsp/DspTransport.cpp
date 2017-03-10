#include "stdafx.h"
#include "DspTransport.h"

#include <stdlib.h>
#include <assert.h>
#include <WinDef.h>
#include <types.h>

#include "../dsplib/loop.h"


#pragma warning(disable:4996)

#define DSP_PROCESSOR_ID 0

unsigned char* CDspTransport::m_dspInBuffer = NULL;
unsigned char* CDspTransport::m_dspOutBuffer = NULL;

#ifdef _DEBUG
unsigned int _total_length = 0;
#endif

bool CDspTransport::Initial(const char* loopOutPath, unsigned int bufferSize)
{
    char tmp[20];
    sprintf(tmp, "%d", bufferSize);
#ifdef _DEBUG
    _total_length = bufferSize;
#endif

    DSP_STATUS status = DSP_Create((char*)loopOutPath, &m_dspInBuffer, tmp, 0);
    if (DSP_FAILED(status))
        return false;
    return true;
}

bool CDspTransport::Send()
{
    assert(NULL != m_dspInBuffer);
    DSP_STATUS status = DSP_Execute_Send(DSP_PROCESSOR_ID);
    if ( DSP_FAILED(status) )
    {
        return FALSE;
    }
    return TRUE;
}

bool CDspTransport::Read()
{
    DSP_STATUS status = DSP_Execute_Receive(DSP_PROCESSOR_ID, &m_dspOutBuffer);
    if ( DSP_FAILED(status) )
	{
		return FALSE;
	}
	return TRUE;
}

void CDspTransport::Destroy()
{
    DSP_Delete(DSP_PROCESSOR_ID);
}

unsigned int CDspTransport::Write(unsigned int offset, const unsigned char* data, unsigned int len)
{
#ifdef _DEBUG
    assert(offset+len < _total_length);
#endif
    memcpy(m_dspInBuffer+offset, data, len);
    return len;
}
