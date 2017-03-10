///
/// @file
/// 	ShortCircuitCalc.h
/// @note
/// 	��·����
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
    /// ��·����ṹ�嶨��
    //////////////////////////////////////////////////////////////////////////

    ///< ��·����
    enum SCType
    {
        A_GROUND,                                   ///< A��ӵض�·
        B_GROUND,                                   ///< B��ӵض�·
        C_GROUND,                                   ///< C��ӵض�·
        AB_SHORT_CIRCUIT,                           ///< AB���·
        BC_SHORT_CIRCUIT,                           ///< BC���·
        CA_SHORT_CIRCUIT,                           ///< AC���·
        AB_GROUND,                                  ///< AB��ӵض�·
        BC_GROUND,                                  ///< BC��ӵض�·
        CA_GROUND,                                  ///< CA��ӵض�·
        ABC_SHORT_CIRCUIT,                          ///< �����·
        SINGLE_PHASE_SYSTEM                         ///< ����ϵͳ
    };
    ///< ����ģ������
    enum CalcModelType
    {
        CURRENT_FIXED,                              ///< ��������
        VOLTAGE_FIXED,                              ///< ��ѹ����
        SYSTEM_IMPEDANCE_FIXED                      ///< ϵͳ�迹����
    };
    ///< ��·����
    enum DirectionType
    {
        POSITIVE,                                   ///< ����
        NEGATIVE                                    ///< ����
    };
    struct Vector
    {
        double      md;                             ///< ģ��module��
        double      dg;                             ///< �Ƕȣ�degree��
    };
    ///< ����ģ��
    struct CalcModel
    {
        CalcModelType    type;                      ///< ����ģ������
        union
        {
            double           cv;                    ///< ����/��ѹ����ֵ
            Vector           zs;                    ///< ZS��������ֵ
        }value;                                     ///< ����ֵ
    };
    ///< ������
    struct Result
    {
        Vector ABCVoltage[3];                       ///< �����ѹֵ
        Vector ABCCurrent[3];                       ///< �������ֵ
    };
    ///< ���򲹳�ϵ����������
    enum KLCalcType
    {
        KLCT_KL,                                    ///< ����KL�ķ�ֵ�����
        KLCT_KRKX,                                  ///< ����KR,KX��ֵ
        KLCT_Z0Z1,                                  ///< ����Z0/Z1�ķ�ֵ�����
    };
    ///< �迹��������
    enum ZLCalcType
    {
        ZLCT_ZPHI,                                  ///< ����Z,Phi��ֵ
        ZLCT_RX                                     ///< ����R,X��ֵ
    };

    //////////////////////////////////////////////////////////////////////////
    /// ��������
    //////////////////////////////////////////////////////////////////////////
    static const double PI;

    //////////////////////////////////////////////////////////////////////////
    /// ��·����
    //////////////////////////////////////////////////////////////////////////

    ///    
    /// @method:    Calc
    /// @access:    public static 
    /// @brief
    ///     ��·����
    /// @param[in] 
    ///     type                ��·����
    ///     Ue                  ���ѹ
    ///     direction           ���Ϸ���
    ///     cm                  ����ģ��
    ///     zl                  ��·�����迹
    ///     zs                  ϵͳ�迹���򲹳�ϵ��
    ///     kl                  ��·�迹���򲹳�ϵ��
    ///     lc                  ���ɵ���
    /// @param[out]
    ///     rs                  ������
    /// @return
    ///     int                 0���ɹ�
    ///                        -1����������
    ///                        -2��������·�������������ѹ���ڶ��ѹ��
    ///
    static int Calc(SCType type, double Ue, DirectionType direction, CalcModel cm, Vector zl, Vector ks, Vector kl, Vector lc, Result &rs);

    //////////////////////////////////////////////////////////////////////////
    /// ��������
    //////////////////////////////////////////////////////////////////////////

    ///
    /// @method:    ZLCalc
    /// @access:    public static 
    /// @brief:
    ///		��·�迹����
    /// @param[in]: 
    ///		type					��·�����迹�������
    ///		v1					    ֵ1
    ///		v2					    ֵ2
    /// @param[out]:
    ///		zl					    �迹������
    /// @return:   
    ///     void
    ///
    static void ZLCalc(ZLCalcType type, double v1, double v2, Vector &zl);

    ///
    /// @method:    KLCalc
    /// @access:    public static 
    /// @brief
    ///     ���򲹳�ϵ������
    /// @param[in]
    ///     type                ���򲹳�ϵ���������
    ///     v1                  ֵ1
    ///     v2                  ֵ2
    ///     zl                  ��·�����迹
    /// @param[out]
    ///     kl                  ͳһ���������ʽ�����򲹳�ϵ��
    /// @return
    ///     void
    ///
    static void KLCalc(KLCalcType type, double v1, double v2, Vector zl,
        Vector &kl);

    //////////////////////////////////////////////////////////////////////////
    /// ��������
    //////////////////////////////////////////////////////////////////////////

    // �ӽ��������A���ѹ����
    static inline Vector &_Va(Result &rs)
    {
        return rs.ABCVoltage[0];
    }
    // �ӽ��������B���ѹ����
    static inline Vector &_Vb(Result &rs)
    {
        return rs.ABCVoltage[1];
    }
    // �ӽ��������C���ѹ����
    static inline Vector &_Vc(Result &rs)
    {
        return rs.ABCVoltage[2];
    }
    // �ӽ��������A���������
    static inline Vector &_Ia(Result &rs)
    {
        return rs.ABCCurrent[0];
    }
    // �ӽ��������B���������
    static inline Vector &_Ib(Result &rs)
    {
        return rs.ABCCurrent[1];
    }
    // �ӽ��������C���������
    static inline Vector &_Ic(Result &rs)
    {
        return rs.ABCCurrent[2];
    }

    ///
    /// @method:    SetVector
    /// @access:    public static 
    /// @brief:
    ///		��������ֵ
    /// @param[out]: 
    ///		v						Ҫ���õ�����
    /// @param[in]:
    ///		md						������ģ
    ///		dg						�����ĽǶ�
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
    /// ��·����
    //////////////////////////////////////////////////////////////////////////

    ///
    /// @method:    P1_GCalc
    /// @access:    private static 
    /// @brief:
    ///		����ӵ�
    /// @param[in]: 
    ///		type						��·����
    ///     Ue                          ���ѹ
    ///		cm						    ����ģ��
    ///		zl	                        ��·�����迹
    ///     ks                          ϵͳ�迹���򲹳�ϵ��
    ///     kl                          ��·�迹���򲹳�ϵ��
    /// @param[out]:
    ///		rs						    ������
    /// @return:   
    ///		void
    ///
    static void P1_GCalc(SCType type, double Ue, CalcModel cm, Vector zl, 
        Vector ks, Vector kl, Result &rs);

    ///
    /// @method:    P2_SCCalc
    /// @access:    private static 
    /// @brief:
    ///		�����·
    /// @param[in]: 
    ///		type					��·����
    ///     Ue                      ���ѹ
    ///		cm						����ģ��
    ///		zl						��·�����迹
    /// @param[out]:
    ///		rs						������
    /// @return:   
    ///     void
    ///
    static void P2_SCCalc(SCType type, double Ue, CalcModel cm, Vector zl,
        Result &rs);

    ///
    /// @method:    P2_SCGCalc
    /// @access:    private static 
    /// @brief:
    ///		����ӵض�·
    /// @param[in]:
    ///     type                    ��·����
    ///     Ue                      ���ѹ
    ///		cm						����ģ��
    ///		zl						��·�����迹
    /// @param[out]:
    ///		rs						������
    /// @return:   
    ///		void			
    ///
    static void P2_GSCCalc(SCType type, double Ue, CalcModel cm, Vector zl, 
        Vector ks, Vector kl, Result &rs);

    ///
    /// @method:    P3_SCCalc
    /// @access:    private static 
    /// @brief:
    ///		�����·
    /// @param[in]: 
    ///     Ue                      ���ѹ
    ///		cm						����ģ��
    ///		zl						��·�����迹
    /// @param[out]:
    ///		rs						������
    /// @return:   
    ///		void		
    ///
    static void P3_SCCalc(double Ue, CalcModel cm, Vector zl, Result &rs);

    ///
    /// @method:    P1_SysCalc
    /// @access:    private static 
    /// @brief:
    ///		����ϵͳ
    /// @param[in]: 
    ///		Ue                      ���ѹ
    ///		cm						����ģ��
    ///		zl						��·�����迹
    /// @param[out]:
    ///		rs						������
    /// @return:   
    ///		void	
    ///
    static void P1_SysCalc(double Ue, CalcModel cm, Vector zl,Result &rs);


    ///
    /// @method:    Lc_EffCalc
    /// @access:    private static 
    /// @brief:
    ///		���ɵ�������Ӱ�����
    /// @param[in]: 
    ///		lc						���ɵ��������ɹ�����ɵ�����
    /// @param[inout]:
    ///		rs						��·������
    /// @return:   
    ///		void
    ///
    static void Lc_EffCalc(Vector lc, Result &rs);

    ///
    /// @method:    CheckBasicResult
    /// @access:    private static 
    /// @brief:
    ///		��������·����������ȷ�ԣ����������貹��ϵ���͸��ɵ�����Ӱ�죩
    /// @param[in]: 
    ///		rs					������·������
    ///     Ue                  ���ѹ
    /// @return:   
    ///		int				    0�������ȷ
    ///                         -1����ѹ��ģ���ڶ��ѹ
    ///
    static int CheckResult(Result rs, double Ue);

    ///
    /// @method:    CheckInputParam
    /// @access:    private static 
    /// @brief:
    ///		��������������ȷ��
    /// @param[in]: 
    ///		cm						����ģ��
    ///		zl						��·�����迹
    ///     ks                      ϵͳ�迹���򲹳�ϵ��
    ///		kl						��·�迹���򲹳�ϵ��
    ///		lc						���ɵ���
    /// @return:   
    ///		int					    0����ȷ
    ///                            -1������ģ�͵ĵ���/��ѹ/ϵͳ�迹�ȣ���·�迹
    ///                                ���򲹳�ϵ���͸��ɵ�����ģС��0������
    ///
    static int CheckInputParam(CalcModel cm, Vector zl, Vector ks, Vector kl,
        Vector lc);

    ///
    /// @method:    DegreeAdjust
    /// @access:    private static 
    /// @brief:
    ///		��λ�ǵ���������λ�ǵ�����-180��~180��ķ�Χ�ڣ�
    /// @param[inout]: 
    ///		rs						��·������
    /// @return:   
    ///		void					
    ///
    static void DegreeAdjust(Result &rs);

    ///
    /// @method:    DirectionAdjust
    /// @access:    private static 
    /// @brief:
    ///		���Ϸ���������������Ҫ�����������ĵ�������
    /// @param[inout]: 
    ///		rs						��·������
    ///     direction               ���Ϸ���
    /// @return:   
    ///		void					
    ///
    static void DirectionAdjust(Result &rs, DirectionType direction);

    ///
    /// @method:    KLCalc
    /// @access:    public static 
    /// @brief:
    ///		KR,KX��ʽ�������򲹳�ϵ��
    /// @param[in]: 
    ///		kr						KR
    ///		kx						KX
    ///		zl						��·�����迹
    /// @param[out]:
    ///		kl						���򲹳�ϵ��
    /// @return:   
    ///     void
    ///
    static void KLCalc(double kr, double kx, Vector zl, Vector &kl);

    ///
    /// @method:    KLCalc
    /// @access:    public static 
    /// @brief:
    ///		Z0/Z1��ʽ�������򲹳�ϵ��
    /// @param[in]: 
    ///		z0z1						Z0/Z1��������
    /// @param[out]:
    ///		kl						    ���򲹳�ϵ��
    /// @return:   
    ///		void				
    ///
    static void KLCalc(Vector z0z1, Vector &kl);

    ///
    /// @method:    Convert
    /// @access:    private static
    /// @brief:
    ///		����ת����
    /// @param[in]: 
    ///		v					    Ҫת��������
    /// @return:   
    ///		complex<double>         ת����ĸ���
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
    ///		����ת����
    /// @param[in]: 
    ///		c					    Ҫת���ĸ���
    /// @return:   
    ///		SCCalc::Vector          ת���������
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
    ///		����ת�Ƕȣ�radian to degree��
    /// @param[]: 
    ///		r						����
    /// @return:   
    ///		double			        ת����ĽǶ�
    ///
    static inline double r2d(double r)
    {
        return r/PI*180;
    }

    ///
    /// @method:    d2r
    /// @access:    private static 
    /// @brief:
    ///		�Ƕ�ת���ȣ�degree to radian��
    /// @param[]: 
    ///		a						�Ƕ�
    /// @return:   
    ///		double			        ת����Ļ���
    ///
    static inline double d2r(double a)
    {
        return a/180*PI;
    }
};
#endif