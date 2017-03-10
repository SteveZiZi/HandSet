extern "C"
{
#include <stdlib.h>

#include "calc.h"
#include "dspCalc.h"
};

#include <windows.h>

#include "gtest/gtest.h"
#include "TestMacro.h"

/// ���ø��㾯�棬���ڸ����͵ĳ��������ֻ�и������ֱ�����Ϊdouble���ͣ�������
/// ��ʼ����������ʱ����Ҫ��ÿһ�����㳣������f��׺����������������matlabģ��
#pragma warning(disable:4305)
#pragma warning(disable:4244)

#define SIGNAL_FREQ              50              ///< �źŻ���Ƶ��

#define SAMPLE_NUM_PER_PERIOD1   80              ///< ÿ���ڲ�������
#define SAMPLE_PERIOD_NUM1       10              ///< ����������
///< �ܲ���������50*10��
#define SAMPLE_NUM1              (SAMPLE_NUM_PER_PERIOD1*SAMPLE_PERIOD_NUM1)
///< ����Ƶ�ʣ�80*50��
#define SAMPLE_FREQ1             (SAMPLE_NUM_PER_PERIOD1*SIGNAL_FREQ)


#define SQRT2                    1.41421356237309504880
    

#define HARM_DC_PRECISION        5e-3           ///< г������ֱ����������
///< г������г����Чֵ����λ�Ǿ���
#define HARM_PRECISION           5e-3           
#define PHASECHK_PRECISION       1e-4           ///< ������������㾫��
#define PHASESEQ_PRECISION       1e-5           ///< �������㾫��
#define POWER_PRECISION          1e-3           ///< ���ʼ��㾫��
#define SAMPLE_CREATE_PRECISION  1e-4           ///< ���Ҳ����ɼ��㾫��
#define FREQ_PRECISION           1e-4           ///< �ź�Ƶ�ʼ��㾫�ȣ�����ֵԽ��Խ��
#if 0
/**
 *  40Hz-70Hz��2�װ�����˹��ͨ�˲�����������2ά��ʾ�����ʣ�4000Hz,5000Hz,8000Hz,10000Hz,12800Hz��
 */
static float a2_40_70Hz[5][5]={
{1.0000000000000000, -3.9197982160636045, 5.7756167061962405, -3.7913012649742948, 0.9355289049791802},
{1.0000000000000000, -3.9379743972595675, 5.8242790308669683, -3.8343673131140292, 0.9480817061067395},
{1.0000000000000000, -3.9632561319188047, 5.8937992239074362, -3.8977675868501578, 0.9672274281518589},
{1.0000000000000000, -3.9711483282387512, 5.9160218097821948, -3.9185671479490636, 0.9736948719763124},
{1.0000000000000000, -3.9778324437423040, 5.9350685930193521, -3.9366250488624668, 0.9793893500287969}
};
static float b2_40_70Hz[5][5]={
{0.0005371697747744, -0.0000000000000000, -0.0010743395495488, -0.0000000000000000, 0.0005371697747744},
{0.0003460413376464, -0.0000000000000000, -0.0006920826752927, -0.0000000000000000, 0.0003460413376464},
{0.0001365107218804, -0.0000000000000000, -0.0002730214437608, -0.0000000000000000, 0.0001365107218804},
{0.0000876555487554, -0.0000000000000000, -0.0001753110975108, -0.0000000000000000, 0.0000876555487554},
{0.0000536556798714, -0.0000000000000000, -0.0001073113597428, -0.0000000000000000, 0.0000536556798714}
};
#endif
///
/// @brief
///    ������ɢ�ȵ�Ԫ����
/// @return
///    void
///
TEST(ATM_TEST, DISP_RATE)
{
    int32_t timeDifs1[80]={0};
    int32_t timeDifs2[80]={
        250+0,250+0,250+0,250+0,250+0,250+0,250+0,250+0,250+0,250+0,
        250+1,250+1,250+1,250+1,250+1,250+1,250+1,
        250+7,250+7,250+7,250+7,250+7,250+7,250+7,
        250+39,250+39,250+39,250+39,250+39,250+39,250+39,
        250+200,250+200,250+200,250+200,250+200,250+200,250+200,
        250+300,250+300,250+300,250+300,250+300,250+300,250+300,
        250-1,250-1,250-1,250-1,250-1,250-1,250-1,
        250-7,250-7,250-7,250-7,250-7,250-7,250-7,
        250-39,250-39,250-39,250-39,250-39,250-39,250-39,
        250-200,250-200,250-200,250-200,250-200,250-200,250-200,
        250-300,250-300,250-300,250-300,250-300,250-300,250-300
    };

    float dispRate[11];
    float sum=0;

    // ��������ʱ����ɢ�ȣ�ȫΪĬ��ֵ��
    for(int i=0; i<80; i++)
    {
        timeDifs1[i] = 250;
    }
    resetDisp();
    dispHashCreate(1);
    for(int i=0; i<10; i++)
    {
        dispRateCalc(timeDifs1,80,4000,1,dispRate);
    }
    AST_F(dispRate[5],100);
    AST_F(dispRate[0],0);

    // ������ɢ��֮���Ƿ����100
    for(int i=0; i<11; i++)
    {
        sum+=dispRate[i];
    }
    AST_F(sum,100);

    // ���Լ���һ��ʱ�����ɢ��
    resetDisp();
    for(int i=0; i<80; i++)
    {
        timeDifs1[i] = 249;
    }
    for(int i=0; i<10; i++)
    {
        dispRateCalc(timeDifs1,80,4000,1,dispRate);
    }
    AST_F(dispRate[4],100);
    AST_F(dispRate[0],0);


    // ���Լ���һ��ʱ��Σ�-2~-9������ɢ��
    for(int i=0; i<80; i++)
    {
        timeDifs1[i] = 250-2-rand()%8;
    }
    resetDisp();
    for(int i=0; i<10; i++)
    {
        dispRateCalc(timeDifs1,80,4000,1,dispRate);
    }
    AST_F(dispRate[3],100);
    AST_F(dispRate[0],0);

    // ������ɢ��֮���Ƿ����100
    sum=0;
    for(int i=0; i<11; i++)
    {
        sum+=dispRate[i];
    }
    AST_F(sum,100);

    // �����ض�����µ���ɢ��(0��10������������7��)
    resetDisp();
    for(int i=0; i<1000; i++)
    {
        dispRateCalc(timeDifs2,80,4000,1,dispRate);
    }

    AST_F(dispRate[0],7.0/80*100);
    AST_F(dispRate[3],7.0/80*100);
    AST_F(dispRate[5],12.5);
    AST_F(dispRate[7],7.0/80*100);
    AST_F(dispRate[10],7.0/80*100);

    // ������ɢ��֮���Ƿ����100
    sum=0;
    for(int i=0; i<11; i++)
    {
        sum+=dispRate[i];
    }
    AST_F(sum,100);

    // ����ASDUΪ2�����
    dispHashCreate(2);

    // ��������ʱ����ɢ�ȣ�ȫΪĬ��ֵ��
    for(int i=0; i<80; i++)
    {
        timeDifs1[i] = 500;
    }
    resetDisp();    
    for(int i=0; i<10; i++)
    {
        dispRateCalc(timeDifs1,80,4000,2,dispRate);
    }
    AST_F(dispRate[5],100);
    AST_F(dispRate[0],0);

    // ������ɢ��֮���Ƿ����100
    sum=0;
    for(int i=0; i<11; i++)
    {
        sum+=dispRate[i];
    }
    AST_F(sum,100);

    // ���Լ���һ��ʱ�����ɢ��
    resetDisp();
    for(int i=0; i<80; i++)
    {
        timeDifs1[i] = 499;
    }
    for(int i=0; i<10; i++)
    {
        dispRateCalc(timeDifs1,80,4000,2,dispRate);
    }
    AST_F(dispRate[4],100);
    AST_F(dispRate[0],0);


    // ���Լ���һ��ʱ��Σ�-2~-9������ɢ��
    for(int i=0; i<80; i++)
    {
        timeDifs1[i] = 500-2-rand()%8;
    }
    resetDisp();
    for(int i=0; i<10; i++)
    {
        dispRateCalc(timeDifs1,80,4000,2,dispRate);
    }
    AST_F(dispRate[3],100);
    AST_F(dispRate[0],0);

    // ������ɢ��֮���Ƿ����100
    sum=0;
    for(int i=0; i<11; i++)
    {
        sum+=dispRate[i];
    }
    AST_F(sum,100);

    // ���Լ���һ��ʱ��Σ�-50*ASDU����+1 ~ -10������ɢ��
    for(int i=0; i<80; i++)
    {
        timeDifs1[i] = 500-10-rand()%90;
    }
    resetDisp();
    for(int i=0; i<10; i++)
    {
        dispRateCalc(timeDifs1,80,4000,2,dispRate);
    }
    AST_F(dispRate[2],100);
    AST_F(dispRate[0],0);

    // ������ɢ��֮���Ƿ����100
    sum=0;
    for(int i=0; i<11; i++)
    {
        sum+=dispRate[i];
    }
    AST_F(sum,100);

    // ���Լ���һ��ʱ��Σ�50*ASDU���� ~ 250*ASDU����-1������ɢ��
    for(int i=0; i<80; i++)
    {
        timeDifs1[i] = 500+50*2+rand()%200;
    }
    resetDisp();
    for(int i=0; i<10; i++)
    {
        dispRateCalc(timeDifs1,80,4000,2,dispRate);
    }
    AST_F(dispRate[9],100);
    AST_F(dispRate[0],0);

    // ������ɢ��֮���Ƿ����100
    sum=0;
    for(int i=0; i<11; i++)
    {
        sum+=dispRate[i];
    }
    AST_F(sum,100);
}

