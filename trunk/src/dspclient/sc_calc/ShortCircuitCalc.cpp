#include "stdafx.h"
#include "ShortCircuitCalc.h"

const double SCCalc::PI = 3.14159265358979323846;
// 短路计算
int SCCalc::Calc( SCType type, double Ue, DirectionType direction, CalcModel cm,
                 Vector zl, Vector ks, Vector kl, Vector lc, Result &rs )
{
    if(CheckInputParam(cm, zl, ks, kl, lc) != 0)
    {
        return -1;
    }
    switch(type)
    {
        // 单相接地短路
    case A_GROUND:
    case B_GROUND:
    case C_GROUND:
        {
            P1_GCalc(type, Ue, cm, zl, ks, kl, rs);
        }
        break;
        // 两相短路
    case AB_SHORT_CIRCUIT:
    case BC_SHORT_CIRCUIT:
    case CA_SHORT_CIRCUIT:
        {
            P2_SCCalc(type, Ue, cm, zl, rs);
        }
        break;
        // 两相接地短路
    case AB_GROUND:
    case BC_GROUND:
    case CA_GROUND:
        {
            P2_GSCCalc(type, Ue, cm, zl, ks, kl, rs);
        }
        break;
        // 三相短路
    case ABC_SHORT_CIRCUIT:
        {
            P3_SCCalc(Ue, cm, zl, rs);
        }
        break;
        // 单相系统
    case SINGLE_PHASE_SYSTEM:
        {
            P1_SysCalc(Ue, cm, zl, rs);
        }
        break;
    }
    
    // 方向调整
    DirectionAdjust(rs, direction);

    // 叠加负荷电流的影响
    Lc_EffCalc(lc, rs);

    // 相位角调整
    DegreeAdjust(rs);

    // 检查短路计算结果
    if(CheckResult(rs, Ue) != 0)
    {
        return -2;
    }
    return 0;
}

// 线路阻抗计算
void SCCalc::ZLCalc(ZLCalcType type, double v1, double v2, Vector &zl)
{        
    switch(type)
    {
    case ZLCT_ZPHI:
        {
            Set(zl, v1, v2);	            
        }
        break;
    case ZLCT_RX:
        {
            complex<double> cpxZl(v1,v2);
            zl = c2v(cpxZl);
        }
        break;
    }    
}

// 零序补偿系数计算
void SCCalc::KLCalc(KLCalcType type, double v1, double v2, Vector z1, Vector &kl)
{
    switch(type)
    {
    case KLCT_KL:
        {
            Set(kl, v1, v2);	            
        }
        break;
    case KLCT_KRKX:
        {
            KLCalc(v1,v2,z1,kl);
        }
        break;
    case KLCT_Z0Z1:
        {
            Vector z0z1={v1,v2};            
            KLCalc(z0z1, kl);
        }            
        break;
    }    
}

// KL计算(kr,kx)
void SCCalc::KLCalc(double kr, double kx, Vector zl, Vector &kl)
{
    complex<double> ckl(kr*cosf(d2r(zl.dg)), kx*sinf(d2r(zl.dg)));
    kl = c2v(ckl);
}

// KL计算（z0/z1）
void SCCalc::KLCalc(Vector z0z1, Vector &kl)
{
    Set(kl, z0z1.md * 1.0/3.0-1.0/3.0, z0z1.dg);        
}

