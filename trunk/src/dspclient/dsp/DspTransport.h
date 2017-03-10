#ifndef _DSPTRANSPORT_H
#define _DSPTRANSPORT_H

class CDspTransport
{
public:
    static bool Initial(const char* loopOutPath, unsigned int bufferSize);

    static bool Send();

    static bool Read();

    static unsigned int Write(unsigned int offset, const unsigned char* data, unsigned int len);

    static void Destroy();

    static unsigned char* GetInBuffer()
    {
        return m_dspInBuffer;
    }
    static unsigned char* GetOutBuffer()
    {
        return m_dspOutBuffer;
    }

private:
    static unsigned char*  m_dspInBuffer;
    static unsigned char*  m_dspOutBuffer;
};

#endif // _DSPTRANSPORT_H
