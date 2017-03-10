#include "stdafx.h"
#include <stdio.h>
#include "testBase.h"
#include "TestReport.h"

CTestReport::CTestReport():
m_fpReport(NULL)
{

}

CTestReport::~CTestReport()
{
    EndTestReport();
}

BOOL CTestReport::InitTestReport(const wchar_t *strFileName,
                                 const wchar_t *strReportTitle)
{   
    const BYTE h[]={0xFF,0xFE};
    FILE *fp;
    fp=_wfopen(strFileName,L"wb");
    if (fp ==NULL)
    {
        goto Init_Fail;
    }
    //写入UNICODE文件头
    fwrite(h,1,2,fp);
    //写入html头、head部分、以及body部分的头
    fwprintf(fp,L"<html>\r\n\t" );
    fwprintf(fp,L"<head><title>%s</title>"
        L"<style>body {font-size:18px;font-face:宋体;line-height:36px}td "
        L"{font-size:14px;font-face:宋体;}</style>"
        L"</head> \r\n",strReportTitle);
    fwprintf(fp,L"\t<body bgcolor=\"#dde5e5\">\r\n");
   
    m_fpReport=fp;
    return TRUE;
Init_Fail:
    return FALSE;
}


BOOL CTestReport::EndTestReport()
{
    if (m_fpReport !=NULL)
    {
        //写入body和html的尾部
        fwprintf(m_fpReport,L"\t</body>\r\n");
        fwprintf(m_fpReport,L"</html>" );
        fclose(m_fpReport);
        m_fpReport=NULL;
    }
    return TRUE;
}

BOOL CTestReport::AddReportItem(const wchar_t *strItemName,const SReportNape *pNape,int count)
{
    if (m_fpReport ==NULL)
        return FALSE;

    //写入测试项的标题
    fwprintf(m_fpReport,L"\t\t<center><font color=\"#000099\"><strong>");
    int i=0;
    while(strItemName[i] )
    {
        fwprintf(m_fpReport,L"%c ",strItemName[i]);
        i++;
    }

    fwprintf(m_fpReport,L"</strong></font></center>\r\n");       

    //写入表格的开头
    fwprintf(m_fpReport,L"\t\t<center><table width=\"500\" "
        L" border=\"1\" align=\"center\" cellpadding=\"5\" cellspacing=\"0\">\r\n");

    //写入表格的标题行
    fwprintf(m_fpReport,L"\t\t<tr>"
        L"<td width=\"150\" height=\"20\" align=\"center\">"
        L"<font color=\"#0000CC\"><strong> %s</strong></font></td>",
        L"记录项");
    fwprintf(m_fpReport,L"<td height=\"20\" align=\"left\">"
        L"<font color=\"#0000CC\"><strong>%s</strong></font></td></tr>",
        L"描述");
    fwprintf(m_fpReport,L"\r\n");

    for (int i=0;i<count;i++)
    {
        if (pNape[i].bHighLight)
        {
            fwprintf(m_fpReport,L"\t\t<tr><td width=\"150\" height=\"20\" align=\"center\">"
                L"<font color=\"#FF0000\">%s</font></td>",
                pNape[i].strNape);
            fwprintf(m_fpReport,L"<td height=\"20\" align=\"left\">"
                L"<font color=\"#FF0000\">%s</font></td></tr>",
                pNape[i].strNapeDescriptor);

        }
        else
        {
            fwprintf(m_fpReport,L"\t\t<tr><td width=\"150\" height=\"20\" align=\"center\">"
                L"<font color=\"#006666\">%s</font></td>",
                pNape[i].strNape);
            fwprintf(m_fpReport,L"<td height=\"20\" align=\"left\">"
                L"<font color=\"#006666\">%s</font></td></tr>",
                pNape[i].strNapeDescriptor);

        }
        
        fwprintf(m_fpReport,L"\r\n");
    }
    //写入表格尾
    fwprintf(m_fpReport,L"\t\t</table></center>\r\n");
    fwprintf(m_fpReport,L"\r\n");
    fwprintf(m_fpReport,L"\t\t<br />\r\n");
    return TRUE;
}