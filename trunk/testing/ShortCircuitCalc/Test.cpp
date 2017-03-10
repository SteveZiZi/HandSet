///
/// @file
/// 	Test.cpp
/// @note
/// 	对短路计算算法的gtest测试用例，对不包含零序补偿系数和负荷电流的短路计算
///     进行测试，结果参考昂立软件ONLLY 系列计算机自动化（继电保护）测试调试系统
///     对于单相系统，结果参考博电软件PowerTest For PNF800，对于负荷电流的影响，
///     参考凯默的DM5000，用MATLAB计算结果验证。由于短路计算算法是分为单相接地，
///     两相短路，两相接地，三相短路，单相系统，所以只需分别对上述每一种短路中一
///     种情况测试零序补偿系数和负荷电流即可
/// @author
///		chao
/// @copyright
///		2016 XuanTong Inc.
/// @time
/// 	2013/08/01
///
#include "stdafx.h"
#include "gtest/gtest.h"

#include <complex>

#include "../../src/dspclient/sc_calc/ShortCircuitCalc.h"
#include "TestMacro.h"


class SC_TEST:public testing::Test
{
protected:
    SCCalc::CalcModel cm;
    SCCalc::Vector zl;
    SCCalc::Vector ks;
    SCCalc::Vector kl;
    SCCalc::Vector lc;
    SCCalc::Result rs;
    double Ue;          ///< 额定电压

    int nRet;
    virtual void SetUp()
    {        
        memset(&cm, 0, sizeof(cm));
        memset(&zl, 0, sizeof(zl));
        memset(&ks, 0, sizeof(ks));
        memset(&kl, 0, sizeof(kl));
        memset(&lc, 0, sizeof(lc));
        Ue = 57.735;
        nRet = 0;
        SCCalc::Set(ks, 0, 0);
    }
    virtual void TearDown()
    {
        // Nothing
    }
};

// A相接地，电流不变
TEST_F(SC_TEST, A_GROUND_CURRENT_TEST)
{    
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 1.0;
    SCCalc::Set(zl, 30, 20);

    SCCalc::Set(ks, 0, 0);
    SCCalc::Set(kl, 0, 0);
    SCCalc::Set(lc, 0, 0);
    
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 30.000, 0.00);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120.00);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);

    EPT_V(SCCalc::_Ia(rs), 1.000, -20.00);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0);
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    // 加入零序补偿系数Kl（ks=0）    
    SCCalc::Set(kl, 1.52, 15.97);    
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);

    // 结果会超出Ue
    ASSERT_EQ(nRet, -2);

    EPT_V(SCCalc::_Va(rs), 74.898, 0.00);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120.00);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);

    EPT_V(SCCalc::_Ia(rs), 1.000, -29.643);
    EPT_V(SCCalc::_Ib(rs), 0, 0);
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    //反向故障
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::NEGATIVE, cm, zl, ks, kl, lc, rs);
    ASSERT_EQ(nRet, -2);

    EPT_V(SCCalc::_Va(rs), 74.898, 0.00);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120.00);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);

    EPT_V(SCCalc::_Ia(rs), 1.000, 150.357);
    EPT_V(SCCalc::_Ib(rs), 0, 0);
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    // 加入负荷电流和负荷功角
    SCCalc::Set(lc, 1.41, 71.42);
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);    

    EPT_V(SCCalc::_Ia(rs), 1.564, 32.562);
    EPT_V(SCCalc::_Ib(rs), 1.410, -48.580);
    EPT_V(SCCalc::_Ic(rs), 1.410, -168.580);    

}

