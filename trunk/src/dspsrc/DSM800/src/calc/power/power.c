/// @file
///     power.cpp
/// @brief
///     功率计算    
/// @note
///     
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/02/22
/// @revise
/// 
/*******************************************************************************
功率算法说明：
    在交流电路中，由电源供给负载的电功率有两种；一种是有功功率，一种是无功功率。
1) 有功功率
    定义：在交流电路中，电源在一个周期内发出瞬时功率的平均值(或负载电阻所消耗的
  功率)，称为"有功功率"。有功功率的符号用P表示，单位有瓦(W)、千瓦(kW)、兆瓦(MW)。

    有功功率是保持用电设备正常运行所需的电功率，也就是将电能转换为其他形式能量
  (机械能、光能、热能)的电功率。比如：5.5千瓦的电动机就是把5.5千瓦的电能转换为机
  械能，带动水泵抽水或脱粒机脱粒；各种照明设备将电能转换为光能，供人们生活和工作
  照明。
    有功功率过低导致线损增加、容量下降、设备使用率下降，从而导致电能浪费加大。

2) 无功功率
    定义：在具有电感或电容的电路中，在每半个周期内，把电源能量变成磁场(或电场)能
  量贮存起来，然后，再释放，又把贮存的磁场（或电场）能量再返回给电源，只是进行这
  种能量的交换，并没有真正消耗能量，我们把这个交换的功率值，称为" 无功功率"。无
  功功率的符号用Q表示，单位为乏(Var)或千乏(kVar)。

3) 视在功率
    在具有电阻和电抗的电路内，电压与电流的乘积叫视在功率，以字母S或符号 Pｓ表示，
  单位为千伏安（kVA）。

4) 公式
    S=UI=sqrt(P^2+Q^2)
    P=Scosφ=UIcosφ
    Q=Ssinφ=UIsinφ
*******************************************************************************/
#include "power.h"

///
/// @brief
///    功率计算
/// @param[in] 
///    pEffValue    0：电压有效值，1：电流有效值
///    pPhaseAngle  0：电压基波相位角，1：电流基波相位角
/// @param[out]
///    pPower       0：视在功率，1：有功功率，2：无功功率
/// @return
///    void
///
void powerCalc(float pEffValues[2], float pPhaseAngles[2], float pPowers[3], 
			   float pPowerFactors[2])
{
    float dPhaseAngle = pPhaseAngles[0]-pPhaseAngles[1];
    float dPhase = mpysp_i(dPhaseAngle,ANGLE2ARC_FACTOR);
	pPowerFactors[0] = cosf(dPhase);
	pPowerFactors[1] = sinf(dPhase);
    pPowers[0]=mpysp_i(pEffValues[0],pEffValues[1]);
    pPowers[1]=mpysp_i(pPowers[0],cosf(dPhase));
    pPowers[2]=mpysp_i(pPowers[0],sinf(dPhase));
}
