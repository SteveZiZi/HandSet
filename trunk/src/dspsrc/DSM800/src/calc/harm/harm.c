/// @file
///     harm2560.cpp
/// @brief
///     г������
/// @note
///     ���㷨������800�㣬1000�㣬1600�㣬2000���2560��г������
///
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/20
/// @revise
/// 
/*******************************************************************************
г���㷨˵����
     ���㷨ֻ������800�㣬1000�㣬1600�㣬2000���2560��г�����㣬���ø��ϻ�����
 Ҷ�任���м��㡣
 
   FFTг���任˵����
     �����õ��������źţ��Ϳ�����FFT�任�ˡ�N�������㣬����FFT֮�󣬾Ϳ��Եõ�N
 �����FFT������������Ƶ��ΪFs���ź�Ƶ��F����������ΪN����ôFFT֮��������һ
 ��ΪN��ĸ�����ÿһ����Ͷ�Ӧ��һ��Ƶ�ʵ㡣������ģֵ�����Ǹ�Ƶ��ֵ�µķ�����
 �ԡ������ԭʼ�źŵķ�����ʲô��ϵ�أ�����ԭʼ�źŵķ�ֵΪA����ôFFT�Ľ����ÿ
 ���㣨���˵�һ����ֱ������֮�⣩��ģֵ����A��N/2��������һ�������ֱ����������
 ��ģֵ����ֱ��������N������ÿ�������λ�أ������ڸ�Ƶ���µ��źŵ���λ����һ����
 ��ʾֱ����������0Hz���������һ����N������һ���㣨ʵ����������ǲ����ڵģ�����
 �Ǽ���ĵ�N+1���㣬Ҳ���Կ����ǽ���һ�����������֣���һ���Ƶ�������ʾ����
 Ƶ��Fs�����м䱻N-1����ƽ���ֳ�N�ȷݣ�ÿ�����Ƶ���������ӡ�
     ĳ��n����ʾ��Ƶ��Ϊ��Fn=(n-1)*Fs/N��������Ĺ�ʽ���Կ�����Fn���ֱܷ浽Ƶ��
 ΪFs/N���������Ƶ��FsΪ1024Hz����������Ϊ1024�㣬����Էֱ浽1Hz��1024Hz�Ĳ���
 �ʲ���1024�㣬�պ���1�룬Ҳ����˵������1��ʱ����źŲ���FFT���������Է�����
 1Hz���������2��ʱ����źŲ���FFT���������Է�����0.5Hz�����Ҫ���Ƶ�ʷֱ�����
 ��������Ӳ���������Ҳ������ʱ�䡣Ƶ�ʷֱ��ʺͲ���ʱ���ǵ�����ϵ��
    ����FFT֮��ĳ��n�ø���a+bi��ʾ����ô���������ģ����An=sqrt(a*a+b*b)����λ��
��Pn=atan2(b,a)���������ϵĽ�����Ϳ��Լ����n�㣨n��1����n<=N/2����Ӧ���źŵı�
��ʽΪ��An/(N/2)*cos(2*pi*Fn*t+Pn)����2*An/N*cos(2*pi*Fn*t+Pn)������n=1����źţ�
��ֱ�����������ȼ�ΪA1/N��
    ����FFT����ĶԳ��ԣ�ͨ������ֻʹ��ǰ�벿�ֵĽ������С�ڲ���Ƶ��һ��Ľ����
    ����������һ���źţ�������2V��ֱ��������Ƶ��Ϊ50Hz����λΪ-30�ȡ�����Ϊ3V��
�����źţ��Լ�һ��Ƶ��Ϊ100Hz����λΪ90�ȡ�����Ϊ1.5V�Ľ����źš�����ѧ���ʽ��
�����£�
    S=2+3*cos(2*pi*50*t-pi*30/180)+1.5*cos(2*pi*100*t+pi*90/180)
    ʽ��cos����Ϊ���ȣ�����-30�Ⱥ�90��Ҫ�ֱ���ɻ��ȡ�������4000Hz�Ĳ����ʶ���
���źŽ��в������ܹ�����800�㡣������������ķ�����Fn=(n-1)*Fs/N�����ǿ���֪����
ÿ������֮��ļ�����Fs/N=5Hz����n�����Ƶ�ʾ���5*(n-1)�����ǵ��ź���3��Ƶ�ʣ�
0Hz��50Hz��100Hz��Ӧ�÷ֱ��ڵ�1���㡢��11���㡢��21�����ϳ��ַ�ֵ����������Ӧ��
�ӽ�0��
    ��ͨ��FFT���������800�����ֵ�����ţ��������������ķ���ֵ���ֱ����������
���ģֵ��������£�
    1�㣺 512
    11�㣺384
    21�㣺192
    ���չ�ʽ�����Լ����ֱ������Ϊ��512/N=512/256=2��50Hz�źŵķ���Ϊ��
384/(N/2)=384/(256/2)=3��100Hz�źŵķ���Ϊ192/(N/2)=192/(256/2)=1.5��
    Ȼ������������λ��Ϣ��ֱ���ź�û����λ���ԣ����ù�����50Hz�źŵ���λΪ��
180*(-0.5236)/pi=-30.0001��100Hz�źŵķ���Ϊ180*1.5708/pi=90.0002��
*******************************************************************************/
#include "harm.h"
#include "perTab.h"                         ///< �����Ѿ�����õ����������
//��ӿ�ִ���ٶ�
#include "../fft/fft.h"
/*
**  г������
*/
/*******************************************************************************
����г��������һЩ������������ͬ���͵�г�������������ͼ���֮���г����
*******************************************************************************/
Uint16 harmSampCount[HARM_LAST] = {800,1000, 1600, 2000, 2560};
Uint16 harmCount[HARM_LAST] = {39, 49, 89, 99, 127};
Uint16 *harmTab[HARM_LAST] = {perTab800, perTab1000, perTab1600, perTab2000, 
    perTab2560};