// A相接地，电压不变
TEST_F(SC_TEST, A_GROUND_VOLTAGE_TEST)
{
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 12.3;    
    SCCalc::Set(zl, 2.6, 17.8);
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);
    ASSERT_EQ(nRet, 0);  

    EPT_V(SCCalc::_Va(rs), 12.3, 0.00);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120.00);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);   

    EPT_V(SCCalc::_Ia(rs), 4.731, -17.8);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0); 
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    // 加入零序补偿系数Kl（ks=0）    
    SCCalc::Set(kl, 1.52, 15.97);    
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 12.300, 0.00);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120.00);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);

    EPT_V(SCCalc::_Ia(rs), 1.895, -27.443);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0);
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    // 反向故障
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::NEGATIVE, cm, zl, ks, kl, lc, rs);
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 12.300, 0.00);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120.00);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);

    EPT_V(SCCalc::_Ia(rs), 1.895, 152.557);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0);
    EPT_V(SCCalc::_Ic(rs), 0.000, 0);

    // 加入负荷电流和负荷功角
    SCCalc::Set(lc, 1.41, 71.42);
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);    

    EPT_V(SCCalc::_Ia(rs), 2.181, 12.264);
    EPT_V(SCCalc::_Ib(rs), 1.410, -48.580);
    EPT_V(SCCalc::_Ic(rs), 1.410, -168.580);
}

// A相接地，系统阻抗不变 
TEST_F(SC_TEST, A_GROUND_SYSTEM_IMPEDANCE_TEST)
{    
    cm.type = SCCalc::SYSTEM_IMPEDANCE_FIXED;
    SCCalc::Set(cm.value.zs, 12.3, 10);
    SCCalc::Set(zl, 2.6, 17.8);
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 10.088, 6.441);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120.00);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);    

    EPT_V(SCCalc::_Ia(rs), 3.880, -11.359);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0); 
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    // 加入零序补偿系数Kl,Ks
    SCCalc::Set(kl, 1.52, 15.97);
    SCCalc::Set(ks, 2.84, 71.18);
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);

    // 结果会超出Ue
    ASSERT_EQ(nRet, -2);

    EPT_V(SCCalc::_Va(rs), 8.156, -32.190);
    EPT_V(SCCalc::_Vb(rs), 96.066, -136.506);
    EPT_V(SCCalc::_Vc(rs), 77.490, 154.073);

    EPT_V(SCCalc::_Ia(rs), 1.256, -59.633);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0);
    EPT_V(SCCalc::_Ic(rs), 0.000, 0);

    // 反向故障
    nRet = SCCalc::Calc(SCCalc::A_GROUND, Ue, SCCalc::NEGATIVE, cm, zl, ks, kl, lc, rs);

    // 结果会超出Ue
    ASSERT_EQ(nRet, -2);

    EPT_V(SCCalc::_Va(rs), 8.156, -32.190);
    EPT_V(SCCalc::_Vb(rs), 96.066, -136.506);
    EPT_V(SCCalc::_Vc(rs), 77.490, 154.073);

    EPT_V(SCCalc::_Ia(rs), 1.256, 120.367);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0);
    EPT_V(SCCalc::_Ic(rs), 0.000, 0);

    // 测试2
    SCCalc::Set(cm.value.zs, 60, 60);
    SCCalc::Set(zl, 60, 30);
    SCCalc::Set(kl,0.667, 30);
    SCCalc::Set(ks,0.667, 60);
    nRet = SCCalc::Calc(SCCalc::A_GROUND, 5, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, -2);

    EPT_V(SCCalc::_Va(rs), 2.812, -19.615);
    EPT_V(SCCalc::_Vb(rs), 6.163, -120.293);
    EPT_V(SCCalc::_Vc(rs), 4.562, 132.953);    

    EPT_V(SCCalc::_Ia(rs), 0.029, -61.551);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0); 
    EPT_V(SCCalc::_Ic(rs), 0, 0);

}

// B相接地，电流不变
TEST_F(SC_TEST, B_GROUND_CURRENT_TEST)
{    
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 7.415;
    SCCalc::Set(zl, 6.24, 26.85);
    nRet = SCCalc::Calc(SCCalc::B_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0.00);
    EPT_V(SCCalc::_Vb(rs), 46.270, -120.00);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);    

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 7.415, -146.85); 
    EPT_V(SCCalc::_Ic(rs), 0, 0); 
}

