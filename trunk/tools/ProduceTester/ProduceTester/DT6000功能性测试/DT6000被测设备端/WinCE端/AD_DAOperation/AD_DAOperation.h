


/**********************************************************************************************
** �������ƣ�epcAdGetSampleValue
** ������������ȡADͨ���Ĳ���ֵ
** ��ڲ�����pValueArray ָ��AD���ݵ�ָ�룬����������AD����,���ص�ֵ��ֱ�Ӵ�ADͨ��������
             ����ֵ��Ϊ���������䷶Χ��ADоƬ�Ĳ�ͬ��������ͬ���õ���AD����ֵ�ɸ�����Ҫ
             ת��Ϊ���е�λ��ģ������ֵ
            
             Ҫ��ȡ��ÿһ��ͨ����ֵռ��һ��DWORD��Ԫ
**           dwBeginChannel  Ҫ��ȡ��ͨ���Ŀ�ʼ
**           dwEndChannel    Ҫ��ȡ��ͨ���Ľ���
** ����ֵ  ����0ֵ  :�ɹ�
**			 =0     :����
** 	 
************************************************************************************************/
extern "C" BOOL epcAdGetSampleValue(DWORD *pValueArray,DWORD dwBeginChannel,DWORD dwEndChannel);

/**********************************************************************************************
** �������ƣ�epcDASetOutputValue
** ��������������DAͨ��
** ��ڲ�����pValueArray ָ��������DA��ֵ���飬ÿһ��ͨ��Ҫ���õ�ֵռ��һ��DWORD��Ԫ��
             �������Ҫ���õ�ֵ����DAоƬ�����ֵ�����ܻᱻ�Զ���ȡ��pValueArray�����Ԫ��
             ��dwBeginChannel��dwEndChannel��������
**           dwBeginChannel  Ҫ���õ�ͨ���Ŀ�ʼ
**           dwEndChannel    Ҫ���õ�ͨ���Ľ���
** ����ֵ  ����0ֵ  :�ɹ�
**			 =0     :����
** 	 
************************************************************************************************/
extern "C" BOOL epcDASetOutputValue(DWORD *pValueArray,DWORD dwBeginChannel,DWORD dwEndChannel);

//��AD/DA�������
extern "C" BOOL    epcAdDaOpenDriver();
//�ر�AD/DA�������
extern "C" void    epcAdDaCloseDriver();