// 单相接地
void SCCalc::P1_GCalc( SCType type, double Ue, CalcModel cm, Vector zl, Vector ks, Vector kl, Result &rs )
{    
    int rsMapSeq[3];

    complex<double> cE[3];
    cE[0]=polar(Ue, d2r(0));
    cE[1]=polar(Ue, d2r(-120));
    cE[2]=polar(Ue, d2r(120));

    complex<double> cZl=v2c(zl);
    complex<double> cKs=v2c(ks);
    complex<double> cKl=v2c(kl);

    // 用于存储三相电压电流的复数临时数组变量
    complex<double> cU[3];
    complex<double> cI[3];

    // 根据短路类型确定映射序列
    // rsMapSeq[0]：接地相；rsMapSeq[1], rsMapSeq[2]：非接地相
    switch(type)
    {
    case A_GROUND:
        {
            rsMapSeq[0]=0;
            rsMapSeq[1]=1;
            rsMapSeq[2]=2;
        }
        break;
    case B_GROUND:
        {
            rsMapSeq[0]=1;
            rsMapSeq[1]=2;
            rsMapSeq[2]=0;
        }
        break;
    case C_GROUND:
        {
            rsMapSeq[0]=2;
            rsMapSeq[1]=0;
            rsMapSeq[2]=1;
        }
        break;
    }

    switch(cm.type)
    {
    case CURRENT_FIXED:
        {            
            // 短路相电流相位
            double scPhi=arg(cE[rsMapSeq[0]])-arg((1.0+cKl)*cZl);
            Set(rs.ABCCurrent[rsMapSeq[0]], cm.value.cv, r2d(scPhi));
            Set(rs.ABCCurrent[rsMapSeq[1]], 0, 0);
            Set(rs.ABCCurrent[rsMapSeq[2]], 0, 0);

            // 短路相电流
            cI[rsMapSeq[0]] = v2c(rs.ABCCurrent[rsMapSeq[0]]);

            // 短路相电压
            cU[rsMapSeq[0]] = cI[rsMapSeq[0]]*(1.0+cKl)*cZl;
            rs.ABCVoltage[rsMapSeq[0]] = c2v(cU[rsMapSeq[0]]);
            rs.ABCVoltage[rsMapSeq[1]] = c2v(cE[rsMapSeq[1]]);
            rs.ABCVoltage[rsMapSeq[2]] = c2v(cE[rsMapSeq[2]]);
        }
        break;
    case VOLTAGE_FIXED:
        {
            // 短路相电流相位
            double scPhi=arg(cE[rsMapSeq[0]])-arg((1.0+cKl)*cZl);

            // 短路相电流大小
            double scMod=cm.value.cv/abs((1.0+cKl)*cZl);

            Set(rs.ABCCurrent[rsMapSeq[0]], scMod, r2d(scPhi));
            Set(rs.ABCCurrent[rsMapSeq[1]], 0, 0);
            Set(rs.ABCCurrent[rsMapSeq[2]], 0, 0);

            // 短路相电压
            Set(rs.ABCVoltage[rsMapSeq[0]], cm.value.cv, r2d(arg(cE[rsMapSeq[0]])));
            rs.ABCVoltage[rsMapSeq[1]] = c2v(cE[rsMapSeq[1]]);
            rs.ABCVoltage[rsMapSeq[2]] = c2v(cE[rsMapSeq[2]]);
        }
        break;
    case SYSTEM_IMPEDANCE_FIXED:
        {            
            complex<double> cZs = v2c(cm.value.zs);

            // 短路相电流
            cI[rsMapSeq[0]] = cE[rsMapSeq[0]]/((1.0+cKs)*cZs+(1.0+cKl)*cZl);

            rs.ABCCurrent[rsMapSeq[0]]=c2v(cI[rsMapSeq[0]]);
            Set(rs.ABCCurrent[rsMapSeq[1]], 0, 0);
            Set(rs.ABCCurrent[rsMapSeq[2]], 0, 0);

            // 短路相电压
            cU[rsMapSeq[0]] = cI[rsMapSeq[0]]*(1.0+cKl)*cZl;
            rs.ABCVoltage[rsMapSeq[0]]=c2v(cU[rsMapSeq[0]]);

            // 其他相电压
            cU[rsMapSeq[1]] = cE[rsMapSeq[1]]-cI[rsMapSeq[0]]*cKs*cZs;
            cU[rsMapSeq[2]] = cE[rsMapSeq[2]]-cI[rsMapSeq[0]]*cKs*cZs;
            rs.ABCVoltage[rsMapSeq[1]]=c2v(cU[rsMapSeq[1]]);
            rs.ABCVoltage[rsMapSeq[2]]=c2v(cU[rsMapSeq[2]]);
        }
        break;
    }
}