///
/// @brief
///    ��Чֵ��Ԫ����
/// @return
///    void
///
TEST(ATM_TEST, EFFECTIVE_VALUE)
{
    float samples[SAMPLE_NUM1];
    float samples1[40]={
        33.2,32,25,8,5.1,51,5,44,5,15,
        45,52.54,51,5.51,54.15,45.4,0,87,4,54,
        1,24,57.5,5,1,54,5,15,4.5,18.7,
        8,98.1,6,1,4.32,34,6,1,23,54};
        float effValue;
        int i=0;
        int j=0;

        // ����ֱ����Чֵ��
        for(i=0; i<SAMPLE_NUM1; i++)
        {
            samples[i]=100;
        }
        effValueCalc(samples,SAMPLE_NUM1, &effValue);
        AST_F(effValue,100);

        // �������Ҳ���Чֵ���㣺
        // ����800�����������ݣ�ÿ����80�������㣬�ظ�10�����ڣ�50*sin(2*50*pi*t)
        for(i=0; i<SAMPLE_PERIOD_NUM1; i++)
        {
            for(j=0; j<SAMPLE_NUM_PER_PERIOD1; j++)
            {
                samples[i*SAMPLE_NUM_PER_PERIOD1+j]=static_cast<float>(
                    50*sin(2*50*PI*j*0.02/SAMPLE_NUM_PER_PERIOD1));
            }
        }

        // ������Чֵ����֤�Ƿ����50/sqrt(2)
        effValueCalc(samples,SAMPLE_NUM1, &effValue);
        AST_F(effValue,50/SQRT2);

        // �����ض�ֵ����Чֵ���㣨��Matlab��������֤��
        effValueCalc(samples1,40, &effValue);
        AST_F(effValue,3.619078149197666e+001f);
}

