/// @file
///     harm2560.cpp
/// @brief
///     谐波计算
/// @note
///     本算法适用于800点，1000点，1600点，2000点和2560点谐波计算
///
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/20
/// @revise
/// 
/*******************************************************************************
谐波算法说明：
     本算法只适用于800点，1000点，1600点，2000点和2560点谐波计算，采用复合基傅里
 叶变换进行计算。
 
   FFT谐波变换说明：
     采样得到的数字信号，就可以做FFT变换了。N个采样点，经过FFT之后，就可以得到N
 个点的FFT结果。假设采样频率为Fs，信号频率F，采样点数为N。那么FFT之后结果就是一
 个为N点的复数。每一个点就对应着一个频率点。这个点的模值，就是该频率值下的幅度特
 性。具体跟原始信号的幅度有什么关系呢？假设原始信号的峰值为A，那么FFT的结果的每
 个点（除了第一个点直流分量之外）的模值就是A的N/2倍。而第一个点就是直流分量，它
 的模值就是直流分量的N倍。而每个点的相位呢，就是在该频率下的信号的相位。第一个点
 表示直流分量（即0Hz），而最后一个点N的再下一个点（实际上这个点是不存在的，这里
 是假设的第N+1个点，也可以看做是将第一个点分做两半分，另一半移到最后）则表示采样
 频率Fs，这中间被N-1个点平均分成N等份，每个点的频率依次增加。
     某点n所表示的频率为：Fn=(n-1)*Fs/N。由上面的公式可以看出，Fn所能分辨到频率
 为Fs/N，如果采样频率Fs为1024Hz，采样点数为1024点，则可以分辨到1Hz。1024Hz的采样
 率采样1024点，刚好是1秒，也就是说，采样1秒时间的信号并做FFT，则结果可以分析到
 1Hz，如果采样2秒时间的信号并做FFT，则结果可以分析到0.5Hz。如果要提高频率分辨力，
 则必须增加采样点数，也即采样时间。频率分辨率和采样时间是倒数关系。
    假设FFT之后某点n用复数a+bi表示，那么这个复数的模就是An=sqrt(a*a+b*b)，相位就
是Pn=atan2(b,a)。根据以上的结果，就可以计算出n点（n≠1，且n<=N/2）对应的信号的表
达式为：An/(N/2)*cos(2*pi*Fn*t+Pn)，即2*An/N*cos(2*pi*Fn*t+Pn)。对于n=1点的信号，
是直流分量，幅度即为A1/N。
    由于FFT结果的对称性，通常我们只使用前半部分的结果，即小于采样频率一半的结果。
    假设我们有一个信号，它含有2V的直流分量，频率为50Hz、相位为-30度、幅度为3V的
交流信号，以及一个频率为100Hz、相位为90度、幅度为1.5V的交流信号。用数学表达式就
是如下：
    S=2+3*cos(2*pi*50*t-pi*30/180)+1.5*cos(2*pi*100*t+pi*90/180)
    式中cos参数为弧度，所以-30度和90度要分别换算成弧度。我们以4000Hz的采样率对这
个信号进行采样，总共采样800点。按照我们上面的分析，Fn=(n-1)*Fs/N，我们可以知道，
每两个点之间的间距就是Fs/N=5Hz，第n个点的频率就是5*(n-1)。我们的信号有3个频率：
0Hz、50Hz、100Hz，应该分别在第1个点、第11个点、第21个点上出现峰值，其它各点应该
接近0。
    先通过FFT计算算出这800个点的值，接着，我们来计算各点的幅度值。分别计算这三个
点的模值，结果如下：
    1点： 512
    11点：384
    21点：192
    按照公式，可以计算出直流分量为：512/N=512/256=2；50Hz信号的幅度为：
384/(N/2)=384/(256/2)=3；100Hz信号的幅度为192/(N/2)=192/(256/2)=1.5。
    然后再来计算相位信息。直流信号没有相位可言，不用管它。50Hz信号的相位为：
180*(-0.5236)/pi=-30.0001；100Hz信号的幅度为180*1.5708/pi=90.0002。
*******************************************************************************/
#include "harm.h"
#include "perTab.h"                         ///< 引入已经定义好的逆序矩阵数
//组加快执行速度
#include "../fft/fft.h"
/*
**  谐波分析
*/
/*******************************************************************************
定义谐波分析的一些参数，包括不同类型的谐波采样点数，和计算之后的谐波数
*******************************************************************************/
Uint16 harmSampCount[HARM_LAST] = {800,1000, 1600, 2000, 2560};
Uint16 harmCount[HARM_LAST] = {39, 49, 89, 99, 127};
Uint16 *harmTab[HARM_LAST] = {perTab800, perTab1000, perTab1600, perTab2000, 
    perTab2560};
