/// @file
///     fft.cpp
/// @brief
///     复合基傅里叶变换算法
/// @note
///     
///
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/20
/// @revise
/// 
/*******************************************************************************
复合基FFT算法说明：
    通用的FFT算法需要采样点N为2的幂，也就是基2的FFT算法，当N不为2的幂的时候如何
计算FFT呢？处理方法有3个：
    1）通过补零，使序列长度等于2的幂，然后采用基2FFT算法即可
    2）N=ML（复合数），采用复合基FFT算法
    3）N!=ML（素数），采用Chirp-Z变换（CZT）
    对于800点FFT，由于800是个复合数，通过因式分解得800=8*10*10，根据按时间提取的
复合基算法，先将原数组按复合基逆序排列，然后分别计算80个基10FFT，10次8个基10FFT和
100次基8FFT，相当于如下8*10*10三维矩阵（按8*10*10逆序，即(i,j,k)->(k,j,i)，并进行
旋转映射，即由原来的100*i+10*j+k-->80*k+10*j+i），首先计算第一维10点的FFT，也就是
每一块中每一行的10个数的FFT，然后计算第二维10点FFT，也就是每一块中每一列的10个数
的FFT，并乘上旋转因子，然后计算第三维8点FFT，也就是每一块中相等行列的8个数的FFT，
并乘上旋转因子，最后的结果便是800点FFT结果了。
(k,j,i)
|(0,0,0),(1,0,0),(2,0,0),(3,0,0),(4,0,0),(5,0,0),(6,0,0),(7,0,0),(8,0,0),(9,0,0)|
|(0,1,0),(1,1,0),(2,1,0),(3,1,0),(4,1,0),(5,1,0),(6,1,0),(7,1,0),(8,1,0),(9,1,0)|
|(0,2,0),(1,2,0),(2,2,0),(3,2,0),(4,2,0),(5,2,0),(6,2,0),(7,2,0),(8,2,0),(9,2,0)|
|(0,3,0),(1,3,0),(2,3,0),(3,3,0),(4,3,0),(5,3,0),(6,3,0),(7,3,0),(8,3,0),(9,3,0)|
|(0,4,0),(1,4,0),(2,4,0),(3,4,0),(4,4,0),(5,4,0),(6,4,0),(7,4,0),(8,4,0),(9,4,0)|
|(0,5,0),(1,5,0),(2,5,0),(3,5,0),(4,5,0),(5,5,0),(6,5,0),(7,5,0),(8,5,0),(9,5,0)|
|(0,6,0),(1,6,0),(2,6,0),(3,6,0),(4,6,0),(5,6,0),(6,6,0),(7,6,0),(8,6,0),(9,6,0)|
|(0,7,0),(1,7,0),(2,7,0),(3,7,0),(4,7,0),(5,7,0),(6,7,0),(7,7,0),(8,7,0),(9,7,0)|
|(0,8,0),(1,8,0),(2,8,0),(3,8,0),(4,8,0),(5,8,0),(6,8,0),(7,8,0),(8,8,0),(9,8,0)|
|(0,9,0),(1,9,0),(2,9,0),(3,9,0),(4,9,0),(5,9,0),(6,9,0),(7,9,0),(8,9,0),(9,9,0)|

|(0,0,1),(1,0,1),(2,0,1),(3,0,1),(4,0,1),(5,0,1),(6,0,1),(7,0,1),(8,0,1),(9,0,1)|
|(0,1,1),(1,1,1),(2,1,1),(3,1,1),(4,1,1),(5,1,1),(6,1,1),(7,1,1),(8,1,1),(9,1,1)|
|(0,2,1),(1,2,1),(2,2,1),(3,2,1),(4,2,1),(5,2,1),(6,2,1),(7,2,1),(8,2,1),(9,2,1)|
|(0,3,1),(1,3,1),(2,3,1),(3,3,1),(4,3,1),(5,3,1),(6,3,1),(7,3,1),(8,3,1),(9,3,1)|
|(0,4,1),(1,4,1),(2,4,1),(3,4,1),(4,4,1),(5,4,1),(6,4,1),(7,4,1),(8,4,1),(9,4,1)|
|(0,5,1),(1,5,1),(2,5,1),(3,5,1),(4,5,1),(5,5,1),(6,5,1),(7,5,1),(8,5,1),(9,5,1)|
|(0,6,1),(1,6,1),(2,6,1),(3,6,1),(4,6,1),(5,6,1),(6,6,1),(7,6,1),(8,6,1),(9,6,1)|
|(0,7,1),(1,7,1),(2,7,1),(3,7,1),(4,7,1),(5,7,1),(6,7,1),(7,7,1),(8,7,1),(9,7,1)|
|(0,8,1),(1,8,1),(2,8,1),(3,8,1),(4,8,1),(5,8,1),(6,8,1),(7,8,1),(8,8,1),(9,8,1)|
|(0,9,1),(1,9,1),(2,9,1),(3,9,1),(4,9,1),(5,9,1),(6,9,1),(7,9,1),(8,9,1),(9,9,1)|
...

|(0,0,7),(1,0,7),(2,0,7),(3,0,7),(4,0,7),(5,0,7),(6,0,7),(7,0,7),(8,0,7),(9,0,7)|
|(0,1,7),(1,1,7),(2,1,7),(3,1,7),(4,1,7),(5,1,7),(6,1,7),(7,1,7),(8,1,7),(9,1,7)|
|(0,2,7),(1,2,7),(2,2,7),(3,2,7),(4,2,7),(5,2,7),(6,2,7),(7,2,7),(8,2,7),(9,2,7)|
|(0,3,7),(1,3,7),(2,3,7),(3,3,7),(4,3,7),(5,3,7),(6,3,7),(7,3,7),(8,3,7),(9,3,7)|
|(0,4,7),(1,4,7),(2,4,7),(3,4,7),(4,4,7),(5,4,7),(6,4,7),(7,4,7),(8,4,7),(9,4,7)|
|(0,5,7),(1,5,7),(2,5,7),(3,5,7),(4,5,7),(5,5,7),(6,5,7),(7,5,7),(8,5,7),(9,5,7)|
|(0,6,7),(1,6,7),(2,6,7),(3,6,7),(4,6,7),(5,6,7),(6,6,7),(7,6,7),(8,6,7),(9,6,7)|
|(0,7,7),(1,7,7),(2,7,7),(3,7,7),(4,7,7),(5,7,7),(6,7,7),(7,7,7),(8,7,7),(9,7,7)|
|(0,8,7),(1,8,7),(2,8,7),(3,8,7),(4,8,7),(5,8,7),(6,8,7),(7,8,7),(8,8,7),(9,8,7)|
|(0,9,7),(1,9,7),(2,9,7),(3,9,7),(4,9,7),(5,9,7),(6,9,7),(7,9,7),(8,9,7),(9,9,7)|

*******************************************************************************/
#include "fft.h"
           
