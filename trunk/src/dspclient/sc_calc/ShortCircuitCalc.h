///
/// @file
/// 	ShortCircuitCalc.h
/// @note
/// 	短路计算
/// @author
///		chao
/// @copyright
///		2016 XuanTong Inc.
/// @time
/// 	2013/07/30
///
#ifndef _SHORT_CIRCUIT_CALC_H_
#define _SHORT_CIRCUIT_CALC_H_

#include <math.h>
#include <complex>
using std::complex;
using std::polar;
using std::abs;
using std::arg;

class SCCalc
{    
public:    
    //////////////////////////////////////////////////////////////////////////
    /// 短路计算结构体定义
    //////////////////////////////////////////////////////////////////////////

    ///< 短路类型
    enum SCType
    {
        A_GROUND,                                   ///< A相接地短路
        B_GROUND,                                   ///< B相接地短路
        C_GROUND,                                   ///< C相接地短路
        AB_SHORT_CIRCUIT,                           ///< AB相短路
        BC_SHORT_CIRCUIT,                           ///< BC相短路
        CA_SHORT_CIRCUIT,                           ///< AC相短路
        AB_GROUND,                                  ///< AB相接地短路
        BC_GROUND,                                  ///< BC相接地短路
        CA_GROUND,                                  ///< CA相接地短路
        ABC_SHORT_CIRCUIT,                          ///< 三相短路
        SINGLE_PHASE_SYSTEM                         ///< 单相系统
    };
    ///< 计算模型类型
    enum CalcModelType
    {
        CURRENT_FIXED,                              ///< 电流不变
        VOLTAGE_FIXED,                              ///< 电压不变
        SYSTEM_IMPEDANCE_FIXED                      ///< 系统阻抗不变
    };
    ///< 短路方向
    enum DirectionType
    {
        POSITIVE,                                   ///< 正向
        NEGATIVE                                    ///< 反向
    };
    struct Vector
    {
        double      md;                             ///< 模（module）
        double      dg;                             ///< 角度（degree）
    };
    ///< 计算模型
    struct CalcModel
    {
        CalcModelType    type;                      ///< 计算模型类型
        union
        {
            double           cv;                    ///< 电流/电压设置值
            Vector           zs;                    ///< ZS向量设置值
        }value;                                     ///< 设置值
    };
    ///< 计算结果
    struct Result
    {
        Vector ABCVoltage[3];                       ///< 三相电压值
        Vector ABCCurrent[3];                       ///< 三相电流值
    };
    ///< 零序补偿系数计算类型
    enum KLCalcType
    {
        KLCT_KL,                                    ///< 输入KL的幅值和相角
        KLCT_KRKX,                                  ///< 输入KR,KX的值
        KLCT_Z0Z1,                                  ///< 输入Z0/Z1的幅值和相角
    };
    ///< 阻抗计算类型
    enum ZLCalcType
    {
        ZLCT_ZPHI,                                  ///< 输入Z,Phi的值
        ZLCT_RX                                     ///< 输入R,X的值
    };

    //////////////////////////////////////////////////////////////////////////
    /// 常量定义
    //////////////////////////////////////////////////////////////////////////
    static const double PI;

    //////////////////////////////////////////////////////////////////////////
    /// 短路计算
    //////////////////////////////////////////////////////////////////////////

    ///    
    /// @method:    Calc
    /// @access:    public static 
    /// @brief
    ///     短路计算
    /// @param[in] 
    ///     type                短路类型
    ///     Ue                  额定电压
    ///     direction           故障方向
    ///     cm                  计算模型
    ///     zl                  线路正序阻抗
    ///     zs                  系统阻抗零序补偿系数
    ///     kl                  线路阻抗零序补偿系数
    ///     lc                  负荷电流
    /// @param[out]
    ///     rs                  计算结果
    /// @return
    ///     int                 0：成功
    ///                        -1：参数错误
    ///                        -2：基本短路计算结果错误（如电压大于额定电压）
    ///
    static int Calc(SCType type, double Ue, DirectionType direction, CalcModel cm, Vector zl, Vector ks, Vector kl, Vector lc, Result &rs);

    //////////////////////////////////////////////////////////////////////////
    /// 参数计算
    //////////////////////////////////////////////////////////////////////////

    ///
    /// @method:    ZLCalc
    /// @access:    public static 
    /// @brief:
    ///		线路阻抗计算
    /// @param[in]: 
    ///		type					线路正序阻抗求解类型
    ///		v1					    值1
    ///		v2					    值2
    /// @param[out]:
    ///		zl					    阻抗计算结果
    /// @return:   
    ///     void
    ///
    static void ZLCalc(ZLCalcType type, double v1, double v2, Vector &zl);