// г����Ԫ����
TEST(ATM_TEST, HARMONIC)
{
    enum{
        SAMPLE_NUM_PER_PERIOD_256 = 256,
        SAMPLE_NUM_PER_PERIOD_200 = 200,
        SAMPLE_NUM_PER_PERIOD_160 = 160,
        SAMPLE_NUM_PER_PERIOD_100 = 100,
        SAMPLE_NUM_PER_PERIOD_80  = 80
    };
    float samples[SAMPLE_NUM_PER_PERIOD_256*10];
    float dcValue = 0;
    float harmValue[SAMPLE_NUM_PER_PERIOD_256/2];
    float harmAngle[SAMPLE_NUM_PER_PERIOD_256/2];
    int i=0;
    int j=0;

    // ����2560�����������ݣ�ÿ����256�������㣬�ظ�10�����ڣ�����Ϊ��
    // y=100+50*sin(2*50*pi*t-2.0/3*PI)+30*sin(2*100*pi*t+PI/4)
    for(i=0; i<10; i++)
    {
        for(j=0; j<SAMPLE_NUM_PER_PERIOD_256; j++)
        {
            samples[i*SAMPLE_NUM_PER_PERIOD_256+j]=(float)(100
                +20*sin(2*SIGNAL_FREQ*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_256-2.0/3*PI)
                +70*sin(2*(SIGNAL_FREQ*2)*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_256+PI/4));
        }
    }

    // ����г��
    harmCalc(HARM_2560,SAMPLE_NUM_PER_PERIOD_256*10, samples, SAMPLE_NUM_PER_PERIOD_256/2-1, &dcValue,
        harmValue, harmAngle);

    // ��ֱ֤�����
    AST_F_EX(dcValue,100,HARM_DC_PRECISION);

    // ��ӡֱ��
    /*printf("DCValue:%f\n",dcValue);

    //��ӡ����г����Чֵ����λ��
    for(i=0; i<127; i++)
    {
        printf("%d:\t%f\t%f\n",i+1,harmValue[i],harmAngle[i]);
    }*/

    // ��֤������2��г����Чֵ����λ��
    AST_F_EX(harmValue[0],20.0/SQRT2,HARM_PRECISION);
    // -120��->240��
    AST_F_EX(harmAngle[0],240,HARM_PRECISION);
    AST_F_EX(harmValue[1],70.0/SQRT2,HARM_PRECISION);
    // 45��
    AST_F_EX(harmAngle[1],45,HARM_PRECISION);

    // ����2000�����������ݣ�ÿ����200�������㣬�ظ�10�����ڣ�����Ϊ��
    // y=100+50*sin(2*50*pi*t-2.0/3*PI)+30*sin(2*100*pi*t+PI/4)
    for(i=0; i<10; i++)
    {
        for(j=0; j<SAMPLE_NUM_PER_PERIOD_200; j++)
        {
            samples[i*SAMPLE_NUM_PER_PERIOD_200+j]=(float)(100
                +20*sin(2*SIGNAL_FREQ*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_200-2.0/3*PI)
                +70*sin(2*(SIGNAL_FREQ*2)*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_200+PI/4));
        }
    }

    // ����г��
    harmCalc(HARM_2000,SAMPLE_NUM_PER_PERIOD_200*10, samples,SAMPLE_NUM_PER_PERIOD_200/2-1, &dcValue,
        harmValue, harmAngle);

    // ��ֱ֤�����
    AST_F_EX(dcValue,100,HARM_DC_PRECISION);

    // ��֤������2��г����Чֵ����λ��
    AST_F_EX(harmValue[0],20/SQRT2,HARM_PRECISION);
    // -120��->240��
    AST_F_EX(harmAngle[0],240,HARM_PRECISION);
    AST_F_EX(harmValue[1],70/SQRT2,HARM_PRECISION);
    // 45��
    AST_F_EX(harmAngle[1],45,HARM_PRECISION);
   

    // ����1600�����������ݣ�ÿ����160�������㣬�ظ�10�����ڣ�����Ϊ��
    // y=100+50*sin(2*50*pi*t-2.0/3*PI)+30*sin(2*100*pi*t+PI/4)
    for(i=0; i<10; i++)
    {
        for(j=0; j<SAMPLE_NUM_PER_PERIOD_160; j++)
        {
            samples[i*SAMPLE_NUM_PER_PERIOD_160+j]=(float)(100
                +20*sin(2*SIGNAL_FREQ*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_160-2.0/3*PI)
                +70*sin(2*(SIGNAL_FREQ*2)*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_160+PI/4));
        }
    }

    // ����г��
    harmCalc(HARM_1600,SAMPLE_NUM_PER_PERIOD_160*10, samples,SAMPLE_NUM_PER_PERIOD_160/2-1, &dcValue,
        harmValue, harmAngle);

    // ��ֱ֤�����
    AST_F_EX(dcValue,100,HARM_DC_PRECISION);

    // ��֤������2��г����Чֵ����λ��
    AST_F_EX(harmValue[0],20/SQRT2,HARM_PRECISION);
    // -120��->240��
    AST_F_EX(harmAngle[0],240,HARM_PRECISION);
    AST_F_EX(harmValue[1],70/SQRT2,HARM_PRECISION);
    // 45��
    AST_F_EX(harmAngle[1],45,HARM_PRECISION);

    // ����1000�����������ݣ�ÿ����160�������㣬�ظ�10�����ڣ�����Ϊ��
    // y=100+50*sin(2*50*pi*t-2.0/3*PI)+30*sin(2*100*pi*t+PI/4)
    for(i=0; i<10; i++)
    {
        for(j=0; j<SAMPLE_NUM_PER_PERIOD_100; j++)
        {
            samples[i*SAMPLE_NUM_PER_PERIOD_100+j]=(float)(100
                +20*sin(2*SIGNAL_FREQ*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_100-2.0/3*PI)
                +70*sin(2*(SIGNAL_FREQ*2)*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_100+PI/4));
        }
    }

    // ����г��
    harmCalc(HARM_1000,SAMPLE_NUM_PER_PERIOD_100*10, samples,SAMPLE_NUM_PER_PERIOD_100/2-1, &dcValue,
        harmValue, harmAngle);

    // ��ֱ֤�����
    AST_F_EX(dcValue,100,HARM_DC_PRECISION);

    // ��֤������2��г����Чֵ����λ��
    AST_F_EX(harmValue[0],20/SQRT2,HARM_PRECISION);
    // -120��->240��
    AST_F_EX(harmAngle[0],240,HARM_PRECISION);
    AST_F_EX(harmValue[1],70/SQRT2,HARM_PRECISION);
    // 45��
    AST_F_EX(harmAngle[1],45,HARM_PRECISION);

    // ����800�����������ݣ�ÿ����160�������㣬�ظ�10�����ڣ�����Ϊ��
    // y=100+50*sin(2*50*pi*t-2.0/3*PI)+30*sin(2*100*pi*t+PI/4)
    for(i=0; i<10; i++)
    {
        for(j=0; j<SAMPLE_NUM_PER_PERIOD_80; j++)
        {
            samples[i*SAMPLE_NUM_PER_PERIOD_80+j]=(float)(100
                +20*sin(2*SIGNAL_FREQ*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_80-2.0/3*PI)
                +70*sin(2*(SIGNAL_FREQ*2)*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_80+PI/4));
        }
    }

    // ����г��
    harmCalc(HARM_800,SAMPLE_NUM_PER_PERIOD_80*10, samples,
        SAMPLE_NUM_PER_PERIOD_80/2-1, &dcValue,
        harmValue, harmAngle);

    // ��ֱ֤�����
    AST_F_EX(dcValue,100,HARM_DC_PRECISION);

    // ��֤������2��г����Чֵ����λ��
    AST_F_EX(harmValue[0],20/SQRT2,HARM_PRECISION);
    // -120��->240��
    AST_F_EX(harmAngle[0],240,HARM_PRECISION);
    AST_F_EX(harmValue[1],70/SQRT2,HARM_PRECISION);
    // 45��
    AST_F_EX(harmAngle[1],45,HARM_PRECISION);

    float dcValue2[2] = {0};
    float samples2[2][SAMPLE_NUM_PER_PERIOD_256*10] = {0};        
    float harmValue2[2][SAMPLE_NUM_PER_PERIOD_256/2]={0};
    float harmAngle2[2][SAMPLE_NUM_PER_PERIOD_256/2]={0};

    // ����2��800�����������ݣ�ÿ����160�������㣬�ظ�10�����ڣ�����Ϊ��
    // y1=100+20*sin(2*50*pi*t-2.0/3*PI)+70*sin(2*100*pi*t+PI/4)
    // y2=50+100*sin(2*50*pi*t+1.0/4*PI)+30*sin(2*100*pi*t+PI/6)
    for(i=0; i<10; i++)
    {
        for(j=0; j<SAMPLE_NUM_PER_PERIOD_80; j++)
        {
            samples2[0][i*SAMPLE_NUM_PER_PERIOD_80+j]=(float)(100
                +20*sin(2*SIGNAL_FREQ*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_80-2.0/3*PI)
                +70*sin(2*(SIGNAL_FREQ*2)*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_80+PI/4));
            samples2[1][i*SAMPLE_NUM_PER_PERIOD_80+j]=(float)(50
                +100*sin(2*SIGNAL_FREQ*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_80+1.0/4*PI)
                +30*sin(2*(SIGNAL_FREQ*2)*PI*j*0.02/SAMPLE_NUM_PER_PERIOD_80+PI/6));
        }
    }

    // ����˫ͨ���㷨���������FFT���鲿һ�μ�������FFT
    harmCalc2(HARM_800,SAMPLE_NUM_PER_PERIOD_80*10, samples2[0], samples2[1],
        SAMPLE_NUM_PER_PERIOD_80/2-1, &dcValue2[0], &dcValue2[1], harmValue2[0],
        harmValue2[1],harmAngle2[0], harmAngle2[1]);

    // ��ֱ֤�����
    AST_F_EX(dcValue2[0],100,HARM_DC_PRECISION);

    // ��֤������2��г����Чֵ����λ��
    AST_F_EX(harmValue2[0][0],20/SQRT2,HARM_PRECISION);
    // -120��->240��
    AST_F_EX(harmAngle2[0][0],240,HARM_PRECISION);
    AST_F_EX(harmValue2[0][1],70/SQRT2,HARM_PRECISION);
    // 45��
    AST_F_EX(harmAngle2[0][1],45,HARM_PRECISION);

    // ��ֱ֤�����
    //AST_F_EX(dcValue2[1],50,HARM_DC_PRECISION);

    // ��֤������2��г����Чֵ����λ��
    AST_F_EX(harmValue2[1][0],100/SQRT2,HARM_PRECISION);
    // -120��->240��
    AST_F_EX(harmAngle2[1][0],45,HARM_PRECISION);
    AST_F_EX(harmValue2[1][1],30/SQRT2,HARM_PRECISION);
    // 45��
    AST_F_EX(harmAngle2[1][1],30,HARM_PRECISION);

#if 0
    float Z1[20], Z2[20] = {0};
    float ZC[20], ZA[20];
    for(i=0; i<20; i++)
    {
        Z1[i]=(float)(100
            +20*sin(2*SIGNAL_FREQ*PI*i*0.02/20-2.0/3*PI)
            +70*sin(2*(SIGNAL_FREQ*2)*PI*i*0.02/20+PI/4));
    }

    fft_20(Z1,Z2);
    //��ӡ����FFT��Чֵ����λ��
    ZC[0] = sqrt(Z1[0]*Z1[0]+Z2[0]*Z2[0]);
    ZC[0] = ZC[0] / 20;
    ZA[0] = 0;
    printf("%d:\t%f\t%f\n",0,ZC[0],ZA[0]);
    for(i=1; i<20; i++)
    {
        ZC[i] = sqrt(Z1[i]*Z1[i]+Z2[i]*Z2[i]);
        ZC[i] = ZC[i]/10;
        ZA[i] = atan(Z2[i]/Z1[i])/PI*180;
        printf("%d:\t%f\t%f\n",i,ZC[i],ZA[i]);
    }
#endif
}