#define  maxPrimeFactor        37           ///< 最大素数因子

 float    twiddleRe[maxPrimeFactor];
 float    twiddleIm[maxPrimeFactor];
 float    zRe[maxPrimeFactor];
 float    zIm[maxPrimeFactor];

// 旋转因子辅助值
 const float c5_1 = -2.5000000e-1;        ///< cos(360/5)=cos(360/5*4)=c5_1+c5_2
 const float c5_2 =  5.5901700e-1;        ///< cos(360/5*2)=cos(360/5*3=)c5_1-c5_2
 const float c5_3 = -9.5105654e-1;        ///< -sin(360/5)=sin(360/5*4)
 const float c5_4 = -1.5388417;           ///< sin(360/5*2)=-sin(360/5*3)=c5_3-c5_4
 const float c5_5 =  3.6327127e-1;        ///< -sin(360/5*2)=sin(360/5*3)=c5_3+c5_5 
 const float c8   =  7.0710677e-1;        ///< cos(360/8)=sin(360/8)

 const float c10_1 = 0.8090169943749475;    ///< cos(360/10)
 const float c10_2 = 0.5877852522924731;    ///< sin(360/10)
 const float c10_3 = 0.30901699437494745;   ///< cos(360/10*2)
 const float c10_4 = 0.9510565162951535;    ///< sin(360/10*2)
 const float c10_5 = -0.30901699437494745;  ///< cos(360/10*3)
 const float c10_6 = 0.9510565162951535;    ///< sin(360/10*3)
 const float c10_7 = -0.8090169943749475;   ///< cos(360/10*4)
 const float c10_8 = 0.5877852522924731;    ///< sin(360/10*4)

 const float c16_1 = 9.2387953e-1;        ///< cos(360/16)
 const float c16_2 = 3.8268343e-1;        ///< sin(360/16)

 const float c20_1 = 9.5105652e-1;        ///< cos(360/20)
 const float c20_2 = 3.0901699e-1;        ///< sin(360/20)
 const float c20_3 = 8.0901699e-1;        ///< cos(360/20*2)
 const float c20_4 = 5.8778525e-1;        ///< sin(360/20*2)
 const float c20_5 = 5.8778525e-1;        ///< cos(360/20*3)
 const float c20_6 = 8.0901699e-1;        ///< sin(360/20*3)
 const float c20_7 = 3.0901699e-1;        ///< cos(360/20*4)
 const float c20_8 = 9.5105652e-1;        ///< sin(360/20*4)
 const float c20_9 = 0;                   ///< cos(360/20*5)
 const float c20_10 = 1;                  ///< sin(360/20*5)
 const float c20_11 = -3.0901699e-1;      ///< cos(360/20*6)
 const float c20_12 = 9.5105652e-1;       ///< sin(360/20*6)
 const float c20_13 = -5.8778525e-1;      ///< cos(360/20*7)
 const float c20_14 = 8.0901699e-1;       ///< sin(360/20*7)
 const float c20_15 = -8.0901699e-1;      ///< cos(360/20*8)
 const float c20_16 = 5.8778525e-1;       ///< sin(360/20*8)
 const float c20_17 = -9.5105652e-1;      ///< cos(360/20*9)
 const float c20_18 = 3.0901699e-1;       ///< sin(360/20*9)


 float omega, tw_re,tw_im;

 Uint16 groupOffset,dataOffset,adr;
 Uint16 groupNo,dataNo,blockNo,twNo;