// 两相短路
void SCCalc::P2_SCCalc( SCType type, double Ue, CalcModel cm, Vector zl, Result &rs )
{
    int rsMapSeq[3];

    complex<double> cE[3];
    cE[0]=polar(Ue, d2r(0));
    cE[1]=polar(Ue, d2r(-120));
    cE[2]=polar(Ue, d2r(120));

    complex<double> cZl=v2c(zl);    

    // 用于存储三相电压电流的复数临时数组变量
    complex<double> cU[3];
    complex<double> cI[3];

    // 根据短路类型确定映射序列
    // rsMapSeq[0]，rsMapSeq[1]：短路相 ；rsMapSeq[2]：非短路相
    switch(type)
    {
    case AB_SHORT_CIRCUIT:
        {
            rsMapSeq[0]=0;
            rsMapSeq[1]=1;
            rsMapSeq[2]=2;
        }
        break;
    case BC_SHORT_CIRCUIT:
        {
            rsMapSeq[0]=1;
            rsMapSeq[1]=2;
            rsMapSeq[2]=0;
        }
        break;
    case CA_SHORT_CIRCUIT:
        {
            rsMapSeq[0]=2;
            rsMapSeq[1]=0;
            rsMapSeq[2]=1;
        }
        break;
    }
    switch(cm.type)
    {
    case CURRENT_FIXED:
        {
            // 短路相1电流
            double scPhi = arg(cE[rsMapSeq[0]]-cE[rsMapSeq[1]])-arg(cZl);
            Set(rs.ABCCurrent[rsMapSeq[0]], cm.value.cv, r2d(scPhi));
            cI[rsMapSeq[0]] = v2c(rs.ABCCurrent[rsMapSeq[0]]);

            // 短路相2电流(-短路相1电流)
            cI[rsMapSeq[1]] = -cI[rsMapSeq[0]];
            rs.ABCCurrent[rsMapSeq[1]] = c2v(cI[rsMapSeq[1]]);

            // 非短路相电流
            Set(rs.ABCCurrent[rsMapSeq[2]], 0, 0);

            cI[rsMapSeq[0]] = v2c(rs.ABCCurrent[rsMapSeq[0]]);
            cI[rsMapSeq[1]] = v2c(rs.ABCCurrent[rsMapSeq[1]]);
            cU[rsMapSeq[0]] = (cE[rsMapSeq[0]]+cE[rsMapSeq[1]])/2.0+
                cI[rsMapSeq[0]]*cZl;
            cU[rsMapSeq[1]] = (cE[rsMapSeq[0]]+cE[rsMapSeq[1]])/2.0+
                cI[rsMapSeq[1]]*cZl;

            rs.ABCVoltage[rsMapSeq[0]] = c2v(cU[rsMapSeq[0]]);
            rs.ABCVoltage[rsMapSeq[1]] = c2v(cU[rsMapSeq[1]]);
            rs.ABCVoltage[rsMapSeq[2]] = c2v(cE[rsMapSeq[2]]);
        }
        break;
    case VOLTAGE_FIXED:
        {
            // 短路相1电流
            double scPhi = arg(cE[rsMapSeq[0]]-cE[rsMapSeq[1]])-arg(cZl);
            double scMod = abs(cm.value.cv/(2.0*cZl));
            Set(rs.ABCCurrent[rsMapSeq[0]], scMod, r2d(scPhi));
            cI[rsMapSeq[0]] = v2c(rs.ABCCurrent[rsMapSeq[0]]);

            // 短路相2电流（-短路相1电流）
            cI[rsMapSeq[1]] = -cI[rsMapSeq[0]];
            rs.ABCCurrent[rsMapSeq[1]] = c2v(cI[rsMapSeq[1]]);

            // 非短路相电流
            Set(rs.ABCCurrent[rsMapSeq[2]], 0, 0);

            cI[rsMapSeq[0]] = v2c(rs.ABCCurrent[rsMapSeq[0]]);
            cI[rsMapSeq[1]] = v2c(rs.ABCCurrent[rsMapSeq[1]]);
            cU[rsMapSeq[0]] = (cE[rsMapSeq[0]]+cE[rsMapSeq[1]])/2.0
                +cI[rsMapSeq[0]]*cZl;
            cU[rsMapSeq[1]] = (cE[rsMapSeq[0]]+cE[rsMapSeq[1]])/2.0
                +cI[rsMapSeq[1]]*cZl;
            rs.ABCVoltage[rsMapSeq[0]] = c2v(cU[rsMapSeq[0]]);
            rs.ABCVoltage[rsMapSeq[1]] = c2v(cU[rsMapSeq[1]]);
            rs.ABCVoltage[rsMapSeq[2]] = c2v(cE[rsMapSeq[2]]);
        }
        break;
    case SYSTEM_IMPEDANCE_FIXED:
        {            
            complex<double> cZs = v2c(cm.value.zs);

            // 短路相电流
            cI[rsMapSeq[0]] = (cE[rsMapSeq[0]] - cE[rsMapSeq[1]])/(2.0*(cZs+cZl));
            cI[rsMapSeq[1]] = -cI[rsMapSeq[0]];
            rs.ABCCurrent[rsMapSeq[0]] = c2v(cI[rsMapSeq[0]]);
            rs.ABCCurrent[rsMapSeq[1]] = c2v(cI[rsMapSeq[1]]);

            // 非短路相电流
            Set(rs.ABCCurrent[rsMapSeq[2]], 0, 0);

            cU[rsMapSeq[0]] = (cE[rsMapSeq[0]]+cE[rsMapSeq[1]])/2.0
                +cI[rsMapSeq[0]]*cZl;
            cU[rsMapSeq[1]] = (cE[rsMapSeq[0]]+cE[rsMapSeq[1]])/2.0
                +cI[rsMapSeq[1]]*cZl;
            
            // 三相电压
            rs.ABCVoltage[rsMapSeq[0]] = c2v(cU[rsMapSeq[0]]);
            rs.ABCVoltage[rsMapSeq[1]] = c2v(cU[rsMapSeq[1]]);
            rs.ABCVoltage[rsMapSeq[2]] = c2v(cE[rsMapSeq[2]]);
        }
        break;
    }
}

