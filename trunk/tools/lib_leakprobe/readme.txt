����: �ڴ�й¶�����


�÷�: ��Դ�ļ����빤�̱���
      ͨ�������������Դ��ڲ鿴���
      �������Լ��Ϻ궨�� USE_LKP_MEMLEAK
               �����Ҫ̽��Bitmap��й¶, ��Ҫ�ټ���USE_LKP_MEMLEAK_BITMAP

         
����Ҫ�����˳����ܿ����ڴ�й¶��Ϣ

��Ҫ�� cnt 11012, handle 0x1088960, size 22, type 1
������Ϣ, cnt����ڼ��η����ڴ�, handle�����ڴ��ַ, type������������ �� malloc, ����new ����LocaleAlloc
һ��й¶��ʾ4�����ڵĶ�����Ϊ��й¶, ��Ϊ����mfc���е�й©

���ÿ�μ�⵽ cnt 11012, handle 0x1088960   ������ֵһ����, �Ϳ����¶ϵ�
�㿴һ�� lib_leakprobe.cpp ����ļ�
void  Lkp_StartProbe ()
{
    Lkp_StartProbeMemLeak();
    //Lkp_BreakAtAllocCount(2649);
    //Lkp_BreakAtAllocHandle((void*)0x7105e0);

    Lkp_StartProbeMemUsage();
}