///
/// @brief
///    4点FFT算法
/// @param[inout] 
///    aRe  实数序列
///    aIm  虚数序列
/// @return
///    void
/// 
 void fft_4(float aRe[], float aIm[])
{
    float  t1_re,t1_im, t2_re,t2_im;
    float  m2_re,m2_im, m3_re,m3_im;

    t1_re  = addsp_i(aRe[0],aRe[2]); 
    t1_im  = addsp_i(aIm[0],aIm[2]);
    t2_re  = addsp_i(aRe[1],aRe[3]); 
    t2_im  = addsp_i(aIm[1],aIm[3]);

    m2_re  = subsp_i(aRe[0],aRe[2]); 
    m2_im  = subsp_i(aIm[0],aIm[2]);
    m3_re  = subsp_i(aIm[1],aIm[3]); 
    m3_im  = subsp_i(aRe[3],aRe[1]);

    aRe[0] = addsp_i(t1_re,t2_re); 
    aIm[0] = addsp_i(t1_im,t2_im);
    aRe[2] = subsp_i(t1_re,t2_re); 
    aIm[2] = subsp_i(t1_im,t2_im);
    aRe[1] = addsp_i(m2_re,m3_re); 
    aIm[1] = addsp_i(m2_im,m3_im);
    aRe[3] = subsp_i(m2_re,m3_re); 
    aIm[3] = subsp_i(m2_im,m3_im);
}  
///
/// @brief
///    5点FFT算法
/// @param[inout] 
///    aRe  实数序列
///    aIm  虚数序列
/// @return
///    void
///
 void fft_5(float aRe[], float aIm[])
{
    float  t1_re,t1_im, t2_re,t2_im, t3_re,t3_im;
    float  t4_re,t4_im, t5_re,t5_im;
    float  m2_re,m2_im, m3_re,m3_im, m4_re,m4_im;
    float  m1_re,m1_im, m5_re,m5_im;
    float  s1_re,s1_im, s2_re,s2_im, s3_re,s3_im;
    float  s4_re,s4_im, s5_re,s5_im;

    t1_re=addsp_i(aRe[1],aRe[4]); t1_im=addsp_i(aIm[1],aIm[4]);
    t2_re=addsp_i(aRe[2],aRe[3]); t2_im=addsp_i(aIm[2],aIm[3]);
    t3_re=subsp_i(aRe[1],aRe[4]); t3_im=subsp_i(aIm[1],aIm[4]);
    t4_re=subsp_i(aRe[3],aRe[2]); t4_im=subsp_i(aIm[3],aIm[2]);
    t5_re=addsp_i(t1_re,t2_re);   t5_im=addsp_i(t1_im,t2_im);

    m1_re=mpysp_i(c5_1,t5_re);           
    m1_im=mpysp_i(c5_1,t5_im);
    m2_re=mpysp_i(c5_2,subsp_i(t1_re,t2_re)); 
    m2_im=mpysp_i(c5_2,subsp_i(t1_im,t2_im));

    m3_re=-mpysp_i(c5_3,addsp_i(t3_im,t4_im)); 
    m3_im= mpysp_i(c5_3,addsp_i(t3_re,t4_re));
    m4_re=-mpysp_i(c5_4,t4_im);	m4_im=mpysp_i(c5_4,t4_re);
    m5_re=-mpysp_i(c5_5,t3_im);	m5_im=mpysp_i(c5_5,t3_re);

    s3_re=subsp_i(m3_re ,m4_re);  s3_im=subsp_i(m3_im ,m4_im);
    s5_re=addsp_i(m3_re ,m5_re);  s5_im=addsp_i(m3_im ,m5_im);
    s1_re=addsp_i(aRe[0],m1_re);  s1_im=addsp_i(aIm[0],m1_im);
    s2_re=addsp_i(s1_re ,m2_re);  s2_im=addsp_i(s1_im ,m2_im);
    s4_re=subsp_i(s1_re ,m2_re);  s4_im=subsp_i(s1_im ,m2_im);

    aRe[0]=addsp_i(aRe[0],t5_re);  aIm[0]=addsp_i(aIm[0],t5_im);
    aRe[1]=addsp_i(s2_re ,s3_re);  aIm[1]=addsp_i(s2_im ,s3_im);
    aRe[2]=addsp_i(s4_re ,s5_re);  aIm[2]=addsp_i(s4_im ,s5_im);
    aRe[3]=subsp_i(s4_re ,s5_re);  aIm[3]=subsp_i(s4_im ,s5_im);
    aRe[4]=subsp_i(s2_re ,s3_re);  aIm[4]=subsp_i(s2_im ,s3_im);
}  


