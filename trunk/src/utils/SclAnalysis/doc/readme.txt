��ǰ���SCL�ļ�������Ļ����ϣ��Ż��������̣�������Ҫ��������ݣ�����ʵ��SCL�ļ���ɾ�����ܡ�

SCL�������������ݣ�
1��Communication->SubNetwork->ConnectedAP�е�ȫ��SMV��GOOSE��ͨ�Ų�������
2��IED->AccessPoint->Server->LDevice�е�LN0->GSEControl/SampledValueControl��
3��IED����GSEControl/SampledValueControl��ص����ݼ����������FCDA����ָ���DOI��
4��IED����GSEControl/SampledValueControl��ص�Inputs��Inputs->ExtRef��ָ���DOI��
   ע����IEC61850Ӧ�����š���һ����ÿ��װ�õ�LLN0��Inputs���ֶ����װ�õ�Goose�Ͳ���ֵ����������Ϣ������������ӵ��߼��ڵ��ǰ׺һ�㺬�С�GOIN���͡�SVIN����P226~P277��P301~302��
5��IED��LDevice��LN0��LN��DataSet�ȵ�һЩ���ԣ���desc��name�ȣ������scltypes.h�Ķ��塣
6��DataTypeTemplates���֣�ȫ����LNType��DOType��DAType���������ֲ���ȡȫ�������ݣ������scltypes.h�Ķ��塣

Ŀǰ���Խ�����£�

�ɰ汾���������
������̷�������SCD�ļ���347k������ʱ��300~400�������ڴ�Ϊԭ����1.5����
����5M��SCD�ļ���                ��ʱ��5~6s��
����10M��SCD�ļ���               ��ʱ��10s��

�°汾���������
������̷�������SCD�ļ���347k������ʱ��255~300ms���ڴ棺216K(+Inputs) \208K(-Inputs)��
����5120k��SCD�ļ���             ��ʱ��2.6s��     �ڴ棺2666K(+Inputs)\2666K(-Inputs)��
����10147k��SCD�ļ���            ��ʱ��4.0s��     �ڴ棺2673K(+Inputs)\2673K(-Inputs)��

SCL�ļ�ɾ�������
220kVͩ�����201303081044.scd��34.6M����3.01M��+Inputs����2.13M��-Inputs����
2012102401leigu.scd��29.9M����          4.47M��+Inputs����4.23M��-Inputs����
WC1016B.scd��5M����                     564K��+Inputs�� ��564K��-Inputs����
WC1016B_2.scd��10147K����               564K��+Inputs�� ��564K��-Inputs����

ʹ�ü򻯺��SCL�ļ����н�������Ҫ��ʱ���ԭ�ļ��٣���������ڴ���ԭ�ļ���������ڴ����󣬴��Ϊ�򻯺��ļ���3~4�������£�
220kVͩ�����201303081044.scd��2.13M������ʱ��6.9s�� �ڴ棺6774K ��
2012102401leigu.scd��4.23M����          ��ʱ��13.7s���ڴ棺16160K��
WC1016B.scd��564K����                   ��ʱ��1.3s�� �ڴ棺2608K ��