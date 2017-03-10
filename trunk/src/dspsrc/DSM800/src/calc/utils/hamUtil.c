#include <stdlib.h>
#include <string.h>
#include "harmUtil.h"

// 输出复合基逆序排列数组序列到文件
void ptTab(Uint16 perTab[], int factor3, int factor2, int factor1)
{
    int i,j,k,m;
    char str[500]="";
    char str1[20];
    system("echo.>tab.h");
    for(i=0; i<factor3; i++)
    {
        for(j=0; j<factor2; j++)
        {
            strcpy(str, "echo ");            
            for(k=0; k<factor1; k++)
            {
                sprintf(str1,"%4d,",perTab[i*factor2*factor1+j*factor1+k]);
                strcat(str, str1);
            }
            strcat(str,">>tab.h");
            system(str);
        }
        system("echo. >>tab.h");
    }
}