///
/// @brief
///    ������������㵥Ԫ����
/// @return
///    void
///
TEST(ATM_TEST, PHASE_CHECK)
{
    // ���Ժ������������
    float dValue,dAngle;

    // 100��0��-100��30��= 5.176380902050414e+001��-75�㣨Matlab��������
    phaseCheckVectorCalc(100,0,100,30,&dValue,&dAngle);
    AST_F_EX(dValue,5.176380902050414e+001,PHASECHK_PRECISION);
    AST_F_EX(dAngle,-75,PHASECHK_PRECISION);

    // 150��60��-60��-30��=1.615549442140351e+002��8.180140948635182e+001��
    phaseCheckVectorCalc(150,60,60,-30,&dValue,&dAngle);
    AST_F_EX(dValue,1.615549442140351e+002,PHASECHK_PRECISION);
    AST_F_EX(dAngle,8.180140948635182e+001,PHASECHK_PRECISION);

    phaseCheckVectorCalc(31.23, 13.95, 35.1, -123.4, &dValue, &dAngle);
    AST_F_EX(dValue,61.8045856128567,PHASECHK_PRECISION);
    AST_F_EX(dAngle,36.5797979100301,PHASECHK_PRECISION);

    // �߽����
    phaseCheckVectorCalc(0,0,0,30,&dValue,&dAngle);
    AST_F_EX(dValue,0,PHASECHK_PRECISION);
    AST_F_EX(dAngle,0,PHASECHK_PRECISION);

    // ��������
    phaseCheckVectorCalc(-50,0,-70,30,&dValue,&dAngle);
    AST_F_EX(dValue,0,PHASECHK_PRECISION);
    AST_F_EX(dAngle,0,PHASECHK_PRECISION);
}

///
/// @brief
///    �������㵥Ԫ����
/// @return
///    void
///
TEST(ATM_TEST, PHASE_SEQ)
{
    float pHarmContent[3]       =   {50,150,30};
    float pHarmAngle[3]         =   {30,-60,90}; 

    float pHarmContent1[3]       =   {100,100,100};
    float pHarmAngle1[3]         =   {0,120,240}; 

    float pPhaseSeqContent[3];
    float pPhaseSeqAngle[3];
    float fUnbalanceRate;

    // ��֤50��30�㣬150��-60�㣬30��90���������������͸������
    phaseSeqCalc(pHarmContent,pHarmAngle,pPhaseSeqContent,pPhaseSeqAngle, 
        &fUnbalanceRate);

    // ��֤����������Matlab��������
    AST_F_EX(pPhaseSeqContent[0],4.667353683469902e+001, PHASESEQ_PRECISION);
    AST_F_EX(pPhaseSeqAngle[0],360-3.234041646081504e+001, PHASESEQ_PRECISION);

    // ��֤����������Matlab��������
    AST_F_EX(pPhaseSeqContent[1],6.699119432745253e+001, PHASESEQ_PRECISION);
    AST_F_EX(pPhaseSeqAngle[1],4.411736003201621e+001, PHASESEQ_PRECISION);

    // ��֤����������Matlab��������
    AST_F_EX(pPhaseSeqContent[2],4.435193541723763e+001, PHASESEQ_PRECISION);
    AST_F_EX(pPhaseSeqAngle[2],1.756897892848149e+002, PHASESEQ_PRECISION);

    // ��֤���಻ƽ���
    AST_F_EX(fUnbalanceRate,66.20561980197822, PHASESEQ_PRECISION);

    phaseSeqCalc(pHarmContent1,pHarmAngle1,pPhaseSeqContent,pPhaseSeqAngle, 
        &fUnbalanceRate);

    // ��֤����������Matlab��������
    AST_F_EX(pPhaseSeqContent[0],0, PHASESEQ_PRECISION);
    AST_F_EX(pPhaseSeqAngle[0],0, PHASESEQ_PRECISION);

    // ��֤����������Matlab��������
    AST_F_EX(pPhaseSeqContent[1],0, PHASESEQ_PRECISION);
    AST_F_EX(pPhaseSeqAngle[1],0, PHASESEQ_PRECISION);    

    // ��֤����������Matlab��������
    AST_F_EX(pPhaseSeqContent[2],100, PHASESEQ_PRECISION);
    AST_F_EX(pPhaseSeqAngle[2],0, PHASESEQ_PRECISION);

    // ��֤���಻ƽ���
    AST_F_EX(fUnbalanceRate,0, PHASESEQ_PRECISION);
}
///
/// @brief
///    ���Լ��㵥Ԫ����
/// @return
///    void
///
TEST(ATM_TEST, POLAR)
{
    int polar;

    // ����������
    polar = polarCalc(50,2,4);
    ASSERT_TRUE(polar==1);

    // ����0����
    polar = polarCalc(0,3,4);
    ASSERT_TRUE(polar==0);

    // ����С���ż�ֵ��Ư��ֵ�ļ���
    polar = polarCalc(3,5,2);
    ASSERT_TRUE(polar==0);

    // ����С���ż�ֵ����Ư��ֵ�ü���
    polar = polarCalc(3,5,2);
    ASSERT_TRUE(polar==0);

    // ���Ը�����
    polar = polarCalc(-53,3,0);
    ASSERT_TRUE(polar==-1);
}

///
/// @brief
///    ���ʼ��㵥Ԫ����
/// @return
///    void
///
TEST(ATM_TEST,POWER)
{
    float pEffValue[2]={50.17,5.21};
    float pPhase[2]={-20,60};
    float pPower[3];
    float pPowerFactors[3];

    // ���Թ��ʼ���
    powerCalc(pEffValue, pPhase, pPower, pPowerFactors);

    // ���ڹ���
    AST_F_EX(pPower[0],261.3857,POWER_PRECISION);

    // �й�����
    AST_F_EX(pPower[1],45.389150473194956085850146689654,POWER_PRECISION);

    // �޹�����
    AST_F_EX(pPower[2],-257.41466388652311214321768220245,POWER_PRECISION);
}

