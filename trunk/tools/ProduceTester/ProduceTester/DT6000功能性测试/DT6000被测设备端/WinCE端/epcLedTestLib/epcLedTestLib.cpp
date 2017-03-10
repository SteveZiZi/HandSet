#include <windows.h>
#include "epcLedTestLib.h"


/*********************************************************************************************************
����IO������
*********************************************************************************************************/
#define     LED1_ON         1                                           /*  ����LED1                    */
#define     LED1_OFF        2                                           /*  Ϩ��LED1                    */
#define     LED2_ON         3                                           /*  ����LED2                    */
#define     LED2_OFF        4                                           /*  Ϩ��LED2                    */
#define     LED1_GET_STATUS 5                                           /*  ��ȡLED1״̬                */
#define     LED2_GET_STATUS 6                                           /*  ��ȡLED2״̬                */

#define     EX_LED1_ON      11                                          /*  ����LED1                    */
#define     EX_LED1_OFF     12                                          /*  Ϩ��LED1                    */
#define     EX_LED2_ON      13                                          /*  ����LED2                    */
#define     EX_LED2_OFF     14                                          /*  Ϩ��LED2                    */
#define     EX_LED3_ON      15                                          /*  ����LED3                    */
#define     EX_LED3_OFF     16                                          /*  Ϩ��LED3                    */
#define     EX_LED4_ON      17                                          /*  ����LED4                    */
#define     EX_LED4_OFF     18                                          /*  Ϩ��LED4                    */


static HANDLE   hLedFile = INVALID_HANDLE_VALUE;          //LED�����ļ����
static DWORD    dwLastStatus = 0;
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call,LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hLedFile = CreateFile(TEXT("LED1:"),                         /*  �� LED ����              */
            GENERIC_READ | GENERIC_WRITE,0, NULL,OPEN_EXISTING, 0,0);
        if (hLedFile == INVALID_HANDLE_VALUE) {
            return FALSE; 
        }
        epcSetLedMask(0);                                           //�ڿ�ʼ��ʱ��ر�����LED��
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (hLedFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hLedFile);                                  /*  �ر� LED ����               */
            hLedFile = INVALID_HANDLE_VALUE;
        }
        break;
    }
    return TRUE;
}

void epcGetLedMask(DWORD *dwMask)
{
    *dwMask = dwLastStatus;
}

void epcSetLedMask(DWORD dwMask)
{
    for (int i=0;i<4;i++)
    {
        if (dwMask & (1<<i))
        {
            ::DeviceIoControl(hLedFile, EX_LED1_ON+i*2, NULL, 1, NULL, 0, NULL, NULL);
        }
        else
        {
            ::DeviceIoControl(hLedFile, EX_LED1_OFF+i*2, NULL, 1, NULL, 0, NULL, NULL);
        }
    }

    dwLastStatus = dwMask;
}