// 两相短路接地
void SCCalc::P2_GSCCalc( SCType type, double Ue, CalcModel cm, Vector zl, Vector ks, Vector kl, Result &rs )
{
    int rsMapSeq[3];

    // 设置ABC三相额定电压
    complex<double> cE[3];
    cE[0]=polar(Ue, d2r(0));
    cE[1]=polar(Ue, d2r(-120));
    cE[2]=polar(Ue, d2r(120));

    complex<double> cZl=v2c(zl);
    complex<double> cKs=v2c(ks);
    complex<double> cKl=v2c(kl);

    // 用于存储三相电压电流的复数临时数组变量
    complex<double> cU[3];
    complex<double> cI[3];

    // 根据短路类型确定映射序列
    // rsMapSeq[0]，rsMapSeq[1]：短路相 ；rsMapSeq[2]：非短路相
    switch(type)
    {
    case AB_GROUND:
        {
            rsMapSeq[0]=0;
            rsMapSeq[1]=1;
            rsMapSeq[2]=2;
        }
        break;
    case BC_GROUND:
        {
            rsMapSeq[0]=1;
            rsMapSeq[1]=2;
            rsMapSeq[2]=0;
        }
        break;
    case CA_GROUND:
        {
            rsMapSeq[0]=2;
            rsMapSeq[1]=0;
            rsMapSeq[2]=1;
        }
        break;
    }
    switch(cm.type)
    {
    case CURRENT_FIXED:
        {
            // 短路相1电流
            double scPhi1 = 
                arg(
                (cE[rsMapSeq[0]]+cKl*(cE[rsMapSeq[0]]-cE[rsMapSeq[1]])) 
                    / (1.0+2.0*cKl)
                )
                -d2r(zl.dg);
            Set(rs.ABCCurrent[rsMapSeq[0]], cm.value.cv, r2d(scPhi1));
            cI[rsMapSeq[0]] = v2c(rs.ABCCurrent[rsMapSeq[0]]);

            // 短路相2电流
            double scPhi2 = 
                arg(
                (cE[rsMapSeq[1]]-cKl*(cE[rsMapSeq[0]]-cE[rsMapSeq[1]])) 
                    / (1.0+2.0*cKl)
                )
                -d2r(zl.dg);
            Set(rs.ABCCurrent[rsMapSeq[1]], cm.value.cv, r2d(scPhi2));
            cI[rsMapSeq[1]] = v2c(rs.ABCCurrent[rsMapSeq[1]]);

            // 非短路相电流
            Set(rs.ABCCurrent[rsMapSeq[2]], 0, 0);

            cU[rsMapSeq[0]] = cZl*cI[rsMapSeq[0]] 
                + cKl*cZl*(cI[rsMapSeq[0]]+ cI[rsMapSeq[1]]);
            cU[rsMapSeq[1]] = cZl*cI[rsMapSeq[1]] 
                + cKl*cZl*(cI[rsMapSeq[0]]+ cI[rsMapSeq[1]]);

            // 三相电压
            rs.ABCVoltage[rsMapSeq[0]] = c2v(cU[rsMapSeq[0]]);
            rs.ABCVoltage[rsMapSeq[1]] = c2v(cU[rsMapSeq[1]]);
            rs.ABCVoltage[rsMapSeq[2]] = c2v(cE[rsMapSeq[2]]);
        }
        break;
    case VOLTAGE_FIXED:
        {
            // 短路相电压
            cU[rsMapSeq[0]] = polar(cm.value.cv, arg(cE[rsMapSeq[0]]));
            cU[rsMapSeq[1]] = polar(cm.value.cv, arg(cE[rsMapSeq[1]]));
            
            // I1+I2
            complex<double> cSumI = (cU[rsMapSeq[0]] + cU[rsMapSeq[1]])
                / ((1.0+2.0*cKl)*cZl);
            cI[rsMapSeq[0]] = (cU[rsMapSeq[0]] 
                - cKl*cZl*cSumI) / cZl;
            cI[rsMapSeq[1]] = (cU[rsMapSeq[1]] 
                - cKl*cZl*cSumI) / cZl;

            // 短路相电流
            rs.ABCCurrent[rsMapSeq[0]] = c2v(cI[rsMapSeq[0]]);
            rs.ABCCurrent[rsMapSeq[1]] = c2v(cI[rsMapSeq[1]]);

            // 非短路相电流
            Set(rs.ABCCurrent[rsMapSeq[2]], 0, 0);

            rs.ABCVoltage[rsMapSeq[0]] = c2v(cU[rsMapSeq[0]]);
            rs.ABCVoltage[rsMapSeq[1]] = c2v(cU[rsMapSeq[1]]);
            rs.ABCVoltage[rsMapSeq[2]] = c2v(cE[rsMapSeq[2]]);
        }
        break;
    case SYSTEM_IMPEDANCE_FIXED:
        {
            complex<double> cZs = v2c(cm.value.zs);
            complex<double> cK0 = (cKs*cZs + cKl*cZl)/(cZs+cZl);
            complex<double> cZf = (1.0+2.0*cKs)*cZs+(1.0+2.0*cKl)*cZl;

            // 短路相电流
            cI[rsMapSeq[0]] = (cE[rsMapSeq[0]] 
                + cK0*(cE[rsMapSeq[0]]-cE[rsMapSeq[1]]))/cZf;
            cI[rsMapSeq[1]] = (cE[rsMapSeq[1]] 
                - cK0*(cE[rsMapSeq[0]]-cE[rsMapSeq[1]]))/cZf;
            rs.ABCCurrent[rsMapSeq[0]] = c2v(cI[rsMapSeq[0]]);
            rs.ABCCurrent[rsMapSeq[1]] = c2v(cI[rsMapSeq[1]]);

            // 非短路相电流
            Set(rs.ABCCurrent[rsMapSeq[2]], 0, 0);

            // 短路相电压
            cU[rsMapSeq[0]] = cZl*cI[rsMapSeq[0]]
                + cKl*cZl*(cI[rsMapSeq[0]]+cI[rsMapSeq[1]]);
            cU[rsMapSeq[1]] = cZl*cI[rsMapSeq[1]]
                + cKl*cZl*(cI[rsMapSeq[0]]+cI[rsMapSeq[1]]);

            // 非短路相电压
            cU[rsMapSeq[2]] = cE[rsMapSeq[2]]
                - cKs*cZs*(cI[rsMapSeq[0]]+cI[rsMapSeq[1]]);

            rs.ABCVoltage[rsMapSeq[0]] = c2v(cU[rsMapSeq[0]]);
            rs.ABCVoltage[rsMapSeq[1]] = c2v(cU[rsMapSeq[1]]);
            rs.ABCVoltage[rsMapSeq[2]] = c2v(cU[rsMapSeq[2]]);
        }
        break;
    }    
}