///
/// @brief
///    8点FFT算法
/// @param[inout] 
///    zRe      实数序列
///    zIm      虚数序列
/// @return
///    void
///
 void fft_8(float zRe[], float zIm[])
{
    float  aRe[4], aIm[4], bRe[4], bIm[4], gem;

    aRe[0] = zRe[0];    bRe[0] = zRe[1];
    aRe[1] = zRe[2];    bRe[1] = zRe[3];
    aRe[2] = zRe[4];    bRe[2] = zRe[5];
    aRe[3] = zRe[6];    bRe[3] = zRe[7];

    aIm[0] = zIm[0];    bIm[0] = zIm[1];
    aIm[1] = zIm[2];    bIm[1] = zIm[3];
    aIm[2] = zIm[4];    bIm[2] = zIm[5];
    aIm[3] = zIm[6];    bIm[3] = zIm[7];

    fft_4(aRe, aIm);
    fft_4(bRe, bIm);

    gem    =mpysp_i(c8,addsp_i(bRe[1],bIm[1]));
    bIm[1] =mpysp_i(c8,subsp_i(bIm[1],bRe[1]));
    bRe[1] = gem;

    gem    = bIm[2];
    bIm[2] =-bRe[2];
    bRe[2] = gem;

    gem    = mpysp_i(c8,subsp_i(bIm[3],bRe[3]));
    bIm[3] =-mpysp_i(c8,addsp_i(bRe[3],bIm[3]));
    bRe[3] = gem;
    
    zRe[0] = addsp_i(aRe[0],bRe[0]); zRe[4] = subsp_i(aRe[0],bRe[0]);
    zIm[0] = addsp_i(aIm[0],bIm[0]); zIm[4] = subsp_i(aIm[0],bIm[0]);
                                                         
    zRe[1] = addsp_i(aRe[1],bRe[1]); zRe[5] = subsp_i(aRe[1],bRe[1]);
    zIm[1] = addsp_i(aIm[1],bIm[1]); zIm[5] = subsp_i(aIm[1],bIm[1]);
                                                           
    zRe[2] = addsp_i(aRe[2],bRe[2]); zRe[6] = subsp_i(aRe[2],bRe[2]);
    zIm[2] = addsp_i(aIm[2],bIm[2]); zIm[6] = subsp_i(aIm[2],bIm[2]);

    zRe[3] = addsp_i(aRe[3],bRe[3]); zRe[7] = subsp_i(aRe[3],bRe[3]);
    zIm[3] = addsp_i(aIm[3],bIm[3]); zIm[7] = subsp_i(aIm[3],bIm[3]);

}  

///
/// @brief
///    10点FFT算法
/// @param[inout] 
///    zRe      实数序列
///    zIm      虚数序列
/// @return
///    void
///
 void fft_10(float zRe[], float zIm[])
{
    float  aRe[5], aIm[5], bRe[5], bIm[5];

    aRe[0] = zRe[0];    bRe[0] = zRe[5];
    aRe[1] = zRe[2];    bRe[1] = zRe[7];
    aRe[2] = zRe[4];    bRe[2] = zRe[9];
    aRe[3] = zRe[6];    bRe[3] = zRe[1];
    aRe[4] = zRe[8];    bRe[4] = zRe[3];

    aIm[0] = zIm[0];    bIm[0] = zIm[5];
    aIm[1] = zIm[2];    bIm[1] = zIm[7];
    aIm[2] = zIm[4];    bIm[2] = zIm[9];
    aIm[3] = zIm[6];    bIm[3] = zIm[1];
    aIm[4] = zIm[8];    bIm[4] = zIm[3];

    fft_5(aRe, aIm); 
    fft_5(bRe, bIm);


    zRe[0] = addsp_i(aRe[0],bRe[0]); zRe[5] = subsp_i(aRe[0],bRe[0]);
    zRe[6] = addsp_i(aRe[1],bRe[1]); zRe[1] = subsp_i(aRe[1],bRe[1]);
    zRe[2] = addsp_i(aRe[2],bRe[2]); zRe[7] = subsp_i(aRe[2],bRe[2]);
    zRe[8] = addsp_i(aRe[3],bRe[3]); zRe[3] = subsp_i(aRe[3],bRe[3]);
    zRe[4] = addsp_i(aRe[4],bRe[4]); zRe[9] = subsp_i(aRe[4],bRe[4]);
                         
    zIm[0] = addsp_i(aIm[0],bIm[0]); zIm[5] = subsp_i(aIm[0],bIm[0]);
    zIm[6] = addsp_i(aIm[1],bIm[1]); zIm[1] = subsp_i(aIm[1],bIm[1]);
    zIm[2] = addsp_i(aIm[2],bIm[2]); zIm[7] = subsp_i(aIm[2],bIm[2]);
    zIm[8] = addsp_i(aIm[3],bIm[3]); zIm[3] = subsp_i(aIm[3],bIm[3]);
    zIm[4] = addsp_i(aIm[4],bIm[4]); zIm[9] = subsp_i(aIm[4],bIm[4]);
}

 ///
 /// @brief
 ///    10点FFT算法1
 /// @param[inout] 
 ///    zRe      实数序列
 ///    zIm      虚数序列
 /// @return
 ///    void
 ///