// B相接地，电压不变
TEST_F(SC_TEST, B_GROUND_VOLTAGE_TEST)
{    
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 12.3;    
    SCCalc::Set(zl, 2.6, 17.8);
    nRet = SCCalc::Calc(SCCalc::B_GROUND,Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);      
    ASSERT_EQ(nRet, 0);  

    EPT_V(SCCalc::_Va(rs), 57.735, 0.00);
    EPT_V(SCCalc::_Vb(rs), 12.3, -120.00);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);    

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 4.731, -137.80); 
    EPT_V(SCCalc::_Ic(rs), 0, 0); 
}

// B相接地，系统阻抗不变
TEST_F(SC_TEST, B_GROUND_SYSTEM_IMPEDANCE_TEST)
{    
    cm.type = SCCalc::SYSTEM_IMPEDANCE_FIXED;
    SCCalc::Set(cm.value.zs, 15.8, 31.2);
    SCCalc::Set(zl, 3.95, 14.2);
    nRet = SCCalc::Calc(SCCalc::B_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);      
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0.00);
    EPT_V(SCCalc::_Vb(rs), 11.629, -133.624);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);    

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 2.944, -147.824); 
    EPT_V(SCCalc::_Ic(rs), 0, 0);
}

// C相接地，电流不变
TEST_F(SC_TEST, C_GROUND_CURRENT_TEST)
{    
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 12.3;    
    SCCalc::Set(zl, 2.6, 17.8);
    nRet = SCCalc::Calc(SCCalc::C_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);      
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0.00);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120.00);
    EPT_V(SCCalc::_Vc(rs), 31.980, 120.00);    

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0); 
    EPT_V(SCCalc::_Ic(rs), 12.3, 102.2);

    // 结果电压/电流大于线路容量的情况
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 15.8;    
    SCCalc::Set(zl, 5.95, 14.2);
    nRet = SCCalc::Calc(SCCalc::C_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);      
    ASSERT_EQ(nRet, -2);

    // 参数错误的情况
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = -15.8;    
    SCCalc::Set(zl, 5.95, 14.2);
    nRet = SCCalc::Calc(SCCalc::C_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, -1);
}

// C相接地，电压不变
TEST_F(SC_TEST, C_GROUND_VOLTAGE_TEST)
{    
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 12.3;    
    SCCalc::Set(zl, 2.6, 17.8);
    nRet = SCCalc::Calc(SCCalc::C_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);      
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0.00);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120.00);
    EPT_V(SCCalc::_Vc(rs), 12.3, 120.00);    

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0); 
    EPT_V(SCCalc::_Ic(rs), 4.731, 102.2);
}

// C相接地，系统阻抗不变
TEST_F(SC_TEST, C_GROUND_SYSTEM_IMPEDANCE_TEST)
{    
    cm.type = SCCalc::SYSTEM_IMPEDANCE_FIXED;
    SCCalc::Set(cm.value.zs, 12.3, 50.24);
    SCCalc::Set(zl, 2.6, 17.8);
    nRet = SCCalc::Calc(SCCalc::C_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);      
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0.00);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120.00);
    EPT_V(SCCalc::_Vc(rs), 10.309, 93.056);    

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 0.000, 0); 
    EPT_V(SCCalc::_Ic(rs), 3.965, 75.256);
}