/*******************************************************************************
因式分解因子，作为三维FFT变换的混合基，如：800=8*10*10代表8*10*10的混合基
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
#define EFF_VALUE_FACTOR         0.70710678118654752440     ///< 有效值转换因子                                                                  

static float yRe[MAX_HARM_SAMP_COUNT],yIm[MAX_HARM_SAMP_COUNT];
//static float fPhaseAngle[2];

///
/// @brief
///    谐波计算算法
/// @note
///    参数中的采样点数是采样值中的数据个数，如果采样点数少于计算所需数据个数，将
///    返回错误，如果采样点数多余计算所需数据个数，则以计算所需的数据个数为准，比
///    如传一个3000点的数组用HARM_2560计算类型计算谐波，将只取数组中前2560个数据
///    来计算
/// @param[in] 
///    harmType             计算类型
///    nSampleSize          采样点数
///    pSampleValues        采样值数组
///    nHarmSize            输出的谐波次数
/// @param[out]
///    pDCValue             直流值
///    pHarmContents        HARM次谐波有效值
///    pHarmPhaseAngles     HARM次谐波相位角（正弦）
/// @return
///    int                  0：成功
///                        -1：输入的采样点数不够
///                        -2：输出的谐波次数大于所能计算的次数
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
    // 参数检查
    if(nSampleSize < nHarmSampCount )
    {
        return -1;
    }
    if(nHarmSize > nHarmCount)    
    {
        return -2;
    }
   
    // 优化此处转换代码，以提高精度和速度（2013-8-12）
#if 0
    for(i=0; i<nHarmSampCount; i++)
    {
        xRe[i] = pSampleValues[i];
        xIm[i] = 0;
    }
#endif
    permute(pHarmTab, nHarmSampCount, pSampleValues, xIm, yRe, yIm);               /*  排序  */

    //  1个nHarmFactors[1]*nHarmFactors[2]次，取数，FFT_10计算。
    twiddleTransf(  1,  nHarmFactors[0], nHarmFactors[1]*nHarmFactors[2], yRe, yIm);
    //  nHarmFactors[0]个 nHarmFactors[2]次，取数，FFT_16计算。
    twiddleTransf(nHarmFactors[0], nHarmFactors[1], nHarmFactors[2], yRe, yIm);
    // nHarmFactors[1]*nHarmFactors[0]个 1次，取数，FFT_16计算。
    twiddleTransf( nHarmFactors[1]*nHarmFactors[0], nHarmFactors[2], 1, yRe, yIm);

    // 计算直流值
    fTmp = mpysp_i(fabs(yRe[0]),(1.0/nHarmSampCount));       // 1/N10
    *pDCValue=fTmp;
    
    // 计算谐波有效值和相位角
    for(i=0;i<nHarmSize;i++)
    {
        j = i*10+10;

        // 取第j-1,j,j+1次谐波值
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

        // 计算第j次谐波的相位角
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

        // 相位角调整
        fAngle += PI0_5;
        while(fAngle>PI2)fAngle = subsp_i(fAngle,PI2);
        while(fAngle<0.0)fAngle = addsp_i(fAngle,PI2);

        // 弧度->角度
        fAngle = mpysp_i(fAngle,ARC2ANGLE_FACTOR);     // 180/PI

        // 与基波求差值?
        /*if(i==0)
        {
            fPhaseAngle[0] = fAngle;
            pHarmPhaseAngle[i] = 0;
        }
        pHarmPhaseAngle[i] = subsp_i(fAngle,fPhaseAngle[0]);*/
        pHarmPhaseAngles[i] = fAngle;

        // 计算谐波有效值
        fTmp = sqrtsp_i(fTmp2);
        fTmp = mpysp_i(fTmp,(1.0/nHarmSampCount));       // 1/N10
        fTmp = mpysp_i(fTmp,EFF_VALUE_FACTOR);  // 1/sqrt(2)
        pHarmContents[i] = fTmp;
        
        // 计算谐波子组值（保留供以后使用）
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
///    2路谐波计算算法
/// @note
///    参数中的采样点数是采样值中的数据个数，如果采样点数少于计算所需数据个数，将
///    返回错误，如果采样点数多余计算所需数据个数，则以计算所需的数据个数为准，比
///    如传一个3000点的数组用HARM_2560计算类型计算谐波，将只取数组中前2560个数据
///    来计算，该算法充分利用了复数FFT的计算，并将2路结果通过公式：
///    1路fft结果 = （1路+2路共轭复数）/2,2路fft结果=（1路-2路共轭复数）*(-j)/2来
///    来将fft结果分离
/// @param[in] 
///    harmType             计算类型
///    nSampleSize          采样点数
///    pSampleValues        1路采样值数组
///    pSampleValues1       2路采样值数组
///    nHarmSize            输出的谐波次数
/// @param[out]
///    pDCValue             1路直流值
///    pDCValue1            2路直流值
///    pHarmContents        1路HARM次谐波有效值
///    pHarmContents1       2路HARM次谐波有效值
///    pHarmPhaseAngles     1路HARM次谐波相位角（正弦）
///    pHarmPhaseAngles1    2路HARM次谐波相位角（正弦）
/// @return
///    int                  0：成功
///                        -1：输入的采样点数不够
///                        -2：输出的谐波次数大于所能计算的次数
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
    // 参数检查
    if(nSampleSize < nHarmSampCount )
    {
        return -1;
    }
    if(nHarmSize > nHarmCount)    
    {
        return -2;
    }
   
    permute(pHarmTab, nHarmSampCount, pSampleValues, pSampleValues1, yRe, yIm);               /*  排序  */

    //  1个256次，取数，FFT_10计算。
    twiddleTransf(  1,  nHarmFactors[0], nHarmFactors[1]*nHarmFactors[2], yRe, yIm);
    //  10个 16次，取数，FFT_16计算。
    twiddleTransf(nHarmFactors[0], nHarmFactors[1], nHarmFactors[2], yRe, yIm);
    // 160个 1次，取数，FFT_16计算。
    twiddleTransf( nHarmFactors[1]*nHarmFactors[0], nHarmFactors[2], 1, yRe, yIm);

    // 计算直流值
    fTmp[0] = mpysp_i(fabs(yRe[0]),(1.0/nHarmSampCount));       // 1/N10
    *pDCValue=fTmp[0];
    fTmp[1] = mpysp_i(fabs(yIm[0]),(1.0/nHarmSampCount));       // 1/N10
    *pDCValue1=fTmp[1];
    
    // 计算谐波有效值和相位角
    for(i=0;i<nHarmSize;i++)
    {
        j = i*10+10;

        // 取第1路j-1,j,j+1次谐波值
        fAnalogue[0][0] = addsp_i(yRe[j-1],yRe[nHarmSampCount-(j-1)]);
        fAnalogue[0][1] = subsp_i(yIm[j-1],yIm[nHarmSampCount-(j-1)]);
        fAnalogue[0][2] = addsp_i(yRe[j],yRe[nHarmSampCount-j]);
        fAnalogue[0][3] = subsp_i(yIm[j],yIm[nHarmSampCount-j]);
        fAnalogue[0][4] = addsp_i(yRe[j+1],yRe[nHarmSampCount-(j+1)]);
        fAnalogue[0][5] = subsp_i(yIm[j+1],yIm[nHarmSampCount-(j+1)]);

        // 取第2路j-1,j,j+1次谐波值
        fAnalogue[1][0] = addsp_i(yIm[j-1],yIm[nHarmSampCount-(j-1)]);
        fAnalogue[1][1] = subsp_i(yRe[nHarmSampCount-(j-1)],yRe[j-1]);
        fAnalogue[1][2] = addsp_i(yIm[j],yIm[nHarmSampCount-j]);
        fAnalogue[1][3] = subsp_i(yRe[nHarmSampCount-j],yRe[j]);
        fAnalogue[1][4] = addsp_i(yIm[j+1],yIm[nHarmSampCount-(j+1)]);
        fAnalogue[1][5] = subsp_i(yRe[nHarmSampCount-(j+1)],yRe[j+1]);

        // 计算谐波有效值和相位角
        for(k=0; k<2; k++)
        {		
            fTmp1[k] = addsp_i(mpysp_i(fAnalogue[k][0],fAnalogue[k][0]) , 
                mpysp_i(fAnalogue[k][1],fAnalogue[k][1]));
            fTmp2[k] = addsp_i(mpysp_i(fAnalogue[k][2],fAnalogue[k][2]) , 
                mpysp_i(fAnalogue[k][3],fAnalogue[k][3]));
            fTmp3[k] = addsp_i(mpysp_i(fAnalogue[k][4],fAnalogue[k][4]) , 
                mpysp_i(fAnalogue[k][5],fAnalogue[k][5]));

            // 计算第j次谐波的相位角
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

            // 相位角调整
            fAngle[k] += PI0_5;
            while(fAngle[k]>PI2)fAngle[k] = subsp_i(fAngle[k],PI2);
            while(fAngle[k]<0.0)fAngle[k] = addsp_i(fAngle[k],PI2);

            // 弧度->角度
            fAngle[k] = mpysp_i(fAngle[k],ARC2ANGLE_FACTOR);     // 180/PI

            // 与基波求差值?
            /*if(i==0)
            {
                fPhaseAngle[0] = fAngle;
                pHarmPhaseAngle[i] = 0;
            }
            pHarmPhaseAngle[i] = subsp_i(fAngle,fPhaseAngle[0]);*/
            pHarmPhaseAngles2[k][i] = fAngle[k];

            // 计算谐波有效值
            fTmp[k] = sqrtsp_i(fTmp2[k]);
            fTmp[k] = mpysp_i(fTmp[k],(1.0/nHarmSampCount));       // 1/N10
            fTmp[k] = mpysp_i(fTmp[k],EFF_VALUE_FACTOR);  // 1/sqrt(2)
            pHarmContents2[k][i] = fTmp[k];
            
            // 计算谐波子组值（保留供以后使用）
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
///    谐波含有率计算算法
/// @param[in] 
///    harmType             计算类型
///    nHarmSize            谐波次数
///    pHarmContents        nHarmSize次谐波有效值
///    nType                1：电压，2：电流
/// @param[out]
///    pTHDValue            nHarmSize次谐波含有率
/// @return
///    int                  0：成功
///                        -1：谐波次数大于所能计算的最大次数
///
int harmSumTHDCalc(HARM_TYPE harmType, Uint16 nHarmSize, 
               const float pHarmContents[], Int16 nType, float *pTHDValue)
{
    float TDHSum = 0;
    Int i=0;
    float baseHarmContentValue = pHarmContents[0];
    Uint16 nHarmCount = harmCount[harmType];

    // 参数检查
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
///    谐波含有率计算算法
/// @param[in]
///    harmType             计算类型
///    nHarmSize            谐波次数
///    pHarmContents        nHarmSize次谐波有效值
///    nType                1：电压，2：电流
/// @param[out]
///    pTHDValues           nHarmSize次谐波含有率
/// @return
///    int                  0：成功
///                        -1：谐波次数大于所能计算的最大次数
///
int harmTHDCalc(HARM_TYPE harmType, Uint16 nHarmSize, const float pHarmContents[],
                Int16 nType, float pTHDValues[])
{
    Uint16 i=0;
    float baseHarmContentValue = pHarmContents[0];    
    Uint16 nHarmCount = harmCount[harmType];

    // 参数检查   
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
///    直流含有率计算算法
/// @param[in]
///    fDCValue             直流大小
///    fBaseHarmContent     基波有效值
/// @param[out]
///    pTHDValue            直流含有率
/// @return
///    int                  0：成功
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