#if 1
void fft_10_1(float zRe[], float zIm[])
{
    float  aRe[5], aIm[5], bRe[5], bIm[5], gem;

    aRe[0] = zRe[0];    bRe[0] = zRe[1];
    aRe[1] = zRe[2];    bRe[1] = zRe[3];
    aRe[2] = zRe[4];    bRe[2] = zRe[5];
    aRe[3] = zRe[6];    bRe[3] = zRe[7];
    aRe[4] = zRe[8];    bRe[4] = zRe[9];

    aIm[0] = zIm[0];    bIm[0] = zIm[1];
    aIm[1] = zIm[2];    bIm[1] = zIm[3];
    aIm[2] = zIm[4];    bIm[2] = zIm[5];
    aIm[3] = zIm[6];    bIm[3] = zIm[7];
    aIm[4] = zIm[8];    bIm[4] = zIm[9];

    fft_5(aRe, aIm); 
    fft_5(bRe, bIm);

    //bind the scale factors W(10,n) to the values just fft calculated.
    gem    =addsp_i(mpysp_i(bRe[1],c10_1),mpysp_i(bIm[1],c10_2));
    bIm[1] =subsp_i(mpysp_i(bIm[1],c10_1),mpysp_i(bRe[1],c10_2));    
    bRe[1] = gem;

    gem    =addsp_i(mpysp_i(bRe[2],c10_3),mpysp_i(bIm[2],c10_4));
    bIm[2] =subsp_i(mpysp_i(bIm[2],c10_3),mpysp_i(bRe[2],c10_4));    
    bRe[2] = gem;

    gem    =addsp_i(mpysp_i(bRe[3],c10_5),mpysp_i(bIm[3],c10_6));
    bIm[3] =subsp_i(mpysp_i(bIm[3],c10_5),mpysp_i(bRe[3],c10_6));    
    bRe[3] = gem;

    gem    =addsp_i(mpysp_i(bRe[4],c10_7),mpysp_i(bIm[4],c10_8));
    bIm[4] =subsp_i(mpysp_i(bIm[4],c10_7),mpysp_i(bRe[4],c10_8));    
    bRe[4] = gem;
    
    zRe[0] = addsp_i(aRe[0],bRe[0]); zRe[5] = subsp_i(aRe[0],bRe[0]);
    zIm[0] = addsp_i(aIm[0],bIm[0]); zIm[5] = subsp_i(aIm[0],bIm[0]);

    zRe[1] = addsp_i(aRe[1],bRe[1]); zRe[6] = subsp_i(aRe[1],bRe[1]);
    zIm[1] = addsp_i(aIm[1],bIm[1]); zIm[6] = subsp_i(aIm[1],bIm[1]);

    zRe[2] = addsp_i(aRe[2],bRe[2]); zRe[7] = subsp_i(aRe[2],bRe[2]);
    zIm[2] = addsp_i(aIm[2],bIm[2]); zIm[7] = subsp_i(aIm[2],bIm[2]);

    zRe[3] = addsp_i(aRe[3],bRe[3]); zRe[8] = subsp_i(aRe[3],bRe[3]);
    zIm[3] = addsp_i(aIm[3],bIm[3]); zIm[8] = subsp_i(aIm[3],bIm[3]);

    zRe[4] = addsp_i(aRe[4],bRe[4]); zRe[9] = subsp_i(aRe[4],bRe[4]);
    zIm[4] = addsp_i(aIm[4],bIm[4]); zIm[9] = subsp_i(aIm[4],bIm[4]);
}
#endif
///
/// @brief
///    16点FFT算法
/// @param[inout] 
///    zRe      实数序列
///    zIm      虚数序列
/// @return
///    void
///
 void fft_16(float zRe[], float zIm[])
{
    float  aRe[8], aIm[8], bRe[8], bIm[8], gem;

    aRe[0] = zRe[0];     bRe[0] = zRe[1];
    aRe[1] = zRe[2];     bRe[1] = zRe[3];
    aRe[2] = zRe[4];     bRe[2] = zRe[5];
    aRe[3] = zRe[6];     bRe[3] = zRe[7];
    aRe[4] = zRe[8];     bRe[4] = zRe[9];
    aRe[5] = zRe[10];    bRe[5] = zRe[11];
    aRe[6] = zRe[12];    bRe[6] = zRe[13];
    aRe[7] = zRe[14];    bRe[7] = zRe[15];

    aIm[0] = zIm[0];     bIm[0] = zIm[1];
    aIm[1] = zIm[2];     bIm[1] = zIm[3];
    aIm[2] = zIm[4];     bIm[2] = zIm[5];
    aIm[3] = zIm[6];     bIm[3] = zIm[7];
    aIm[4] = zIm[8];     bIm[4] = zIm[9];
    aIm[5] = zIm[10];    bIm[5] = zIm[11];
    aIm[6] = zIm[12];    bIm[6] = zIm[13];
    aIm[7] = zIm[14];    bIm[7] = zIm[15];

    fft_8(aRe, aIm);
    fft_8(bRe, bIm);

    gem    =addsp_i(mpysp_i(bRe[1],c16_1),mpysp_i(bIm[1],c16_2));
    bIm[1] =subsp_i(mpysp_i(bIm[1],c16_1),mpysp_i(bRe[1],c16_2));    
    bRe[1] = gem;

    gem    =mpysp_i(c8,addsp_i(bRe[2],bIm[2]));
    bIm[2] =mpysp_i(c8,subsp_i(bIm[2],bRe[2]));
    bRe[2] = gem;
    
    gem    = addsp_i(mpysp_i(bRe[3],c16_2),mpysp_i(bIm[3],c16_1));
    bIm[3] = subsp_i(mpysp_i(bIm[3],c16_2),mpysp_i(bRe[3],c16_1));
    bRe[3] = gem;

    gem    = bIm[4];
    bIm[4] =-bRe[4];
    bRe[4] = gem;
    
    gem    =  subsp_i(mpysp_i(bIm[5],c16_1),mpysp_i(bRe[5],c16_2)); 
    bIm[5] = -addsp_i(mpysp_i(bRe[5],c16_1),mpysp_i(bIm[5],c16_2));
    bRe[5] = gem;

    gem    =  mpysp_i(c8,subsp_i(bIm[6],bRe[6]));
    bIm[6] = -mpysp_i(c8,addsp_i(bRe[6],bIm[6]));
    bRe[6] = gem;

    gem    =  subsp_i(mpysp_i(bIm[7],c16_2),mpysp_i(bRe[7],c16_1)); 
    bIm[7] = -addsp_i(mpysp_i(bRe[7],c16_2),mpysp_i(bIm[7],c16_1));
    bRe[7] = gem;

    zRe[0] = addsp_i(aRe[0],bRe[0]); zRe[8] = subsp_i(aRe[0],bRe[0]);
    zIm[0] = addsp_i(aIm[0],bIm[0]); zIm[8] = subsp_i(aIm[0],bIm[0]);

    zRe[1] = addsp_i(aRe[1],bRe[1]); zRe[9] = subsp_i(aRe[1],bRe[1]);
    zIm[1] = addsp_i(aIm[1],bIm[1]); zIm[9] = subsp_i(aIm[1],bIm[1]);

    zRe[2] = addsp_i(aRe[2],bRe[2]); zRe[10] = subsp_i(aRe[2],bRe[2]);
    zIm[2] = addsp_i(aIm[2],bIm[2]); zIm[10] = subsp_i(aIm[2],bIm[2]);

    zRe[3] = addsp_i(aRe[3],bRe[3]); zRe[11] = subsp_i(aRe[3],bRe[3]);
    zIm[3] = addsp_i(aIm[3],bIm[3]); zIm[11] = subsp_i(aIm[3],bIm[3]);

    zRe[4] = addsp_i(aRe[4],bRe[4]); zRe[12] = subsp_i(aRe[4],bRe[4]);
    zIm[4] = addsp_i(aIm[4],bIm[4]); zIm[12] = subsp_i(aIm[4],bIm[4]);

    zRe[5] = addsp_i(aRe[5],bRe[5]); zRe[13] = subsp_i(aRe[5],bRe[5]);
    zIm[5] = addsp_i(aIm[5],bIm[5]); zIm[13] = subsp_i(aIm[5],bIm[5]);

    zRe[6] = addsp_i(aRe[6],bRe[6]); zRe[14] = subsp_i(aRe[6],bRe[6]);
    zIm[6] = addsp_i(aIm[6],bIm[6]); zIm[14] = subsp_i(aIm[6],bIm[6]);

    zRe[7] = addsp_i(aRe[7],bRe[7]); zRe[15] = subsp_i(aRe[7],bRe[7]);
    zIm[7] = addsp_i(aIm[7],bIm[7]); zIm[15] = subsp_i(aIm[7],bIm[7]);
}
 ///
 /// @brief
 ///    20点FFT算法
 /// @param[inout] 
 ///    zRe      实数序列
 ///    zIm      虚数序列
 /// @return
 ///    void
 ///
 void fft_20(float zRe[], float zIm[])
 {
     float  aRe[10], aIm[10], bRe[10], bIm[10], gem;

     aRe[0] = zRe[0];     bRe[0] = zRe[1];
     aRe[1] = zRe[2];     bRe[1] = zRe[3];
     aRe[2] = zRe[4];     bRe[2] = zRe[5];
     aRe[3] = zRe[6];     bRe[3] = zRe[7];
     aRe[4] = zRe[8];     bRe[4] = zRe[9];
     aRe[5] = zRe[10];    bRe[5] = zRe[11];
     aRe[6] = zRe[12];    bRe[6] = zRe[13];
     aRe[7] = zRe[14];    bRe[7] = zRe[15];
     aRe[8] = zRe[16];    bRe[8] = zRe[17];
     aRe[9] = zRe[18];    bRe[9] = zRe[19];     

     aIm[0] = zIm[0];     bIm[0] = zIm[1];
     aIm[1] = zIm[2];     bIm[1] = zIm[3];
     aIm[2] = zIm[4];     bIm[2] = zIm[5];
     aIm[3] = zIm[6];     bIm[3] = zIm[7];
     aIm[4] = zIm[8];     bIm[4] = zIm[9];
     aIm[5] = zIm[10];    bIm[5] = zIm[11];
     aIm[6] = zIm[12];    bIm[6] = zIm[13];
     aIm[7] = zIm[14];    bIm[7] = zIm[15];
     aIm[8] = zIm[16];    bIm[8] = zIm[17];
     aIm[9] = zIm[18];    bIm[9] = zIm[19];

     fft_10(aRe, aIm);
     fft_10(bRe, bIm);

     gem    =addsp_i(mpysp_i(bRe[1],c20_1),mpysp_i(bIm[1],c20_2));
     bIm[1] =subsp_i(mpysp_i(bIm[1],c20_1),mpysp_i(bRe[1],c20_2));    
     bRe[1] = gem;

     gem    =addsp_i(mpysp_i(bRe[2],c20_3),mpysp_i(bIm[2],c20_4));
     bIm[2] =subsp_i(mpysp_i(bIm[2],c20_3),mpysp_i(bRe[2],c20_4));    
     bRe[2] = gem;

     gem    =addsp_i(mpysp_i(bRe[3],c20_5),mpysp_i(bIm[3],c20_6));
     bIm[3] =subsp_i(mpysp_i(bIm[3],c20_5),mpysp_i(bRe[3],c20_6));    
     bRe[3] = gem;

     gem    =addsp_i(mpysp_i(bRe[4],c20_7),mpysp_i(bIm[4],c20_8));
     bIm[4] =subsp_i(mpysp_i(bIm[4],c20_7),mpysp_i(bRe[4],c20_8));    
     bRe[4] = gem;

     gem    =addsp_i(mpysp_i(bRe[5],c20_9),mpysp_i(bIm[5],c20_10));
     bIm[5] =subsp_i(mpysp_i(bIm[5],c20_9),mpysp_i(bRe[5],c20_10));
     bRe[5] = gem;

     gem    =addsp_i(mpysp_i(bRe[6],c20_11),mpysp_i(bIm[6],c20_12));
     bIm[6] =subsp_i(mpysp_i(bIm[6],c20_11),mpysp_i(bRe[6],c20_12));    
     bRe[6] = gem;

     gem    =addsp_i(mpysp_i(bRe[7],c20_13),mpysp_i(bIm[7],c20_14));
     bIm[7] =subsp_i(mpysp_i(bIm[7],c20_13),mpysp_i(bRe[7],c20_14));    
     bRe[7] = gem;

     gem    =addsp_i(mpysp_i(bRe[8],c20_15),mpysp_i(bIm[8],c20_16));
     bIm[8] =subsp_i(mpysp_i(bIm[8],c20_15),mpysp_i(bRe[8],c20_16));    
     bRe[8] = gem;

     gem    =addsp_i(mpysp_i(bRe[9],c20_17),mpysp_i(bIm[9],c20_18));
     bIm[9] =subsp_i(mpysp_i(bIm[9],c20_17),mpysp_i(bRe[9],c20_18));    
     bRe[9] = gem;

     zRe[0] = addsp_i(aRe[0],bRe[0]);   zRe[10] = subsp_i(aRe[0],bRe[0]);
     zIm[0] = addsp_i(aIm[0],bIm[0]);   zIm[10] = subsp_i(aIm[0],bIm[0]);
                                        
     zRe[1] = addsp_i(aRe[1],bRe[1]);   zRe[11] = subsp_i(aRe[1],bRe[1]);
     zIm[1] = addsp_i(aIm[1],bIm[1]);   zIm[11] = subsp_i(aIm[1],bIm[1]);
                                        
     zRe[2] = addsp_i(aRe[2],bRe[2]);   zRe[12] = subsp_i(aRe[2],bRe[2]);
     zIm[2] = addsp_i(aIm[2],bIm[2]);   zIm[12] = subsp_i(aIm[2],bIm[2]);
                                                  
     zRe[3] = addsp_i(aRe[3],bRe[3]);   zRe[13] = subsp_i(aRe[3],bRe[3]);
     zIm[3] = addsp_i(aIm[3],bIm[3]);   zIm[13] = subsp_i(aIm[3],bIm[3]);
                                                  
     zRe[4] = addsp_i(aRe[4],bRe[4]);   zRe[14] = subsp_i(aRe[4],bRe[4]);
     zIm[4] = addsp_i(aIm[4],bIm[4]);   zIm[14] = subsp_i(aIm[4],bIm[4]);
                                                  
     zRe[5] = addsp_i(aRe[5],bRe[5]);   zRe[15] = subsp_i(aRe[5],bRe[5]);
     zIm[5] = addsp_i(aIm[5],bIm[5]);   zIm[15] = subsp_i(aIm[5],bIm[5]);
                                                  
     zRe[6] = addsp_i(aRe[6],bRe[6]);   zRe[16] = subsp_i(aRe[6],bRe[6]);
     zIm[6] = addsp_i(aIm[6],bIm[6]);   zIm[16] = subsp_i(aIm[6],bIm[6]);
                                                  
     zRe[7] = addsp_i(aRe[7],bRe[7]);   zRe[17] = subsp_i(aRe[7],bRe[7]);
     zIm[7] = addsp_i(aIm[7],bIm[7]);   zIm[17] = subsp_i(aIm[7],bIm[7]);
                                       
     zRe[8] = addsp_i(aRe[8],bRe[8]);   zRe[18] = subsp_i(aRe[8],bRe[8]);
     zIm[8] = addsp_i(aIm[8],bIm[8]);   zIm[18] = subsp_i(aIm[8],bIm[8]);
                                       
     zRe[9] = addsp_i(aRe[9],bRe[9]);   zRe[19] = subsp_i(aRe[9],bRe[9]);
     zIm[9] = addsp_i(aIm[9],bIm[9]);   zIm[19] = subsp_i(aIm[9],bIm[9]);

 }

 /// @brief                            
 ///    构造factor3*factor2*factor1复合基逆序映射序列
 /// @param[out]    
 ///    perTab          按照factor3*factor2*factor1复合基逆序映射数组
 ///    factor3         第3维基底
 ///    factor2         第2维基底
 ///    factor1         第1维基底
 /// @return
 ///    void
 ///
 void compBaseSeq(Uint16 perTab[], Uint16 factor3, Uint16 factor2, Uint16 factor1)
 {
     int i,j,k,m;

     for(j= 0;j<factor2;j++){

         for(i = 0;i<factor1;i++){
             perTab[i+j*factor1] = (i *factor3*factor2)+j*(factor3);
         }
     }


     for(k = 1;k<factor3;k++){

         for(m=0 ;m<factor2*factor1;m++){

             perTab[factor2*factor1*k+m]  = perTab[m]+k;
         }
     }
 }
 ///
 /// @brief
 ///     对原始数据用按时间抽取
 /// @param[in] 
 ///    perTab      按照factor3*factor2*factor1复合基逆序映射数组
 ///    tabSize     perTab数组大小
 ///    xRe         采样点实数序列
 ///    xIm         采样点虚数序列
 /// @param[out]
 ///    yRe         抽取后的实数序列
 ///    yIm         抽取后的虚数序列
 /// @return
 ///    void
 ///