// 三相短路
void SCCalc::P3_SCCalc( double Ue, CalcModel cm, Vector zl, Result &rs )
{
    // 设置ABC三相额定电压
    complex<double> cE[3];
    cE[0]=polar(Ue, d2r(0));
    cE[1]=polar(Ue, d2r(-120));
    cE[2]=polar(Ue, d2r(120));

    complex<double> cZl=v2c(zl);    

    // 用于存储三相电压电流的复数临时数组变量
    complex<double> cU[3];
    complex<double> cI[3];

    switch(cm.type)
    {
    case CURRENT_FIXED:
        {
            // Ia, Ib, Ic
            cI[0] = polar(cm.value.cv, arg(cE[0])-arg(cZl));
            cI[1] = polar(cm.value.cv, arg(cE[1])-arg(cZl));
            cI[2] = polar(cm.value.cv, arg(cE[2])-arg(cZl));
            _Ia(rs) = c2v(cI[0]);
            _Ib(rs) = c2v(cI[1]);
            _Ic(rs) = c2v(cI[2]);

            // Va, Vb, Vc
            cU[0] = cI[0]*cZl;
            cU[1] = cI[1]*cZl;
            cU[2] = cI[2]*cZl;
            _Va(rs) = c2v(cU[0]);
            _Vb(rs) = c2v(cU[1]);
            _Vc(rs) = c2v(cU[2]);
        }
        break;
    case VOLTAGE_FIXED:
        {
            // Ia, Ib, Ic
            cI[0] = polar(cm.value.cv/abs(cZl), arg(cE[0]) - arg(cZl));
            cI[1] = polar(cm.value.cv/abs(cZl), arg(cE[1]) - arg(cZl));
            cI[2] = polar(cm.value.cv/abs(cZl), arg(cE[2]) - arg(cZl));
            _Ia(rs) = c2v(cI[0]);
            _Ib(rs) = c2v(cI[1]);
            _Ic(rs) = c2v(cI[2]);

            // Va, Vb, Vc
            cU[0] = cI[0]*cZl;
            cU[1] = cI[1]*cZl;
            cU[2] = cI[2]*cZl;
            _Va(rs) = c2v(cU[0]);
            _Vb(rs) = c2v(cU[1]);
            _Vc(rs) = c2v(cU[2]);
        }
        break;
    case SYSTEM_IMPEDANCE_FIXED:
        {
            complex<double> cZs = v2c(cm.value.zs);

            // Ia, Ib, Ic
            cI[0] = cE[0]/(cZs+cZl);
            cI[1] = cE[1]/(cZs+cZl);
            cI[2] = cE[2]/(cZs+cZl);
            _Ia(rs) = c2v(cI[0]);
            _Ib(rs) = c2v(cI[1]);
            _Ic(rs) = c2v(cI[2]);

            // Va, Vb, Vc
            cU[0] = cI[0]*cZl;
            cU[1] = cI[1]*cZl;
            cU[2] = cI[2]*cZl;
            _Va(rs) = c2v(cU[0]);
            _Vb(rs) = c2v(cU[1]);
            _Vc(rs) = c2v(cU[2]);
        }
        break;        
    }
}

