#ifndef   __TEST_REPORT_H__
#define   __TEST_REPORT_H__

#include <string>
using namespace std;

class   CTestReport
{
public:
    CTestReport();
    ~CTestReport();
    BOOL    InitTestReport(const wchar_t *strFileName,const wchar_t *strReportTitle);
    BOOL    AddReportItem(const wchar_t *strItemName,const SReportNape *pNape,int count);
    BOOL    EndTestReport();


private:
    CTestReport(CTestReport &){};
    FILE        *m_fpReport;                        //指向日志文件
};




#endif