// 两相短路Ks,Kl无影响，无需测试
// AB相短路，电流不变
TEST_F(SC_TEST, AB_SHORT_CIRCUIT_CURRENT_TEST)
{   
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 5.251;
    SCCalc::Set(zl, 2.510, 12.98);
    nRet = SCCalc::Calc(SCCalc::AB_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 31.734, -35.46);
    EPT_V(SCCalc::_Vb(rs), 31.734, -84.54);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);    

    EPT_V(SCCalc::_Ia(rs), 5.251, 17.02);
    EPT_V(SCCalc::_Ib(rs), 5.251, -162.98); 
    EPT_V(SCCalc::_Ic(rs), 0, 0); 

    // 反向故障
    nRet = SCCalc::Calc(SCCalc::AB_SHORT_CIRCUIT, Ue, SCCalc::NEGATIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 31.734, -35.46);
    EPT_V(SCCalc::_Vb(rs), 31.734, -84.54);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);    

    EPT_V(SCCalc::_Ia(rs), 5.251, -162.98);
    EPT_V(SCCalc::_Ib(rs), 5.251, 17.020); 
    EPT_V(SCCalc::_Ic(rs), 0, 0);    
}

// AB相短路，电压不变
TEST_F(SC_TEST, AB_SHORT_CIRCUIT_VOLTAGE_TEST)
{    
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 5.251;
    SCCalc::Set(zl, 2.510, 12.98);
    nRet = SCCalc::Calc(SCCalc::AB_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 28.987, -54.803);
    EPT_V(SCCalc::_Vb(rs), 28.987, -65.197);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120);    

    EPT_V(SCCalc::_Ia(rs), 1.046, 17.020);
    EPT_V(SCCalc::_Ib(rs), 1.046, -162.980); 
    EPT_V(SCCalc::_Ic(rs), 0, 0);
}

// AB相短路，系统阻抗不变
TEST_F(SC_TEST, AB_SHORT_CIRCUIT_SYSTEM_IMPEDANCE_TEST)
{    
    cm.type = SCCalc::SYSTEM_IMPEDANCE_FIXED;
    SCCalc::Set(cm.value.zs, 5.251, 23.10);
    SCCalc::Set(zl, 2.510, 12.98);
    nRet = SCCalc::Calc(SCCalc::AB_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 34.762, -32.390);
    EPT_V(SCCalc::_Vb(rs), 31.383, -90.887);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120);

    EPT_V(SCCalc::_Ia(rs), 6.465, 10.169);
    EPT_V(SCCalc::_Ib(rs), 6.465, -169.831);
    EPT_V(SCCalc::_Ic(rs), 0, 0);
}

// BC相短路，电流不变
TEST_F(SC_TEST, BC_SHORT_CIRCUIT_CURRENT_TEST)
{   
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 3.189;
    SCCalc::Set(zl, 9.187, 80.46);
    nRet = SCCalc::Calc(SCCalc::BC_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0);
    EPT_V(SCCalc::_Vb(rs), 41.130, -134.577);
    EPT_V(SCCalc::_Vc(rs), 41.130, 134.577);

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 3.189, -170.460);
    EPT_V(SCCalc::_Ic(rs), 3.189, 9.540);
}

// BC相短路，电压不变
TEST_F(SC_TEST, BC_SHORT_CIRCUIT_VOLTAGE_TEST)
{    
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 3.189;
    SCCalc::Set(zl, 9.187, 80.46);
    nRet = SCCalc::Calc(SCCalc::BC_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0);
    EPT_V(SCCalc::_Vb(rs), 28.912, -176.838);
    EPT_V(SCCalc::_Vc(rs), 28.912, 176.838);

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 0.174, -170.460);
    EPT_V(SCCalc::_Ic(rs), 0.174, 9.540);
}

// BC相短路，系统阻抗不变
TEST_F(SC_TEST, BC_SHORT_CIRCUIT_SYSTEM_IMPEDANCE_TEST)
{    
    cm.type = SCCalc::SYSTEM_IMPEDANCE_FIXED;
    SCCalc::Set(cm.value.zs, 3.189, 80.41);
    SCCalc::Set(zl, 9.187, 80.46);
    nRet = SCCalc::Calc(SCCalc::BC_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0);
    EPT_V(SCCalc::_Vb(rs), 47.016, -127.866);
    EPT_V(SCCalc::_Vc(rs), 47.026, 127.882);

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 4.040, -170.447);
    EPT_V(SCCalc::_Ic(rs), 4.040, 9.553);
}