    ///
    /// @method:    KLCalc
    /// @access:    public static 
    /// @brief
    ///     零序补偿系数计算
    /// @param[in]
    ///     type                零序补偿系数求解类型
    ///     v1                  值1
    ///     v2                  值2
    ///     zl                  线路正序阻抗
    /// @param[out]
    ///     kl                  统一输出向量形式的零序补偿系数
    /// @return
    ///     void
    ///
    static void KLCalc(KLCalcType type, double v1, double v2, Vector zl,
        Vector &kl);

    //////////////////////////////////////////////////////////////////////////
    /// 内联函数
    //////////////////////////////////////////////////////////////////////////

    // 从结果集返回A相电压引用
    static inline Vector &_Va(Result &rs)
    {
        return rs.ABCVoltage[0];
    }
    // 从结果集返回B相电压引用
    static inline Vector &_Vb(Result &rs)
    {
        return rs.ABCVoltage[1];
    }
    // 从结果集返回C相电压引用
    static inline Vector &_Vc(Result &rs)
    {
        return rs.ABCVoltage[2];
    }
    // 从结果集返回A相电流引用
    static inline Vector &_Ia(Result &rs)
    {
        return rs.ABCCurrent[0];
    }
    // 从结果集返回B相电流引用
    static inline Vector &_Ib(Result &rs)
    {
        return rs.ABCCurrent[1];
    }
    // 从结果集返回C相电流引用
    static inline Vector &_Ic(Result &rs)
    {
        return rs.ABCCurrent[2];
    }

    ///
    /// @method:    SetVector
    /// @access:    public static 
    /// @brief:
    ///		设置向量值
    /// @param[out]: 
    ///		v						要设置的向量
    /// @param[in]:
    ///		md						向量的模
    ///		dg						向量的角度
    /// @return:   
    ///		void				
    ///
    static inline void Set(Vector &v, double md, double dg)
    {
        v.md = md;
        v.dg = dg;
    }
private:    

    //////////////////////////////////////////////////////////////////////////
    /// 短路计算
    //////////////////////////////////////////////////////////////////////////

    ///
    /// @method:    P1_GCalc
    /// @access:    private static 
    /// @brief:
    ///		单相接地
    /// @param[in]: 
    ///		type						短路类型
    ///     Ue                          额定电压
    ///		cm						    计算模型
    ///		zl	                        线路正序阻抗
    ///     ks                          系统阻抗零序补偿系数
    ///     kl                          线路阻抗零序补偿系数
    /// @param[out]:
    ///		rs						    计算结果
    /// @return:   
    ///		void
    ///
    static void P1_GCalc(SCType type, double Ue, CalcModel cm, Vector zl, 
        Vector ks, Vector kl, Result &rs);

    ///
    /// @method:    P2_SCCalc
    /// @access:    private static 
    /// @brief:
    ///		两相短路
    /// @param[in]: 
    ///		type					短路类型
    ///     Ue                      额定电压
    ///		cm						计算模型
    ///		zl						线路正序阻抗
    /// @param[out]:
    ///		rs						计算结果
    /// @return:   
    ///     void
    ///
    static void P2_SCCalc(SCType type, double Ue, CalcModel cm, Vector zl,
        Result &rs);

    ///
    /// @method:    P2_SCGCalc
    /// @access:    private static 
    /// @brief:
    ///		两相接地短路
    /// @param[in]:
    ///     type                    短路类型
    ///     Ue                      额定电压
    ///		cm						计算模型
    ///		zl						线路正序阻抗
    /// @param[out]:
    ///		rs						计算结果
    /// @return:   
    ///		void			
    ///
    static void P2_GSCCalc(SCType type, double Ue, CalcModel cm, Vector zl, 
        Vector ks, Vector kl, Result &rs);

    ///
    /// @method:    P3_SCCalc
    /// @access:    private static 
    /// @brief:
    ///		三相短路
    /// @param[in]: 
    ///     Ue                      额定电压
    ///		cm						计算模型
    ///		zl						线路正序阻抗
    /// @param[out]:
    ///		rs						计算结果
    /// @return:   
    ///		void		
    ///
    static void P3_SCCalc(double Ue, CalcModel cm, Vector zl, Result &rs);

    ///
    /// @method:    P1_SysCalc
    /// @access:    private static 
    /// @brief:
    ///		单相系统
    /// @param[in]: 
    ///		Ue                      额定电压
    ///		cm						计算模型
    ///		zl						线路正序阻抗
    /// @param[out]:
    ///		rs						计算结果
    /// @return:   
    ///		void	
    ///
    static void P1_SysCalc(double Ue, CalcModel cm, Vector zl,Result &rs);