void permute(Uint16 perTab[], Int16 tabSize, const float xRe[], const float xIm[],
             float yRe[],float yIm[])
{
 Uint16 i;

 for (i = 0; i < tabSize; i++) 
 {
     yRe[i] = xRe[perTab[i]];                   /*HANNING[i];*/
     yIm[i] = xIm[perTab[i]];                   /*HANNING[i];*/
 }
}  
///
/// @brief
///    
/// @param[in] 
///    sofarRadix       二维基
///    radix            FFT基
///    remainRadix      剩余基
/// @param[out]
///    yRe              FFT后的实数序列
///    yIm              FFT后的虚数序列
/// @return
///    void
///
void twiddleTransf(Uint16 sofarRadix, Uint16 radix, 
                          Uint16 remainRadix,float yRe[], float yIm[])
{  
    float  cosw, sinw, gem;

    omega = divsp_i(PI2,uintsp_i(sofarRadix*radix));
    cosw =  cosf(omega);
    sinw = -sinf(omega);	
    tw_re = 1.0;
    tw_im = 0.0;
    dataOffset=0;
    groupOffset=dataOffset;
    adr=groupOffset;

    // 进行3维FFT计算
    for (dataNo=0; dataNo<sofarRadix; dataNo++)
    {
        // 计算旋转因子
        if (sofarRadix>1)
        {
            twiddleRe[0] = 1.0; 
            twiddleIm[0] = 0.0;
            twiddleRe[1] = tw_re;
            twiddleIm[1] = tw_im;
            for (twNo=2; twNo<radix; twNo++)
            {
                twiddleRe[twNo] = subsp_i(mpysp_i(tw_re,twiddleRe[twNo-1]),
                    mpysp_i(tw_im,twiddleIm[twNo-1]));
                twiddleIm[twNo] = addsp_i(mpysp_i(tw_im,twiddleRe[twNo-1]),
                    mpysp_i(tw_re,twiddleIm[twNo-1]));
            }
            gem   = subsp_i(mpysp_i(cosw,tw_re),mpysp_i(sinw,tw_im));
            tw_im = addsp_i(mpysp_i(sinw,tw_re),mpysp_i(cosw,tw_im));
            tw_re = gem;
        }

        // 计算2维FFT
        for (groupNo=0; groupNo<remainRadix; groupNo++)
        {
            // 乘以旋转因子
            if ((sofarRadix>1) && (dataNo > 0))
            {
                zRe[0]=yRe[adr];
                zIm[0]=yIm[adr];
                blockNo=1;
                do
                {
                    adr = addsp_i(adr,sofarRadix);
                    zRe[blockNo]= subsp_i(mpysp_i(twiddleRe[blockNo],yRe[adr]),
                        mpysp_i(twiddleIm[blockNo],yIm[adr]));
                    zIm[blockNo]= addsp_i(mpysp_i(twiddleRe[blockNo],yIm[adr]),
                        mpysp_i(twiddleIm[blockNo],yRe[adr])); 
                    
                    blockNo++;
                } while (blockNo < radix);
            }
            else
            {
                for (blockNo=0; blockNo<radix; blockNo++)
                {
                   zRe[blockNo]=yRe[adr];
                   zIm[blockNo]=yIm[adr];
                   adr=adr+sofarRadix;
                }
            }

            // 进行特定基的FFT计算，支持基4，基5，基8和基10
            switch(radix)
            {
                case  4  : fft_4(zRe,zIm);        break;
                case  5  : fft_5(zRe,zIm);        break;
                case  8  : fft_8(zRe,zIm);		  break;
                case 10  : fft_10(zRe,zIm);		  break;
                case 16  : fft_16(zRe,zIm);       break;
                case 20  : fft_20(zRe,zIm);       break;
                default  : fft_8(zRe,zIm);		  break;
            }
            adr=groupOffset;
            for (blockNo=0; blockNo<radix; blockNo++)
            {
                yRe[adr]=zRe[blockNo]; yIm[adr]=zIm[blockNo];
                adr=adr+sofarRadix;
            }
            groupOffset=groupOffset+sofarRadix*radix;
            adr=groupOffset;
        }
        dataOffset=dataOffset+1;
        groupOffset=dataOffset;
        adr=groupOffset;
    }
}