// CA相短路，电流不变
TEST_F(SC_TEST, CA_SHORT_CIRCUIT_CURRENT_TEST)
{    
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 8.542;
    SCCalc::Set(zl, 6.541, 87.15);
    nRet = SCCalc::Calc(SCCalc::CA_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   

    // 结果会超出Ue
    ASSERT_EQ(nRet, -2);

    EPT_V(SCCalc::_Va(rs), 62.890, -2.676);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120);
    EPT_V(SCCalc::_Vc(rs), 62.890, 122.676);

    EPT_V(SCCalc::_Ia(rs), 8.542, -117.150);
    EPT_V(SCCalc::_Ib(rs), 0, 0);
    EPT_V(SCCalc::_Ic(rs), 8.542, 62.850);
}

// CA相短路，电压不变
TEST_F(SC_TEST, CA_SHORT_CIRCUIT_VOLTAGE_TEST)
{    
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 8.542;
    SCCalc::Set(zl, 6.541, 87.15);
    nRet = SCCalc::Calc(SCCalc::CA_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 29.182, 51.584);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120);
    EPT_V(SCCalc::_Vc(rs), 29.182, 68.416);

    EPT_V(SCCalc::_Ia(rs), 0.653, -117.150);
    EPT_V(SCCalc::_Ib(rs), 0, 0);
    EPT_V(SCCalc::_Ic(rs), 0.653, 62.850);
}

// CA相短路，系统阻抗不变
TEST_F(SC_TEST, CA_SHORT_CIRCUIT_SYSTEM_IMPEDANCE_TEST)
{    
    cm.type = SCCalc::SYSTEM_IMPEDANCE_FIXED;
    SCCalc::Set(cm.value.zs, 8.542,150.58);
    SCCalc::Set(zl, 6.541, 87.15);
    nRet = SCCalc::Calc(SCCalc::CA_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 24.664, 3.999);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120);
    EPT_V(SCCalc::_Vc(rs), 48.468, 84.953);

    EPT_V(SCCalc::_Ia(rs), 3.884, -153.552);
    EPT_V(SCCalc::_Ib(rs), 0, 0);
    EPT_V(SCCalc::_Ic(rs), 3.884, 26.448);
}

// AB相接地短路，电流不变
TEST_F(SC_TEST, AB_GROUND_CURRENT_TEST)
{    
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 8.189;
    SCCalc::Set(zl, 3.541, 87.15);
    nRet = SCCalc::Calc(SCCalc::AB_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 28.997, 0);
    EPT_V(SCCalc::_Vb(rs), 28.997, -120);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120);

    EPT_V(SCCalc::_Ia(rs), 8.189, -87.15);
    EPT_V(SCCalc::_Ib(rs), 8.189, 152.85);
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    // 加入零序补偿系数Kl（ks=0）    
    SCCalc::Set(kl, 1.52, 15.97);    
    nRet = SCCalc::Calc(SCCalc::AB_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 35.807, 3.452);
    EPT_V(SCCalc::_Vb(rs), 29.891, -118.167);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);

    EPT_V(SCCalc::_Ia(rs), 8.189, -65.408);
    EPT_V(SCCalc::_Ib(rs), 8.189, 130.633);
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    // 反向故障
    SCCalc::Set(kl, 1.52, 15.97);    
    nRet = SCCalc::Calc(SCCalc::AB_GROUND, Ue, SCCalc::NEGATIVE, cm, zl, ks, kl, lc, rs);
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 35.807, 3.452);
    EPT_V(SCCalc::_Vb(rs), 29.891, -118.167);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);

    EPT_V(SCCalc::_Ia(rs), 8.189, 114.592);
    EPT_V(SCCalc::_Ib(rs), 8.189, -49.367);
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    // ZL为0的时候
    Ue = 100;
    cm.value.cv = 40;
    SCCalc::Set(kl, 0.667, 0);
    SCCalc::Set(lc, 0, 0);
    SCCalc::Set(zl, 0, 60); 
    nRet = SCCalc::Calc(SCCalc::AB_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 0, 0.00);
    EPT_V(SCCalc::_Vb(rs), 0, 0);
    EPT_V(SCCalc::_Vc(rs), 100, 120.00);

    EPT_V(SCCalc::_Ia(rs), 40, -43.894);
    EPT_V(SCCalc::_Ib(rs), 40, 163.894);
    EPT_V(SCCalc::_Ic(rs), 0, 0);
}