///
/// @brief
///    �������ɵ�Ԫ����
/// @return
///    void
///
TEST(ATM_TEST, SAMPLE_CREATE)
{
    float samples[80];

    ///< ���Ҳ���sin(2*pi*50*t+60��)��Matlab������
    float sinResults[80]={17.320508,18.051706,18.671609,19.176395,19.562952,
        19.828897,19.972591,19.993146,19.890438,19.665098,19.318517,18.852830,
        18.270909,17.576342,16.773411,15.867067,14.862897,13.767092,12.586408,
        11.328125,10.000000,8.610222,7.167359,5.680307,4.158234,2.610524,
        1.046719,-0.523539,-2.090569,-3.644711,-5.176381,-6.676137,-8.134733,
        -9.543175,-10.892781,-12.175229,-13.382612,-14.507487,-15.542919,
        -16.482524,-17.320508,-18.051706,-18.671609,-19.176395,-19.562952,
        -19.828897,-19.972591,-19.993146,-19.890438,-19.665098,-19.318517,
        -18.852830,-18.270909,-17.576342,-16.773411,-15.867067,-14.862897,
        -13.767092,-12.586408,-11.328125,-10.000000,-8.610222,-7.167359,
        -5.680307,-4.158234,-2.610524,-1.046719,0.523539,2.090569,3.644711,
        5.176381,6.676137,8.134733,9.543175,10.892781,12.175229,13.382612,
        14.507487,15.542919,16.482524};

    ///< ���Ҳ�cos(2*pi*50*t+60��)��Matlab������
    float cosResults[80]={10.000000,8.610222,7.167359,5.680307,4.158234,2.610524,
        1.046719,-0.523539,-2.090569,-3.644711,-5.176381,-6.676137,-8.134733,
        -9.543175,-10.892781,-12.175229,-13.382612,-14.507487,-15.542919,
        -16.482524,-17.320508,-18.051706,-18.671609,-19.176395,-19.562952,
        -19.828897,-19.972591,-19.993146,-19.890438,-19.665098,-19.318517,
        -18.852830,-18.270909,-17.576342,-16.773411,-15.867067,-14.862897,
        -13.767092,-12.586408,-11.328125,-10.000000,-8.610222,-7.167359,
        -5.680307,-4.158234,-2.610524,-1.046719,0.523539,2.090569,3.644711,
        5.176381,6.676137,8.134733,9.543175,10.892781,12.175229,13.382612,
        14.507487,15.542919,16.482524,17.320508,18.051706,18.671609,19.176395,
        19.562952,19.828897,19.972591,19.993146,19.890438,19.665098,19.318517,
        18.852830,18.270909,17.576342,16.773411,15.867067,14.862897,13.767092,
        12.586408,11.328125};

    // ��������/������ϲ��θ�������
    // 100+30*sin(2*pi*50*t-45��)+20*sin(2*pi*80*t-60��)+50*sin(2*pi*100*t-30��)
    // +10*sin(2*pi*150*t+65��)+40*sin(2*pi*200*t-80��) 
    // 100+30*cos(2*pi*50*t-45��)+20*cos(2*pi*80*t-60��)+50*cos(2*pi*100*t-30��)
    // +10*cos(2*pi*150*t+65��)+40*cos(2*pi*200*t-80��) 
    float pOmigas[] = {50,80,100,150,200};
    float pInitPhaseAngles[]={-45,-60,-30,65,-80};
    float pWidths[]={30,20,50,10,40};

    // ����������ϲ��ε�Matlab������
    // 100+30*sin(2*pi*50*t-45��)+20*sin(2*pi*80*t-60��)+50*sin(2*pi*100*t-30��)
    // +10*sin(2*pi*150*t+65��)+40*sin(2*pi*200*t-80��) 
    float sinsResults[80]={6.137056,21.148906,39.888978,61.386748,84.425441,
        107.644417,129.656478,149.168882,165.096439,176.655773,183.431613,
        185.408661,182.965915,176.833958,168.019324,157.703281,147.124838,
        137.459352,129.704569,124.585170,122.485149,123.414607,127.014201,
        132.596792,139.222208,145.797771,151.194700,154.368913,154.474299,
        150.957203,143.622699,132.665897,118.664964,102.536215,85.455341,
        68.752128,53.788637,41.832449,33.937061,30.840943,32.894919,40.024909,
        51.733658,67.141369,85.061439,104.104171,122.798662,139.721384,
        153.619381,163.516563,168.793274,169.231924,165.024764,156.743563,
        145.274586,131.725611,117.314331,103.249217,90.614516,80.270559,
        72.778903,68.359327,66.882449,67.898194,70.696773,74.395583,78.042943,
        80.727892,81.684760,80.381764,76.584552,70.388118,62.213768,52.771287,
        42.990023,33.925710,26.652357,22.150061,21.200057,24.297700};

    // ����������ϲ��ε�Matlab������
    // 100+30*cos(2*pi*50*t-45��)+20*cos(2*pi*80*t-60��)+50*cos(2*pi*100*t-30��)
    // +10*cos(2*pi*150*t+65��)+40*cos(2*pi*200*t-80��) 
    float cossResults[80]={185.686583,202.355724,215.595754,224.463929,
        228.326202,226.919994,220.382886,209.244160,194.379732,176.934602,
        158.220099,139.595724,122.346894,107.570323,96.078077,88.329542,
        84.397821,83.973771,86.407189,90.781056,96.011507,100.963681,104.572017,
        105.953150,104.500228,99.949295,92.411087,82.364974,70.615463,58.215360,
        46.362962,36.283207,29.104332,25.742066,26.802716,32.514705,42.695414,
        56.756812,73.749595,92.441885,111.425199,129.237754,144.493536,
        156.004976,162.887701,164.637548,161.172705,152.837148,140.365295,
        124.811454,107.450999,89.662855,72.804593,58.092043,46.494817,38.657455,
        34.853368,34.975497,38.563956,44.867375,52.931328,61.704680,70.152981,
        77.367448,82.658617,85.625355,86.192463,84.613292,81.437318,77.446181,
        73.564844,70.757034,69.915734,71.759962,76.748515,85.019627,96.362933,
        110.226920,125.761480,141.891677};
    int i=0;

    // �������Ҳ�����
    sinSampleCreate(samples,80,4000,50,60,20);
    for(i=0; i<80; i++)
    {
        AST_F_EX(samples[i],sinResults[i],SAMPLE_CREATE_PRECISION);
    }

    // �������Ҳ�����
    cosSampleCreate(samples,80,4000,50,60,20);
    for(i=0; i<80; i++)
    {
        AST_F_EX(samples[i],cosResults[i],SAMPLE_CREATE_PRECISION);
    }

    // ���Ը���������ϲ������ɣ�����Ϊ��
    // 100+30*sin(2*pi*50*t-45��)+20*sin(2*pi*80*t-60��)+50*sin(2*pi*100*t-30��)
    // +10*sin(2*pi*150*t+65��)+40*sin(2*pi*200*t-80��)
    sinSamplesCreate(samples,80,4000,100,pOmigas,pInitPhaseAngles,pWidths,5);
    for(i=0; i<80; i++)
    {
        AST_F_EX(samples[i],sinsResults[i],SAMPLE_CREATE_PRECISION);
    }

    // ���Ը���������ϲ������ɣ�����Ϊ��
    // 100+30*cos(2*pi*50*t-45��)+20*cos(2*pi*80*t-60��)+50*cos(2*pi*100*t-30��)
    // +10*cos(2*pi*150*t+65��)+40*cos(2*pi*200*t-80��) 
    cosSamplesCreate(samples,80,4000,100,pOmigas,pInitPhaseAngles,pWidths,5);
    for(i=0; i<80; i++)
    {
        AST_F_EX(samples[i],cossResults[i],SAMPLE_CREATE_PRECISION);
    }

    pOmigas[0] = 50;
    pInitPhaseAngles[0] = 0;
    pWidths[0] = 0.008*sqrt(3.0f);

    sinSamplesCreate(samples,80,4000,0,pOmigas,pInitPhaseAngles,pWidths,1);
#if 0
    for(int i=0; i<80; i++)
    {
        printf("%d: %f\n", i, samples[i]);
    }
#endif
}
// Ƶ�ʵ�Ԫ����
TEST(ATM_TEST, FREQ)
{
    float samples[800] = {0.0f};
    float filteredSamples[800] = {0.0f};
    int i = 0;
    float freq = 0;
    Int16 reValue;    
    Uint16 validSampleSum = 0;
    float periodSum = 0;
    float difTime = 0;

    // ���Բ���ֵȫΪ0����������������Ƶ��Ϊ0��������Ϊ0����Ч������Ϊ0
    peroidNumCalc(800,4000,samples, 40, 70, &periodSum, &validSampleSum, &difTime);
    ASSERT_EQ(validSampleSum, 0);
    ASSERT_EQ(periodSum,0);
    reValue = freqCalc(validSampleSum,4000, periodSum, difTime, &freq);
    ASSERT_EQ(freq, 0);

    // �����ò�������С����Сֵ����-1
    ASSERT_TRUE(reValue==-1);
    AST_F_EX(freq, 0, FREQ_PRECISION);
    
    // ���Բ���ֵȫΪһ���������������ֱ�������������Ƶ��Ϊ0��������Ϊ0����Ч������Ϊ0
    for(int i=0; i<800; i++)
    {
        samples[i] = 220.0;
    }    
    peroidNumCalc(800,4000,samples, 40, 70, &periodSum, &validSampleSum, &difTime);
    ASSERT_EQ(validSampleSum, 0);
    ASSERT_EQ(periodSum,0);

    reValue = freqCalc(validSampleSum,4000, periodSum, difTime, &freq);    
    ASSERT_EQ(freq, 0);

    // �����ò�������С����Сֵ����-1
    ASSERT_TRUE(reValue==-1);
    AST_F_EX(freq, 0, FREQ_PRECISION);

#if 1
    // ����50Hz�ļ�����
    sinSampleCreate(samples,800,4000,50,60,20);
    /*
    filterx(4, a2_40_70Hz[0], b2_40_70Hz[0],
        800, samples, filteredSamples, 0);*/
    // ���ѭ����ʹ�˲����ȶ�
    // �����˲����ȶ���ʹ����ļ���׼ȷ
    for(int i=0; i<15; i++)
    {    
        iir_6th_filter(samples, a40_70Hz[0], b40_70Hz[0], g40_70Hz[0], &iir_state, 
            800, filteredSamples);
    }
    for(int i=1; i<100; i++)
    {        
        iir_6th_filter(samples, a40_70Hz[0], b40_70Hz[0], g40_70Hz[0], &iir_state, 
            800, filteredSamples);

    
#if 0
    for(int i=0; i<800; i++)
    {
        printf("%f\n", filteredSamples[i]);
    }
#endif
        peroidNumCalc(800,4000,filteredSamples, 40, 70, &periodSum, &validSampleSum, &difTime);    
        reValue = freqCalc(validSampleSum,4000, periodSum, difTime, &freq);
        // ���Է���ֵo
        ASSERT_TRUE(reValue==0);
        EPT_F_EX(freq, 50, FREQ_PRECISION);    
    }

    // ����50Hz������Ϊ0��Ľ��
    // ����50Hz�ļ�����
    float fOmigas[1]={50};
    float fInitPhaseAngles[1] = {0};
    float fWidths[1]={9333.809511662427322091145579784};
    sinSamplesCreate(samples,80,4000,0,fOmigas,fInitPhaseAngles,fWidths,1);
    for(int i=1; i<10; i++)
    {
        for(int j=0; j<80; j++)
        {
            samples[i*80+j]=samples[j];
        }        
    }
    
    /*filterx(4, a2_40_70Hz[0], b2_40_70Hz[0],
        800, samples, filteredSamples, 0);*/

    // �����˲����ȶ���ʹ����ļ���׼ȷ
    for(int i=0; i<15; i++)
    {    
        iir_6th_filter(samples, a40_70Hz[0], b40_70Hz[0], g40_70Hz[0], &iir_state, 
            800, filteredSamples);
    }

    for(int i=0; i<30; i++)
    {    
        iir_6th_filter(samples, a40_70Hz[0], b40_70Hz[0], g40_70Hz[0], &iir_state, 
            800, filteredSamples);    
#if 0
    for(int i=0; i<800; i++)
    {
        printf("%f\n", samples[i]);
    }
#endif
        peroidNumCalc(800,4000,filteredSamples, 40, 70, &periodSum, &validSampleSum, &difTime);    
        reValue = freqCalc(validSampleSum,4000, periodSum, difTime, &freq);
        // ���Է���ֵo
        ASSERT_TRUE(reValue==0);
        EPT_F_EX(freq, 50, FREQ_PRECISION);
    }
    // ����50Hz������Ϊ0��Ľ��
    // ����50Hz��С�����źż�����
    sinSampleCreate(samples,800,4000,50,60,2);
    for(int i=0; i<10; i++)
    {
        for(int j=0; j<80; j++)
        {
            samples[i*80+j]=(int)samples[j];
        }        
    }

    /*filterx(4, a2_40_70Hz[0], b2_40_70Hz[0],
        800, samples, filteredSamples, 0);*/
    // �����˲����ȶ���ʹ����ļ���׼ȷ
    for(int i=0; i<15; i++)
    {    
        iir_6th_filter(samples, a40_70Hz[0], b40_70Hz[0], g40_70Hz[0], &iir_state, 
            800, filteredSamples);
    }

    for(int i=0; i<30; i++)
    {        
        iir_6th_filter(samples, a40_70Hz[0], b40_70Hz[0], g40_70Hz[0], &iir_state, 
            800, filteredSamples);    
#if 0
    for(int i=0; i<800; i++)
    {
        printf("%f\n", samples[i]);
    }
#endif
        peroidNumCalc(800,4000,filteredSamples, 40, 70, &periodSum, &validSampleSum, &difTime);    
        reValue = freqCalc(validSampleSum,4000, periodSum, difTime, &freq);
        // ���Է���ֵo
        ASSERT_TRUE(reValue==0);
        EPT_F_EX(freq, 50, FREQ_PRECISION)<<i<<"test failed";
    }

    // �����ض�����Ľ��
    float samples1[]={ 7.062433 ,8.128169 , 9.143790 , 10.103039 , 10.999998,
        11.829140 , 12.585350 , 13.263966  ,13.860807,  14.372191 , 14.794966 ,
        15.126526 , 15.364822 , 15.508394 , 15.556348 , 15.508394 , 15.364824 ,
        15.126526 , 14.794966 , 14.372192  ,13.860807  ,13.263968  ,12.585350  ,
        11.829141  ,11.000001  ,10.103043  ,9.143793  ,8.128170  ,7.062436  ,
        5.953157  ,4.807175  ,3.631557  ,2.433552  ,1.220538  ,0.000002  ,
        -1.220535  ,-2.433547  ,-3.631555  ,-4.807175  ,-5.953154  ,-7.062432  ,
        -8.128167  ,-9.143790  ,-10.103039  ,-10.999998  ,-11.829137  ,
        -12.585347  ,-13.263966  ,-13.860806  ,-14.372191  ,-14.794966  ,
        -15.126526  ,-15.364822  ,-15.508394  ,-15.556348  ,-15.508394  ,
        -15.364826  ,-15.126527  ,-14.794968  ,-14.372192  ,-13.860809  ,
        -13.263969  ,-12.585352  ,-11.829141  ,-11.000001  ,-10.103041  ,
        -9.143793  ,-8.128169  ,-7.062433  ,-5.953162  ,-4.807182  ,-3.631563  ,
        -2.433553  ,-1.220542  ,0.000000  ,1.220537  ,2.433548  ,3.631557  ,
        4.807175  ,5.953155 };

    for(int i=0; i<10; i++)
    {
        for(int j=0; j<80; j++)
        {
            samples[i*80+j]=samples1[j];
        }
    }

    peroidNumCalc(800,4000,samples, 40, 70, &periodSum, &validSampleSum, &difTime);    
    reValue = freqCalc(validSampleSum,4000, periodSum, difTime, &freq);
    // ���Է���ֵ0
    ASSERT_TRUE(reValue==0);
    AST_F_EX(freq, 50, FREQ_PRECISION);

    // ����60Hz�ļ�����
    sinSampleCreate(samples,800,4000,60,60,20);
#if 0
    for(int i=0; i<800; i++)
    {
        printf("%f\n", samples[i]);
    }
#endif
    /*filterx(4, a2_40_70Hz[0], b2_40_70Hz[0],
        800, samples, filteredSamples, 0);*/
    
    // �����˲����ȶ���ʹ����ļ���׼ȷ
    for(int i=0; i<15; i++)
    {
        iir_6th_filter(samples, a40_70Hz[0], b40_70Hz[0], g40_70Hz[0], &iir_state, 
            800, filteredSamples);
    }

    for(int i=0; i<30; i++)
    {    
        iir_6th_filter(samples, a40_70Hz[0], b40_70Hz[0], g40_70Hz[0], &iir_state, 
            800, filteredSamples);
        peroidNumCalc(800,4000,filteredSamples, 40, 70, &periodSum, &validSampleSum, &difTime);

        reValue = freqCalc(validSampleSum,4000,periodSum, difTime, &freq);
        ASSERT_TRUE(reValue==0);
        EPT_F_EX(freq, 60, FREQ_PRECISION);
    }
    // ����72Hz�ļ�����
    sinSampleCreate(samples,800,9000,72.5,30,20);   
    peroidNumCalc(800,9000,samples, 60, 80, &periodSum, &validSampleSum, &difTime);
    reValue = freqCalc(validSampleSum,9000,periodSum, difTime, &freq);
    ASSERT_TRUE(reValue==0);
    AST_F_EX(freq, 72.5, FREQ_PRECISION);

    // ����25.6Hz�ļ�����
    sinSampleCreate(samples,800,2048,25.6,-30,30);    
    peroidNumCalc(800,2048,samples, 10,30, &periodSum, &validSampleSum, &difTime);
    reValue = freqCalc(validSampleSum,2048,periodSum, difTime, &freq);
    ASSERT_TRUE(reValue==0);
    AST_F_EX(freq, 25.6, FREQ_PRECISION);

    // ����2�����ڵ����
    sinSampleCreate(samples,800,8000,25.6,-30,30);
    peroidNumCalc(800,8000,samples,10,50, &periodSum, &validSampleSum, &difTime);
    // ��2������
    ASSERT_EQ(periodSum,2);
    // 2��������625������
    ASSERT_EQ(validSampleSum, 625);
    reValue = freqCalc(validSampleSum,8000,periodSum, difTime, &freq);      
    ASSERT_TRUE(reValue == 0);    
    AST_F_EX(freq, 25.6,FREQ_PRECISION);
#endif

}
// ���ɲ��ԣ��������� + г�����㣩
TEST(ATM_TEST,INTEGRATE)
{
    float dcValue;
    float harmContents[127];
    float harmAngles[127];
    float channelData[2560];
    float omigas[4]={50,100,200,300};
    float initPhaseAngles[4]={-60,30,90,120};
    float widths[4]={220,30,50,20};
    sinSamplesCreate(channelData,256,12800,60,omigas,initPhaseAngles,widths,4);
    for(int i=1; i<10; i++)
    {
        for(int j=0; j<256; j++)
        {
            channelData[i*256+j]=channelData[j];
        }
    }
    harmCalc(HARM_2560, 2560, channelData, 50, &dcValue, harmContents, harmAngles);
    EPT_F_EX(dcValue, 60.0, HARM_DC_PRECISION);
    EPT_F_EX(harmContents[0], 220.0/SQRT2, HARM_PRECISION);
    EPT_F_EX(harmAngles[0], 300.0, HARM_PRECISION);
    EPT_F_EX(harmContents[1], 30.0/SQRT2, HARM_PRECISION);
    EPT_F_EX(harmAngles[1], 30.0, HARM_PRECISION);
    EPT_F_EX(harmContents[3], 50.0/SQRT2, HARM_PRECISION);
    EPT_F_EX(harmAngles[3], 90.0, HARM_PRECISION);
    EPT_F_EX(harmContents[5], 20.0/SQRT2, HARM_PRECISION);
    EPT_F_EX(harmAngles[5], 120.0, HARM_PRECISION);
#if 0
    for(int i=0; i<127; i++)
    {
        printf("%d:\t%f\t%f\n",i+1,harmContents[i],harmAngles[i]);
    }
#endif
}