/*******************************************************************************
��ʽ�ֽ����ӣ���Ϊ��άFFT�任�Ļ�ϻ����磺800=8*10*10����8*10*10�Ļ�ϻ�
*******************************************************************************/
const Uint16 harmFactors[HARM_LAST][3] =
{
    {10,10,8},
    {10,10,10},
    {10,10,16},
    {10,10,20},
    {10,16,16}
};

/******************************************************************************/
#define EFF_VALUE_FACTOR         0.70710678118654752440     ///< ��Чֵת������                                                                  

static float yRe[MAX_HARM_SAMP_COUNT],yIm[MAX_HARM_SAMP_COUNT];
//static float fPhaseAngle[2];

///
/// @brief
///    г�������㷨
/// @note
///    �����еĲ��������ǲ���ֵ�е����ݸ�������������������ڼ����������ݸ�������
///    ���ش����������������������������ݸ��������Լ�����������ݸ���Ϊ׼����
///    �紫һ��3000���������HARM_2560�������ͼ���г������ֻȡ������ǰ2560������
///    ������
/// @param[in] 
///    harmType             ��������
///    nSampleSize          ��������
///    pSampleValues        ����ֵ����
///    nHarmSize            �����г������
/// @param[out]
///    pDCValue             ֱ��ֵ
///    pHarmContents        HARM��г����Чֵ
///    pHarmPhaseAngles     HARM��г����λ�ǣ����ң�
/// @return
///    int                  0���ɹ�
///                        -1������Ĳ�����������
///                        -2�������г�������������ܼ���Ĵ���
///
int harmCalc(HARM_TYPE harmType, Uint16 nSampleSize, const float pSampleValues[], 
             Uint16 nHarmSize, float *pDCValue, float pHarmContents[], 
             float pHarmPhaseAngles[])
{
    int i,j;
    float fAnalogue[6];
    float fTmp,fTmp1,fTmp2,fTmp3;
    float fAngle;
    Uint16 nHarmSampCount = harmSampCount[harmType];
    Uint16 nHarmCount = harmCount[harmType];
    Uint16 *pHarmTab = harmTab[harmType];
    Uint16 nHarmFactors[3];
//    float xRe[MAX_HARM_SAMP_COUNT] = {0.0f};
    float xIm[MAX_HARM_SAMP_COUNT] = {0.0f};

    for(i=0; i<3; i++)
    {
        nHarmFactors[i] = harmFactors[harmType][i];
    }
    // �������
    if(nSampleSize < nHarmSampCount )
    {
        return -1;
    }
    if(nHarmSize > nHarmCount)    
    {
        return -2;
    }
   
    // �Ż��˴�ת�����룬����߾��Ⱥ��ٶȣ�2013-8-12��
#if 0
    for(i=0; i<nHarmSampCount; i++)
    {
        xRe[i] = pSampleValues[i];
        xIm[i] = 0;
    }
#endif
    permute(pHarmTab, nHarmSampCount, pSampleValues, xIm, yRe, yIm);               /*  ����  */

    //  1��nHarmFactors[1]*nHarmFactors[2]�Σ�ȡ����FFT_10���㡣
    twiddleTransf(  1,  nHarmFactors[0], nHarmFactors[1]*nHarmFactors[2], yRe, yIm);
    //  nHarmFactors[0]�� nHarmFactors[2]�Σ�ȡ����FFT_16���㡣
    twiddleTransf(nHarmFactors[0], nHarmFactors[1], nHarmFactors[2], yRe, yIm);
    // nHarmFactors[1]*nHarmFactors[0]�� 1�Σ�ȡ����FFT_16���㡣
    twiddleTransf( nHarmFactors[1]*nHarmFactors[0], nHarmFactors[2], 1, yRe, yIm);

    // ����ֱ��ֵ
    fTmp = mpysp_i(fabs(yRe[0]),(1.0/nHarmSampCount));       // 1/N10
    *pDCValue=fTmp;
    
    // ����г����Чֵ����λ��
    for(i=0;i<nHarmSize;i++)
    {
        j = i*10+10;

        // ȡ��j-1,j,j+1��г��ֵ
        fAnalogue[0] = addsp_i(yRe[j-1],yRe[nHarmSampCount-(j-1)]);
        fAnalogue[1] = subsp_i(yIm[j-1],yIm[nHarmSampCount-(j-1)]);
        fAnalogue[2] = addsp_i(yRe[j],yRe[nHarmSampCount-j]);
        fAnalogue[3] = subsp_i(yIm[j],yIm[nHarmSampCount-j]);
        fAnalogue[4] = addsp_i(yRe[j+1],yRe[nHarmSampCount-(j+1)]);
        fAnalogue[5] = subsp_i(yIm[j+1],yIm[nHarmSampCount-(j+1)]);

        fTmp1 = addsp_i(mpysp_i(fAnalogue[0],fAnalogue[0]) , 
            mpysp_i(fAnalogue[1],fAnalogue[1]));
        fTmp2 = addsp_i(mpysp_i(fAnalogue[2],fAnalogue[2]) , 
            mpysp_i(fAnalogue[3],fAnalogue[3]));
        fTmp3 = addsp_i(mpysp_i(fAnalogue[4],fAnalogue[4]) , 
            mpysp_i(fAnalogue[5],fAnalogue[5]));

        // �����j��г������λ��
        if(fAnalogue[2])
        {
            fAngle = atanf(divsp_i(fAnalogue[3],fAnalogue[2]));
            if((fAnalogue[2]<0)&&(fAnalogue[3]>0))
            {
                fAngle = addsp_i(fAngle,PI);
            }
            else if((fAnalogue[2]<0)&&(fAnalogue[3]<0))
            {
                fAngle = subsp_i(fAngle,PI);
            }
        }
        else
        {
            if(fAnalogue[3]>0)fAngle = PI0_5;
            else fAngle = -PI0_5;
        }

        // ��λ�ǵ���
        fAngle += PI0_5;
        while(fAngle>PI2)fAngle = subsp_i(fAngle,PI2);
        while(fAngle<0.0)fAngle = addsp_i(fAngle,PI2);

        // ����->�Ƕ�
        fAngle = mpysp_i(fAngle,ARC2ANGLE_FACTOR);     // 180/PI

        // ��������ֵ?
        /*if(i==0)
        {
            fPhaseAngle[0] = fAngle;
            pHarmPhaseAngle[i] = 0;
        }
        pHarmPhaseAngle[i] = subsp_i(fAngle,fPhaseAngle[0]);*/
        pHarmPhaseAngles[i] = fAngle;

        // ����г����Чֵ
        fTmp = sqrtsp_i(fTmp2);
        fTmp = mpysp_i(fTmp,(1.0/nHarmSampCount));       // 1/N10
        fTmp = mpysp_i(fTmp,EFF_VALUE_FACTOR);  // 1/sqrt(2)
        pHarmContents[i] = fTmp;
        
        // ����г������ֵ���������Ժ�ʹ�ã�
        /*fTmp = addsp_i(fTmp1,addsp_i(fTmp2,fTmp3));
        fTmp = sqrtsp_i(fTmp);
        fTmp = mpysp_i(fTmp,1.3810679e-4);       // 1/(N10*SQRT2);
        fTmp = mpysp_i(fTmp,fScale[nType]);
        fTmp = mpysp_i(fTmp,paramBuf[4+nType*3]);
        reDat.nHarmSubGroup[nType][i] = fTmp;*/
    }
    return 0;
}

