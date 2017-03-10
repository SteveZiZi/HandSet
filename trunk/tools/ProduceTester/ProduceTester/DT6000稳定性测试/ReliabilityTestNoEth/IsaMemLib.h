BOOL IsaMem_LoadLib();

void IsaMem_UnLoadLib();

DWORD epcIsaMemByteR(    DWORD   dMemAddress,
                                    PUCHAR  pucData);

/*********************************************************************************************************
** Function name:           epcIsaMemUCHARW
** Descriptions:            ��Memory �ռ�д��һ���ֽ�
** Input parameters:        dMemAddress             Ҫд�����ݵ�Memory ��ַ        
**                          ucData                   Ҫд�������       
** Output parameters:                                  
** Returned value:          ISA_NO_ERR              д�����ݳɹ�
**                          ISA_MEM_OVER_RANGE      ��ǰҪ�õ�Memory��ַ������Χ
*********************************************************************************************************/
DWORD epcIsaMemByteW(    DWORD   dMemAddress,
                                    UCHAR   ucData);

/*********************************************************************************************************
** Function name:           MemWordR
** Descriptions:            ��Memory �ռ��ȡ16 bit����
** Input parameters:        dMemAddress             Ҫ��ȡ���ݵ�Memory ��ַ               
** Output parameters:       pusData                 ��ȡ��������
** Returned value:          ISA_NO_ERR              ��ȡ���ݳɹ�
**                          ISA_MEM_OVER_RANGE      ��ǰҪ�õ�Memory��ַ������Χ
**                          ISA_POINTER_IS_NULL     �����ָ�����ΪNULL
**                          ISA_ADDR_ODD            ����ĵ�ַΪ����
*********************************************************************************************************/
DWORD epcIsaMemWordR( DWORD   dMemAddress,
                                 PUINT16 pusData);

/*********************************************************************************************************
** Function name:           MemWordW
** Descriptions:            ��Memory �ռ�д��16  bit����
** Input parameters:        dMemAddress             Ҫд�����ݵ�Memory ��ַ               
**                          usData                  Ҫд�������
** Output parameters:                          
** Returned value:          ISA_NO_ERR              ��ȡ���ݳɹ�
**                          ISA_MEM_OVER_RANGE      ��ǰҪ�õ�Memory��ַ������Χ
**                          ISA_ADDR_ODD            ����ĵ�ַΪ����
*********************************************************************************************************/
DWORD epcIsaMemWordW( DWORD   dMemAddress,
                                 UINT16  usData);

/*********************************************************************************************************
** Function name:           IsaInitIntrrupt
** Descriptions:            ��ʼ���ж�
** Input parameters:        dwIRQNumber             �жϺ�
**                                                  IRQ3��IRQ4...
**                          hIntrEvent              �ж϶�Ӧ���¼�                          
** Output parameters:       pdwSysIntr              ϵͳ������߼��жϺ�
** Returned value:          ISA_NO_ERR              �жϳ�ʼ���ɹ�
**                          ISA_NO_IRQ_NUM          PC/104���߲���ʹ�ø��жϺ�
**                          ISA_INIT_INTR_FAIL      �жϳ�ʼ��ʧ��
*********************************************************************************************************/
DWORD epcIsaInitIntrrupt( DWORD   dwIRQNumber, 
                                     HANDLE  hIntrEvent, 
                                     PDWORD  pdwSysIntr);

/*********************************************************************************************************
** Function name:           epcIsaIoWordW
** Descriptions:            ��IO�ռ�д��16  bit����
** Input parameters:        dwIoAddress              Ҫ��ȡ���ݵ�IO��ַ               
**                          usData                  ��Ҫд������
** Output parameters:       
** Returned value:          ISA_NO_ERR              ��ȡ���ݳɹ�
**                          ISA_IO_OVER_RANGE       ��ǰҪ���ʵ�I/O��ַ������Χ
**                          ISA_ADDR_ODD            ����ĵ�ַΪ����
*********************************************************************************************************/
DWORD epcIsaIoWordW(  DWORD   dwIoAddress,
                                 UINT16  usData);


/*********************************************************************************************************
** Function name:           epcIsaIntrruptDisable
** Descriptions:            ��ֹ�ж�    
** Input parameters:        dwSysIntrNumber         �߼��жϺ�
** Output parameters:
** Returned value:          ISA_NO_ERR              ��ֹ�жϳɹ�
**                          ISA_INTR_UNINIT         �жϻ�û�г�ʼ��
*********************************************************************************************************/
DWORD epcIsaIntrruptDisable( DWORD dwSysIntrNumber);

/*********************************************************************************************************
** Function name:           epcIsaIntrruptDone
** Descriptions:            ����ʹ���ж�    
** Input parameters:        dwSysIntrNumber         �߼��жϺ�
** Output parameters:             
** Returned value:          ISA_NO_ERR              ����ʹ���жϳɹ�              
**                          ISA_INTR_UNINIT         �жϻ�û�г�ʼ��
*********************************************************************************************************/
DWORD epcIsaIntrruptDone(    DWORD dwSysIntrNumber);