// DSP������ɢ�ȼ��ɺ�������
TEST(ATM_TEST, DSP_DISP)
{
    MSGTIME pMsgTimes[4000];
    float pMsgDisp[11];

    Config config;

    // ���֧��8��ASDU
    config.nASDUCount = rand()%8+1;
    config.nMsgTimeCalcCount = 4000;
    config.nSamplingFreq = 4000;

    dispHashCreate(config.nASDUCount);
    resetDisp();

    // Ĭ�ϱ���ʱ���
    int defaultTimeDif = 1000000/config.nSamplingFreq*config.nASDUCount;

    pMsgTimes[0].nSecond = 0;
    pMsgTimes[0].nMicrosecond = 0;
    srand(GetTickCount());

    // ģ��洢�������ʱ������
    for(int j = 0; j<1000; j++)
    {
        for(int i = 1; i<4000; i++)
        {
            pMsgTimes[i].nSecond = pMsgTimes[i-1].nSecond;
            int nRand = rand()%100;
            // 78%����0
            if(nRand >0 && nRand <= 78)
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif;
            }
            // 1%��1
            else if(nRand>78 && nRand <= 78+1)
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif+1;
            }
            // 1%��-1
            else if(nRand>79 && nRand <= 79+1)
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif-1;
            }
            // 2%��2~9
            else if(nRand>80 && nRand <= 80+2)
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif+2+rand()%7;
            }
            // 1%��-2~-9
            else if(nRand>82 && nRand <= 82+1)
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif-2-rand()%7;
            }
            // 2%��10 ~ 50*ASDU����-1
            else if(nRand>83 && nRand <=83+2)
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif+10
                    +rand()%(50*config.nASDUCount-1-10);
            }
            // 3%��-10 ~ -50*ASDU����+1
            else if(nRand>85 && nRand <=85+3)
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif-10
                    -rand()%(50*config.nASDUCount-1-10);
            }
            // 4%��50*ASDU���� ~ 250*ASDU����-1
            else if(nRand>88 && nRand <=88+4)
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif+50*config.nASDUCount
                    +rand()%(200*config.nASDUCount-1);
            }
            // 3%��-50*ASDU���� ~ -250*ASDU����+1
            else if(nRand>92 && nRand <=92+3)
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif-50*config.nASDUCount
                    -rand()%(200*config.nASDUCount-1);
            }
            // 2%��>=250*ASDU����
            else if(nRand>95 && nRand <=95+2)
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif+250*config.nASDUCount
                    +rand()%300;
            }
            // 3%��<=-250*ASDU����
            else
            {
                pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+defaultTimeDif-250*config.nASDUCount
                    -rand()%300;
            }
            pMsgTimes[i].nSecond = pMsgTimes[i-1].nSecond;        

            //��λ
            if(pMsgTimes[i].nMicrosecond>1000000)
            {
                pMsgTimes[i].nMicrosecond-=1000000;
                pMsgTimes[i].nSecond++;
            }
        }
        MsgDispCalc(pMsgTimes, &config, pMsgDisp);
    }
        
    // ���ڱ�����ɢ����һ�ָ����㷨�����Բ�����Խ��Խ׼ȷ������4000000���㣬ȡ0.1%�����
    EPT_F_EX(pMsgDisp[0],3,0.1);
    EPT_F_EX(pMsgDisp[1],3,0.1);
    EPT_F_EX(pMsgDisp[2],3,0.1);
    EPT_F_EX(pMsgDisp[3],1,0.1);
    EPT_F_EX(pMsgDisp[4],1,0.1);
    EPT_F_EX(pMsgDisp[5],78,0.1);
    EPT_F_EX(pMsgDisp[6],1,0.1);
    EPT_F_EX(pMsgDisp[7],2,0.1);
    EPT_F_EX(pMsgDisp[8],2,0.1);
    EPT_F_EX(pMsgDisp[9],4,0.1);
    EPT_F_EX(pMsgDisp[10],2,0.1);