///
/// @brief
///    2·г�������㷨
/// @note
///    �����еĲ��������ǲ���ֵ�е����ݸ�������������������ڼ����������ݸ�������
///    ���ش����������������������������ݸ��������Լ�����������ݸ���Ϊ׼����
///    �紫һ��3000���������HARM_2560�������ͼ���г������ֻȡ������ǰ2560������
///    �����㣬���㷨��������˸���FFT�ļ��㣬����2·���ͨ����ʽ��
///    1·fft��� = ��1·+2·�������/2,2·fft���=��1·-2·�������*(-j)/2��
///    ����fft�������
/// @param[in] 
///    harmType             ��������
///    nSampleSize          ��������
///    pSampleValues        1·����ֵ����
///    pSampleValues1       2·����ֵ����
///    nHarmSize            �����г������
/// @param[out]
///    pDCValue             1·ֱ��ֵ
///    pDCValue1            2·ֱ��ֵ
///    pHarmContents        1·HARM��г����Чֵ
///    pHarmContents1       2·HARM��г����Чֵ
///    pHarmPhaseAngles     1·HARM��г����λ�ǣ����ң�
///    pHarmPhaseAngles1    2·HARM��г����λ�ǣ����ң�
/// @return
///    int                  0���ɹ�
///                        -1������Ĳ�����������
///                        -2�������г�������������ܼ���Ĵ���
///
int harmCalc2(HARM_TYPE harmType, Uint16 nSampleSize, const float pSampleValues[], 
             const float pSampleValues1[], Uint16 nHarmSize, float *pDCValue,
             float *pDCValue1, float pHarmContents[], float pHarmContents1[],
             float pHarmPhaseAngles[], float pHarmPhaseAngles1[])
{
    int i,j,k;
    float fAnalogue[2][6];
    float fTmp[2],fTmp1[2],fTmp2[2],fTmp3[2];
    float fAngle[2];
    Uint16 nHarmSampCount = harmSampCount[harmType];
    Uint16 nHarmCount = harmCount[harmType];
    Uint16 *pHarmTab = harmTab[harmType];
    Uint16 nHarmFactors[3];
    float *pHarmContents2[2];
    float *pHarmPhaseAngles2[2];
    //float xRe[MAX_HARM_SAMP_COUNT] = {0.0f};
    //float xIm[MAX_HARM_SAMP_COUNT] = {0.0f};

    pHarmContents2[0] = pHarmContents;
    pHarmContents2[1] = pHarmContents1;
	pHarmPhaseAngles2[0] = pHarmPhaseAngles;
	pHarmPhaseAngles2[1] = pHarmPhaseAngles1;

    for(i=0; i<3; i++)
    {
        nHarmFactors[i] = harmFactors[harmType][i];
    }
    // �������
    if(nSampleSize < nHarmSampCount )
    {
        return -1;
    }
    if(nHarmSize > nHarmCount)    
    {
        return -2;
    }
   
    permute(pHarmTab, nHarmSampCount, pSampleValues, pSampleValues1, yRe, yIm);               /*  ����  */

    //  1��256�Σ�ȡ����FFT_10���㡣
    twiddleTransf(  1,  nHarmFactors[0], nHarmFactors[1]*nHarmFactors[2], yRe, yIm);
    //  10�� 16�Σ�ȡ����FFT_16���㡣
    twiddleTransf(nHarmFactors[0], nHarmFactors[1], nHarmFactors[2], yRe, yIm);
    // 160�� 1�Σ�ȡ����FFT_16���㡣
    twiddleTransf( nHarmFactors[1]*nHarmFactors[0], nHarmFactors[2], 1, yRe, yIm);

    // ����ֱ��ֵ
    fTmp[0] = mpysp_i(fabs(yRe[0]),(1.0/nHarmSampCount));       // 1/N10
    *pDCValue=fTmp[0];
    fTmp[1] = mpysp_i(fabs(yIm[0]),(1.0/nHarmSampCount));       // 1/N10
    *pDCValue1=fTmp[1];
    
    // ����г����Чֵ����λ��
    for(i=0;i<nHarmSize;i++)
    {
        j = i*10+10;

        // ȡ��1·j-1,j,j+1��г��ֵ
        fAnalogue[0][0] = addsp_i(yRe[j-1],yRe[nHarmSampCount-(j-1)]);
        fAnalogue[0][1] = subsp_i(yIm[j-1],yIm[nHarmSampCount-(j-1)]);
        fAnalogue[0][2] = addsp_i(yRe[j],yRe[nHarmSampCount-j]);
        fAnalogue[0][3] = subsp_i(yIm[j],yIm[nHarmSampCount-j]);
        fAnalogue[0][4] = addsp_i(yRe[j+1],yRe[nHarmSampCount-(j+1)]);
        fAnalogue[0][5] = subsp_i(yIm[j+1],yIm[nHarmSampCount-(j+1)]);

        // ȡ��2·j-1,j,j+1��г��ֵ
        fAnalogue[1][0] = addsp_i(yIm[j-1],yIm[nHarmSampCount-(j-1)]);
        fAnalogue[1][1] = subsp_i(yRe[nHarmSampCount-(j-1)],yRe[j-1]);
        fAnalogue[1][2] = addsp_i(yIm[j],yIm[nHarmSampCount-j]);
        fAnalogue[1][3] = subsp_i(yRe[nHarmSampCount-j],yRe[j]);
        fAnalogue[1][4] = addsp_i(yIm[j+1],yIm[nHarmSampCount-(j+1)]);
        fAnalogue[1][5] = subsp_i(yRe[nHarmSampCount-(j+1)],yRe[j+1]);

        // ����г����Чֵ����λ��
        for(k=0; k<2; k++)
        {		
            fTmp1[k] = addsp_i(mpysp_i(fAnalogue[k][0],fAnalogue[k][0]) , 
                mpysp_i(fAnalogue[k][1],fAnalogue[k][1]));
            fTmp2[k] = addsp_i(mpysp_i(fAnalogue[k][2],fAnalogue[k][2]) , 
                mpysp_i(fAnalogue[k][3],fAnalogue[k][3]));
            fTmp3[k] = addsp_i(mpysp_i(fAnalogue[k][4],fAnalogue[k][4]) , 
                mpysp_i(fAnalogue[k][5],fAnalogue[k][5]));

            // �����j��г������λ��
            if(fAnalogue[k][2])
            {
                fAngle[k] = atanf(divsp_i(fAnalogue[k][3],fAnalogue[k][2]));
                if((fAnalogue[k][2]<0)&&(fAnalogue[k][3]>0))
                {
                    fAngle[k] = addsp_i(fAngle[k],PI);
                }
                else if((fAnalogue[k][2]<0)&&(fAnalogue[k][3]<0))
                {
                    fAngle[k] = subsp_i(fAngle[k],PI);
                }
            }
            else
            {
                if(fAnalogue[k][3]>0)fAngle[k] = PI0_5;
                else fAngle[k] = -PI0_5;
            }

            // ��λ�ǵ���
            fAngle[k] += PI0_5;
            while(fAngle[k]>PI2)fAngle[k] = subsp_i(fAngle[k],PI2);
            while(fAngle[k]<0.0)fAngle[k] = addsp_i(fAngle[k],PI2);

            // ����->�Ƕ�
            fAngle[k] = mpysp_i(fAngle[k],ARC2ANGLE_FACTOR);     // 180/PI

            // ��������ֵ?
            /*if(i==0)
            {
                fPhaseAngle[0] = fAngle;
                pHarmPhaseAngle[i] = 0;
            }
            pHarmPhaseAngle[i] = subsp_i(fAngle,fPhaseAngle[0]);*/
            pHarmPhaseAngles2[k][i] = fAngle[k];

            // ����г����Чֵ
            fTmp[k] = sqrtsp_i(fTmp2[k]);
            fTmp[k] = mpysp_i(fTmp[k],(1.0/nHarmSampCount));       // 1/N10
            fTmp[k] = mpysp_i(fTmp[k],EFF_VALUE_FACTOR);  // 1/sqrt(2)
            pHarmContents2[k][i] = fTmp[k];
            
            // ����г������ֵ���������Ժ�ʹ�ã�
            /*fTmp = addsp_i(fTmp1,addsp_i(fTmp2,fTmp3));
            fTmp = sqrtsp_i(fTmp);
            fTmp = mpysp_i(fTmp,1.3810679e-4);       // 1/(N10*SQRT2);
            fTmp = mpysp_i(fTmp,fScale[nType]);
            fTmp = mpysp_i(fTmp,paramBuf[4+nType*3]);
            reDat.nHarmSubGroup[nType][i] = fTmp;*/
        }
    }
    return 0;
}