// AB相接地短路，电压不变
TEST_F(SC_TEST, AB_GROUND_VOLTAGE_TEST)
{    
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 8.189;
    SCCalc::Set(zl, 3.541, 87.15);
    nRet = SCCalc::Calc(SCCalc::AB_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 8.189, 0);
    EPT_V(SCCalc::_Vb(rs), 8.189, -120);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120);

    EPT_V(SCCalc::_Ia(rs), 2.313, -87.15);
    EPT_V(SCCalc::_Ib(rs), 2.313, 152.85);
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    // 加入零序补偿系数Kl（ks=0）    
    SCCalc::Set(kl, 1.52, 15.97);    
    nRet = SCCalc::Calc(SCCalc::AB_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 8.189, 0.00);
    EPT_V(SCCalc::_Vb(rs), 8.189, -120.00);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120.00);

    EPT_V(SCCalc::_Ia(rs), 1.963, -65.408);
    EPT_V(SCCalc::_Ib(rs), 2.082, 130.633);
    EPT_V(SCCalc::_Ic(rs), 0, 0);
}

// AB相接地短路，系统阻抗不变
TEST_F(SC_TEST, AB_GROUND_SYSTEM_IMPEDANCE_TEST)
{    
    cm.type = SCCalc::SYSTEM_IMPEDANCE_FIXED;
    SCCalc::Set(cm.value.zs, 8.189, 52.15);
    SCCalc::Set(zl, 3.541, 87.15);
    nRet = SCCalc::Calc(SCCalc::AB_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 18.134, 24.621);
    EPT_V(SCCalc::_Vb(rs), 18.134, -95.379);
    EPT_V(SCCalc::_Vc(rs), 57.735, 120);

    EPT_V(SCCalc::_Ia(rs), 5.121, -62.529);
    EPT_V(SCCalc::_Ib(rs), 5.121, 177.471);
    EPT_V(SCCalc::_Ic(rs), 0, 0);

    // 加入零序补偿系数Kl,Ks
    SCCalc::Set(kl, 1.52, 15.97);
    SCCalc::Set(ks, 0.84, 71.18);
    nRet = SCCalc::Calc(SCCalc::AB_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);

    // 结果会超出Ue
    ASSERT_EQ(nRet, -2);

    EPT_V(SCCalc::_Va(rs), 15.670, 6.955);
    EPT_V(SCCalc::_Vb(rs), 23.856, -96.329);
    EPT_V(SCCalc::_Vc(rs), 69.076, 124.275);

    EPT_V(SCCalc::_Ia(rs), 3.976, -42.991);
    EPT_V(SCCalc::_Ib(rs), 5.012, 155.755);
    EPT_V(SCCalc::_Ic(rs), 0, 0);
}

// BC相接地短路，电流不变
TEST_F(SC_TEST, BC_GROUND_CURRENT_TEST)
{ 
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 10.887;
    SCCalc::Set(zl, 2.842, -54.12);
    nRet = SCCalc::Calc(SCCalc::BC_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0);
    EPT_V(SCCalc::_Vb(rs), 30.941, -120);
    EPT_V(SCCalc::_Vc(rs), 30.941, 120);

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 10.887, -65.88);
    EPT_V(SCCalc::_Ic(rs), 10.887, 174.12);
}