// 单相系统
void SCCalc::P1_SysCalc( double Ue, CalcModel cm, Vector zl,Result &rs )
{
    // 设置ABC三相额定电压
    complex<double> cE[3];
    cE[0]=polar(Ue, d2r(0));
    cE[1]=polar(Ue, d2r(-120));
    cE[2]=polar(Ue, d2r(120));

    complex<double> cZl=v2c(zl);    

    // 用于存储三相电压电流的复数临时数组变量
    complex<double> cU[3];
    complex<double> cI[3];

    switch(cm.type)
    {
    case CURRENT_FIXED:
        {
            // Ia
            cI[0] = polar(cm.value.cv, arg(cE[0])-arg(cZl));
            _Ia(rs) = c2v(cI[0]);
            Set(_Ib(rs), 0, 0);
            Set(_Ic(rs), 0, 0);

            // Va
            cU[0] = cI[0]*cZl;
            _Va(rs) = c2v(cU[0]);
            Set(_Vb(rs), 0, 0);
            Set(_Vc(rs), 0, 0);
        }
        break;
    case VOLTAGE_FIXED:
        {
            // Ia
            cI[0] = polar(cm.value.cv/abs(cZl), arg(cE[0]) - arg(cZl));
            _Ia(rs) = c2v(cI[0]);
            Set(_Ib(rs), 0, 0);
            Set(_Ic(rs), 0, 0);

            // Va
            cU[0] = cI[0]*cZl;
            _Va(rs) = c2v(cU[0]);
            Set(_Vb(rs), 0, 0);
            Set(_Vc(rs), 0, 0);
        }
        break;
    case SYSTEM_IMPEDANCE_FIXED:
        {
            // 无影响
            Set(_Ia(rs), 0, 0);
            Set(_Ib(rs), 0, 0);
            Set(_Ic(rs), 0, 0);
                       
            Set(_Va(rs), 0, 0);
            Set(_Vb(rs), 0, 0);
            Set(_Vc(rs), 0, 0);
        }
        break;
    }
}