///
/// @brief
///    г�������ʼ����㷨
/// @param[in] 
///    harmType             ��������
///    nHarmSize            г������
///    pHarmContents        nHarmSize��г����Чֵ
///    nType                1����ѹ��2������
/// @param[out]
///    pTHDValue            nHarmSize��г��������
/// @return
///    int                  0���ɹ�
///                        -1��г�������������ܼ����������
///
int harmSumTHDCalc(HARM_TYPE harmType, Uint16 nHarmSize, 
               const float pHarmContents[], Int16 nType, float *pTHDValue)
{
    float TDHSum = 0;
    Int i=0;
    float baseHarmContentValue = pHarmContents[0];
    Uint16 nHarmCount = harmCount[harmType];

    // �������
    if(nHarmSize > nHarmCount)
    {
        return -1;
    }

    for(i=1; i<nHarmSize; i++)
    {
       TDHSum = TDHSum + mpysp_i(pHarmContents[i],pHarmContents[i]);
    }    
    TDHSum = sqrtsp_i(TDHSum);
    switch(nType)
    {
        case 1:
             if((baseHarmContentValue > 0.1))
             {
                 *pTHDValue= mpysp_i(divsp_i(TDHSum,baseHarmContentValue),100);
             }
             else
             {
                 *pTHDValue= 0;
             }

             break;
        case 2:
             if((baseHarmContentValue>0.02))
             {
                 *pTHDValue= mpysp_i(divsp_i(TDHSum,baseHarmContentValue),100);
             }
             else
             {
                 *pTHDValue = 0;
             }
             break;
        default:
		     if((baseHarmContentValue>0.0001))
             {
                 *pTHDValue= mpysp_i(divsp_i(TDHSum,baseHarmContentValue),100);
             }
             else
             {
                 *pTHDValue = 0;
             }
		    break;
    }
    return 0;
}