// BC相接地短路，电压不变
TEST_F(SC_TEST, BC_GROUND_VOLTAGE_TEST)
{    
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 10.887;
    SCCalc::Set(zl, 2.842, -54.12);
    nRet = SCCalc::Calc(SCCalc::BC_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0);
    EPT_V(SCCalc::_Vb(rs), 10.887, -120);
    EPT_V(SCCalc::_Vc(rs), 10.887, 120);

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 3.831, -65.880);
    EPT_V(SCCalc::_Ic(rs), 3.831, 174.120);
}

// BC相接地短路，系统阻抗不变
TEST_F(SC_TEST, BC_GROUND_SYSTEM_IMPEDANCE_TEST)
{    
    cm.type = SCCalc::SYSTEM_IMPEDANCE_FIXED;
    SCCalc::Set(cm.value.zs, 10.887, -20.95);
    SCCalc::Set(zl, 2.842, -54.12);
    nRet = SCCalc::Calc(SCCalc::BC_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 57.735, 0);
    EPT_V(SCCalc::_Vb(rs), 12.285, -146.485);
    EPT_V(SCCalc::_Vc(rs), 12.285, 93.515);

    EPT_V(SCCalc::_Ia(rs), 0, 0);
    EPT_V(SCCalc::_Ib(rs), 4.323, -92.365);
    EPT_V(SCCalc::_Ic(rs), 4.323, 147.635);
}

// CA相接地短路，电流不变
TEST_F(SC_TEST, CA_GROUND_CURRENT_TEST)
{
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 9.155;
    SCCalc::Set(zl, 4.841, -34.92);
    nRet = SCCalc::Calc(SCCalc::CA_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 44.319, 0);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120);
    EPT_V(SCCalc::_Vc(rs), 44.319, 120);

    EPT_V(SCCalc::_Ia(rs), 9.155, 34.92);
    EPT_V(SCCalc::_Ib(rs), 0, 0);
    EPT_V(SCCalc::_Ic(rs), 9.155, 154.92);
}

// CA相接地短路，电压不变
TEST_F(SC_TEST, CA_GROUND_VOLTAGE_TEST)
{    
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 9.155;
    SCCalc::Set(zl, 4.841, -34.92);
    nRet = SCCalc::Calc(SCCalc::CA_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 9.155, 0);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120);
    EPT_V(SCCalc::_Vc(rs), 9.155, 120);

    EPT_V(SCCalc::_Ia(rs), 1.891, 34.92);
    EPT_V(SCCalc::_Ib(rs), 0, 0);
    EPT_V(SCCalc::_Ic(rs), 1.891, 154.92);
}

// CA相接地短路，系统阻抗不变
TEST_F(SC_TEST, CA_GROUND_IMPEDANCE_TEST)
{    
    cm.type = SCCalc::SYSTEM_IMPEDANCE_FIXED;
    SCCalc::Set(cm.value.zs, 5.152, 135.84);
    SCCalc::Set(zl, 4.841, 34.92);
    nRet = SCCalc::Calc(SCCalc::CA_GROUND, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 43.903, -52.619);
    EPT_V(SCCalc::_Vb(rs), 57.735, -120);
    EPT_V(SCCalc::_Vc(rs), 43.903, 67.381);

    EPT_V(SCCalc::_Ia(rs), 9.069, -87.539);
    EPT_V(SCCalc::_Ib(rs), 0, 0);
    EPT_V(SCCalc::_Ic(rs), 9.069, 32.461);
}

// 两相短路Ks,Kl无影响，无需测试
// 三相短路，电流不变
TEST_F(SC_TEST, THREE_SHORT_CIRCUIT_CURRENT_TEST)
{    
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 13.941;
    SCCalc::Set(zl, 2.584, 150.54);
    nRet = SCCalc::Calc(SCCalc::ABC_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 36.024, 0);
    EPT_V(SCCalc::_Vb(rs), 36.024, -120);
    EPT_V(SCCalc::_Vc(rs), 36.024, 120);

    EPT_V(SCCalc::_Ia(rs), 13.941, -150.54);
    EPT_V(SCCalc::_Ib(rs), 13.941, 89.46);
    EPT_V(SCCalc::_Ic(rs), 13.941, -30.54);    
}

