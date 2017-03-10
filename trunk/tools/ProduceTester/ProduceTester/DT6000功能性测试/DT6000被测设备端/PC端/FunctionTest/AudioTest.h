#ifndef     __AUDIO_TEST_H__
#define     __AUDIO_TEST_H__


//定义音频测试参数命令
#define    PARAM_BEGIN_RECORD           0       //开始录音
#define    PARAM_STOP_RECORD            1       //停止录音
#define    PARAM_BEGIN_PLAY             2       //开始播放声音
#define    PARAM_STOP_PLAY              3       //停止播放声音
#define    PARAM_GET_POS_RECORD         4       //获取录制的声音位置信息
#define    PARAM_GET_POS_PLAY           5       //获取的声音位置信息

class CAudioTest:public CTestBase
{
public:
    CAudioTest();
    ~CAudioTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const;

    TEST_STATUS OnBeginTest(CConnect *pConnect) ;
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);

private:
    BOOL    m_bTestRecord;                  //是否测试录音
    

};


#endif