#if 0
    for(int i=0; i<11; i++)
    {
        printf("%d:%f\n", i, pMsgDisp[i]);
    }
#endif

    resetDisp();
    pMsgTimes[0].nSecond = 0;
    pMsgTimes[0].nMicrosecond = 0;
    srand(GetTickCount());
    for(int j = 0; j<1000; j++)
    {
        for(int i = 1; i<4000; i++)
        {                                    
            pMsgTimes[i].nSecond = pMsgTimes[i-1].nSecond;        
            pMsgTimes[i].nMicrosecond = pMsgTimes[i-1].nMicrosecond+250*config.nASDUCount;//rand()%499+1;
            //��λ
            if(pMsgTimes[i].nMicrosecond>=1000000)
            {
                pMsgTimes[i].nMicrosecond-=1000000;
                pMsgTimes[i].nSecond++;
            }
        }
        MsgDispCalc(pMsgTimes, &config, pMsgDisp);
        pMsgTimes[0].nSecond = pMsgTimes[4000-1].nSecond;        
        pMsgTimes[0].nMicrosecond = pMsgTimes[4000-1].nMicrosecond+250*config.nASDUCount;//rand()%499+1;
        //��λ
        if(pMsgTimes[0].nMicrosecond>=1000000)
        {
            pMsgTimes[0].nMicrosecond-=1000000;
            pMsgTimes[0].nSecond++;
        }
    }
    EPT_F_EX(pMsgDisp[0],0,0.1);
    EPT_F_EX(pMsgDisp[1],0,0.1);
    EPT_F_EX(pMsgDisp[2],0,0.1);
    EPT_F_EX(pMsgDisp[3],0,0.1);
    EPT_F_EX(pMsgDisp[4],0,0.1);
    EPT_F_EX(pMsgDisp[5],100,0.1);
    EPT_F_EX(pMsgDisp[6],0,0.1);
    EPT_F_EX(pMsgDisp[7],0,0.1);
    EPT_F_EX(pMsgDisp[8],0,0.1);
    EPT_F_EX(pMsgDisp[9],0,0.1);
    EPT_F_EX(pMsgDisp[10],0,0.1);

}
#if 1
// DSP����ֵ�������
TEST(ATM_TEST, SMV)
{
    SmvCalcData *scd = new SmvCalcData;
    Config *cfg = new Config;    
    SMV_DATA_CALC_RT *sdcr = new SMV_DATA_CALC_RT;
    cfg->nSamplingFreq = 12800;
    cfg->nRatedFreq = 50;
    // ����12��ͨ������
    scd->nChannelCount = 12;
    scd->nSmvPerCircle = 256;
    scd->nChannelVCType[0] = SMV_VCTYPE_VOLTAGE|SMV_VCTYPE_PHASE_A;
    scd->nChannelVCType[1] = SMV_VCTYPE_VOLTAGE|SMV_VCTYPE_PHASE_B;
    scd->nChannelVCType[2] = SMV_VCTYPE_VOLTAGE|SMV_VCTYPE_PHASE_C;
    scd->nChannelVCType[3] = SMV_VCTYPE_CURRENT|SMV_VCTYPE_PHASE_A;
    scd->nChannelVCType[4] = SMV_VCTYPE_CURRENT|SMV_VCTYPE_PHASE_B;
    scd->nChannelVCType[5] = SMV_VCTYPE_CURRENT|SMV_VCTYPE_PHASE_C;
    scd->nChannelVCType[6] = SMV_VCTYPE_VOLTAGE|SMV_VCTYPE_PHASE_D;
    scd->nChannelVCType[7] = SMV_VCTYPE_VOLTAGE|SMV_VCTYPE_PHASE_E;
    scd->nChannelVCType[8] = SMV_VCTYPE_VOLTAGE|SMV_VCTYPE_PHASE_F;
    scd->nChannelVCType[9] = SMV_VCTYPE_CURRENT|SMV_VCTYPE_PHASE_D;
    scd->nChannelVCType[10] = SMV_VCTYPE_CURRENT|SMV_VCTYPE_PHASE_E;
    scd->nChannelVCType[11] = SMV_VCTYPE_CURRENT|SMV_VCTYPE_PHASE_F;
    float fOmigas[4] = {50,100,150,300};
    float fInitPhaseAngles[4] = {30,50,90,-60};
    float fWidths[4] = {220,0.1,0.2,0.5};
    for(int i=0; i<scd->nChannelCount; i++)
    {
        sinSamplesCreate(scd->fChannelData[i],scd->nSmvPerCircle*10,
            cfg->nSamplingFreq,0,fOmigas,fInitPhaseAngles,fWidths, 4);
    }
#if 0
    for(int i=0; i<scd->nSmvPerCircle*10; i++)
    {
        printf("%f\n",scd->fChannelData[0][i]);
    }
#endif
    for(int i=0; i<40; i++)
    {
        SmvDataCalc(scd, cfg, sdcr);
    }    
    AST_F_EX(sdcr->fHarmContent[0][0],220.0/SQRT2,HARM_PRECISION);
    AST_F_EX(sdcr->fHarmContent[1][0],220.0/SQRT2,HARM_PRECISION);
    AST_F_EX(sdcr->fHarmAngle[1][0],30,HARM_PRECISION);
    AST_F_EX(sdcr->fHarmAngle[1][1],50,HARM_PRECISION);
    delete scd;
    delete cfg;
    delete sdcr;
}
#endif