///
/// @brief
///    г�������ʼ����㷨
/// @param[in]
///    harmType             ��������
///    nHarmSize            г������
///    pHarmContents        nHarmSize��г����Чֵ
///    nType                1����ѹ��2������
/// @param[out]
///    pTHDValues           nHarmSize��г��������
/// @return
///    int                  0���ɹ�
///                        -1��г�������������ܼ����������
///
int harmTHDCalc(HARM_TYPE harmType, Uint16 nHarmSize, const float pHarmContents[],
                Int16 nType, float pTHDValues[])
{
    Uint16 i=0;
    float baseHarmContentValue = pHarmContents[0];    
    Uint16 nHarmCount = harmCount[harmType];

    // �������   
    if(nHarmSize > nHarmCount)
    {
        return -1;
    }

    switch(nType)
    {
        case 1:
            if((baseHarmContentValue>0.1))
            {
                for(i=0; i<nHarmSize; i++)
                 {
                    pTHDValues[i] = mpysp_i(divsp_i(pHarmContents[i],baseHarmContentValue),100);
                 }
            }
            else
            {
                for(i=0; i<nHarmSize; i++)
                 {
                    pTHDValues[i] = 0;
                 }
            }
             break;

        case 2:
             if((baseHarmContentValue>0.02))
             {
                 for(i=0; i<nHarmSize; i++)
                 {
                    pTHDValues[i] = mpysp_i(divsp_i(pHarmContents[i],baseHarmContentValue),100);
                 }
             }
            else
            {
                 for(i=0; i<nHarmSize; i++)
                 {
                    pTHDValues[i] = 0;
                 }
             }
             break;
        default:
		    if((baseHarmContentValue>0.0001))
             {
                 for(i=0; i<nHarmSize; i++)
                 {
                    pTHDValues[i] = mpysp_i(divsp_i(pHarmContents[i],baseHarmContentValue),100);
                 }
             }
            else
            {
                 for(i=0; i<nHarmSize; i++)
                 {
                    pTHDValues[i] = 0;
                 }
            }
             break;
    }
    return 0;
}

///
/// @brief
///    ֱ�������ʼ����㷨
/// @param[in]
///    fDCValue             ֱ����С
///    fBaseHarmContent     ������Чֵ
/// @param[out]
///    pTHDValue            ֱ��������
/// @return
///    int                  0���ɹ�
///
int dcTHDCalc(float fDCValue, float fBaseHarmContent, Int16 nType, 
			float *pTHDValue)
{	
    switch(nType)
    {
        case 1:
             if((fBaseHarmContent > 0.1))
             {
                 *pTHDValue= mpysp_i(divsp_i(fDCValue,fBaseHarmContent),100);
             }
             else
             {
                 *pTHDValue= 0;
             }

             break;
        case 2:
             if((fBaseHarmContent>0.02))
             {
                 *pTHDValue= mpysp_i(divsp_i(fDCValue,fBaseHarmContent),100);
             }
             else
             {
                 *pTHDValue = 0;
             }
             break;
        default:
		    if((fBaseHarmContent>0.0001))
			{
				*pTHDValue= mpysp_i(divsp_i(fDCValue,fBaseHarmContent),100);
			}
			else
			{
				*pTHDValue = 0;
			}
		    break;
    }
    return 0;	
}