    ///
    /// @method:    Lc_EffCalc
    /// @access:    private static 
    /// @brief:
    ///		负荷电流叠加影响计算
    /// @param[in]: 
    ///		lc						负荷电流，负荷功角组成的向量
    /// @param[inout]:
    ///		rs						短路计算结果
    /// @return:   
    ///		void
    ///
    static void Lc_EffCalc(Vector lc, Result &rs);

    ///
    /// @method:    CheckBasicResult
    /// @access:    private static 
    /// @brief:
    ///		检查基本短路计算结果的正确性（不包括另需补偿系数和负荷电流的影响）
    /// @param[in]: 
    ///		rs					基本短路计算结果
    ///     Ue                  额定电压
    /// @return:   
    ///		int				    0：结果正确
    ///                         -1：电压的模大于额定电压
    ///
    static int CheckResult(Result rs, double Ue);

    ///
    /// @method:    CheckInputParam
    /// @access:    private static 
    /// @brief:
    ///		检测输入参数的正确性
    /// @param[in]: 
    ///		cm						计算模型
    ///		zl						线路正序阻抗
    ///     ks                      系统阻抗零序补偿系数
    ///		kl						线路阻抗零序补偿系数
    ///		lc						负荷电流
    /// @return:   
    ///		int					    0：正确
    ///                            -1：计算模型的电流/电压/系统阻抗比，线路阻抗
    ///                                零序补偿系数和负荷电流的模小于0，错误
    ///
    static int CheckInputParam(CalcModel cm, Vector zl, Vector ks, Vector kl,
        Vector lc);

    ///
    /// @method:    DegreeAdjust
    /// @access:    private static 
    /// @brief:
    ///		相位角调整（将相位角调整到-180°~180°的范围内）
    /// @param[inout]: 
    ///		rs						短路计算结果
    /// @return:   
    ///		void					
    ///
    static void DegreeAdjust(Result &rs);

    ///
    /// @method:    DirectionAdjust
    /// @access:    private static 
    /// @brief:
    ///		故障方向调整（反向故障要反向计算出来的电流方向）
    /// @param[inout]: 
    ///		rs						短路计算结果
    ///     direction               故障方向
    /// @return:   
    ///		void					
    ///
    static void DirectionAdjust(Result &rs, DirectionType direction);

    ///
    /// @method:    KLCalc
    /// @access:    public static 
    /// @brief:
    ///		KR,KX方式计算零序补偿系数
    /// @param[in]: 
    ///		kr						KR
    ///		kx						KX
    ///		zl						线路正序阻抗
    /// @param[out]:
    ///		kl						零序补偿系数
    /// @return:   
    ///     void
    ///
    static void KLCalc(double kr, double kx, Vector zl, Vector &kl);

    ///
    /// @method:    KLCalc
    /// @access:    public static 
    /// @brief:
    ///		Z0/Z1方式计算零序补偿系数
    /// @param[in]: 
    ///		z0z1						Z0/Z1（向量）
    /// @param[out]:
    ///		kl						    零序补偿系数
    /// @return:   
    ///		void				
    ///
    static void KLCalc(Vector z0z1, Vector &kl);

    ///
    /// @method:    Convert
    /// @access:    private static
    /// @brief:
    ///		向量转复数
    /// @param[in]: 
    ///		v					    要转换的向量
    /// @return:   
    ///		complex<double>         转换后的复数
    ///
    static inline complex<double> v2c(Vector v)
    {        
        complex<double> c(polar(v.md, d2r(v.dg)));
        return c;
    }

    ///
    /// @method:    c2v
    /// @access:    private static
    /// @brief:
    ///		复数转向量
    /// @param[in]: 
    ///		c					    要转换的复数
    /// @return:   
    ///		SCCalc::Vector          转换后的向量
    ///
    static inline Vector c2v(complex<double> c)
    {        
        Vector v = {abs(c), r2d(arg(c))};
        return v;
    }

    ///
    /// @method:    r2d
    /// @access:    private static 
    /// @brief:
    ///		弧度转角度（radian to degree）
    /// @param[]: 
    ///		r						弧度
    /// @return:   
    ///		double			        转换后的角度
    ///
    static inline double r2d(double r)
    {
        return r/PI*180;
    }

    ///
    /// @method:    d2r
    /// @access:    private static 
    /// @brief:
    ///		角度转弧度（degree to radian）
    /// @param[]: 
    ///		a						角度
    /// @return:   
    ///		double			        转换后的弧度
    ///
    static inline double d2r(double a)
    {
        return a/180*PI;
    }
};
#endif