// 基本短路计算结果检查
int SCCalc::CheckResult( Result rs ,double Ue)
{
    for(int i=0; i<3; i++)
    {            
        if(rs.ABCVoltage[i].md > Ue)
        {
            return -1;
        }
    }
    return 0;
}

// 检测输入参数的正确性
int SCCalc::CheckInputParam( CalcModel cm, Vector zl, Vector ks, Vector kl, Vector lc )
{
    switch(cm.type)
    {
    case CURRENT_FIXED:
    case VOLTAGE_FIXED:
        if(cm.value.cv < 0)
        {
            return -1;
        }
        break;
    case SYSTEM_IMPEDANCE_FIXED:
        if(cm.value.zs.md < 0)
        {
            return -1;
        }
    }
    if(zl.md < 0 ||ks.md < 0 || kl.md < 0 || lc.md < 0)
    {
        return -1;
    }
    return 0;
}

// 相位角调整
void SCCalc::DegreeAdjust( Result &rs)
{
    for(int i=0; i<3; i++)
    {
        while(rs.ABCCurrent[i].dg > 180)
        {
            rs.ABCCurrent[i].dg -= 360;
        }
        while(rs.ABCCurrent[i].dg < -180)
        {
            rs.ABCCurrent[i].dg += 360;
        }
        while(rs.ABCVoltage[i].dg > 180)
        {
            rs.ABCVoltage[i].dg -= 360;
        }
        while(rs.ABCVoltage[i].dg < -180)
        {
            rs.ABCVoltage[i].dg += 360;
        }         
    }
}

void SCCalc::DirectionAdjust( Result &rs, DirectionType direction )
{
    // 先将相位角调整到-180°~180°的范围内
    DegreeAdjust(rs);

    // 如果是反向故障则电流取反
    if(direction == NEGATIVE)
    {
        for(int i=0; i<3; i++)
        {
            if(rs.ABCCurrent[i].md != 0)
            {
                if(rs.ABCCurrent[i].dg < 0)
                {
                    rs.ABCCurrent[i].dg = 180 + rs.ABCCurrent[i].dg;
                }
                else
                {
                    rs.ABCCurrent[i].dg = rs.ABCCurrent[i].dg - 180;
                }
            }
        }
    }
}

void SCCalc::Lc_EffCalc( Vector lc, Result &rs )
{
    complex<double> cLc[3];
    cLc[0] = polar(lc.md, d2r(0+lc.dg));
    cLc[1] = polar(lc.md, d2r(-120+lc.dg));
    cLc[2] = polar(lc.md, d2r(120+lc.dg));

    complex<double> cI[3];
    cI[0] = v2c(_Ia(rs));
    cI[1] = v2c(_Ib(rs));
    cI[2] = v2c(_Ic(rs));
    
    cI[0] = cI[0]+cLc[0];
    cI[1] = cI[1]+cLc[1];
    cI[2] = cI[2]+cLc[2];

    _Ia(rs) = c2v(cI[0]);
    _Ib(rs) = c2v(cI[1]);
    _Ic(rs) = c2v(cI[2]);
}