// 三相短路，电压不变
TEST_F(SC_TEST, THREE_SHORT_CIRCUIT_VOLTAGE_TEST)
{    
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 13.941;
    SCCalc::Set(zl, 2.584, 150.54);
    nRet = SCCalc::Calc(SCCalc::ABC_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 13.941, 0);
    EPT_V(SCCalc::_Vb(rs), 13.941, -120);
    EPT_V(SCCalc::_Vc(rs), 13.941, 120);

    EPT_V(SCCalc::_Ia(rs), 5.395, -150.54);
    EPT_V(SCCalc::_Ib(rs), 5.395, 89.46);
    EPT_V(SCCalc::_Ic(rs), 5.395, -30.54);
}

// 三相短路，系统阻抗不变
TEST_F(SC_TEST, THREE_SHORT_CIRCUIT_SYSTEM_IMPEDANCE_TEST)
{    
    cm.type = SCCalc::SYSTEM_IMPEDANCE_FIXED;
    SCCalc::Set(cm.value.zs, 13.941, 68.15);
    SCCalc::Set(zl, 2.584, 150.54);
    nRet = SCCalc::Calc(SCCalc::ABC_SHORT_CIRCUIT, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 10.281, 72.224);
    EPT_V(SCCalc::_Vb(rs), 10.281, -47.776);
    EPT_V(SCCalc::_Vc(rs), 10.281, -167.776);

    EPT_V(SCCalc::_Ia(rs), 3.979, -78.316);
    EPT_V(SCCalc::_Ib(rs), 3.979, 161.684);
    EPT_V(SCCalc::_Ic(rs), 3.979, 41.684);
}

// 单相系统Ks,Kl无影响，无需测试
// 单相系统，电流不变
TEST_F(SC_TEST, SINGLE_SYSTEM_CURRENT_TEST)
{
    cm.type = SCCalc::CURRENT_FIXED;
    cm.value.cv = 9.548;
    SCCalc::Set(zl, 4.512, 41.53);
    nRet = SCCalc::Calc(SCCalc::SINGLE_PHASE_SYSTEM, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 43.081, 0);
    EPT_V(SCCalc::_Vb(rs), 0, 0);
    EPT_V(SCCalc::_Vc(rs), 0, 0);

    EPT_V(SCCalc::_Ia(rs), 9.548, -41.53);
    EPT_V(SCCalc::_Ib(rs), 0, 0);
    EPT_V(SCCalc::_Ic(rs), 0, 0);
}

// 单相系统，电压不变
TEST_F(SC_TEST, SINGLE_SYSTEM_VOLTAGE_TEST)
{
    cm.type = SCCalc::VOLTAGE_FIXED;
    cm.value.cv = 9.548;
    SCCalc::Set(zl, 4.512, 41.53);
    nRet = SCCalc::Calc(SCCalc::SINGLE_PHASE_SYSTEM, Ue, SCCalc::POSITIVE, cm, zl, ks, kl, lc, rs);   
    ASSERT_EQ(nRet, 0);

    EPT_V(SCCalc::_Va(rs), 9.548, 0);
    EPT_V(SCCalc::_Vb(rs), 0, 0);
    EPT_V(SCCalc::_Vc(rs), 0, 0);

    EPT_V(SCCalc::_Ia(rs), 2.116, -41.53);
    EPT_V(SCCalc::_Ib(rs), 0, 0);
    EPT_V(SCCalc::_Ic(rs), 0, 0);
}

// 单相系统，电压不变
TEST_F(SC_TEST, KL_TEST)
{
    SCCalc::Vector zl;
    SCCalc::Vector kl;
    SCCalc::Set(zl, 100, 30);
    SCCalc::KLCalc(SCCalc::KLCT_KRKX, 28, 13, zl, kl);

    // MATLAB计算结果
    EPT_V(kl, 25.1047804212664, 15.0056818504451);
}