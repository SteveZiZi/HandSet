BOOL IsaMem_LoadLib();

void IsaMem_UnLoadLib();

DWORD epcIsaMemByteR(    DWORD   dMemAddress,
                                    PUCHAR  pucData);

/*********************************************************************************************************
** Function name:           epcIsaMemUCHARW
** Descriptions:            向Memory 空间写入一个字节
** Input parameters:        dMemAddress             要写入数据的Memory 地址        
**                          ucData                   要写入的数据       
** Output parameters:                                  
** Returned value:          ISA_NO_ERR              写入数据成功
**                          ISA_MEM_OVER_RANGE      当前要访的Memory地址超出范围
*********************************************************************************************************/
DWORD epcIsaMemByteW(    DWORD   dMemAddress,
                                    UCHAR   ucData);

/*********************************************************************************************************
** Function name:           MemWordR
** Descriptions:            在Memory 空间读取16 bit数据
** Input parameters:        dMemAddress             要读取数据的Memory 地址               
** Output parameters:       pusData                 读取到的数据
** Returned value:          ISA_NO_ERR              读取数据成功
**                          ISA_MEM_OVER_RANGE      当前要访的Memory地址超出范围
**                          ISA_POINTER_IS_NULL     输入的指针变量为NULL
**                          ISA_ADDR_ODD            输入的地址为奇数
*********************************************************************************************************/
DWORD epcIsaMemWordR( DWORD   dMemAddress,
                                 PUINT16 pusData);

/*********************************************************************************************************
** Function name:           MemWordW
** Descriptions:            在Memory 空间写入16  bit数据
** Input parameters:        dMemAddress             要写入数据的Memory 地址               
**                          usData                  要写入的数据
** Output parameters:                          
** Returned value:          ISA_NO_ERR              读取数据成功
**                          ISA_MEM_OVER_RANGE      当前要访的Memory地址超出范围
**                          ISA_ADDR_ODD            输入的地址为奇数
*********************************************************************************************************/
DWORD epcIsaMemWordW( DWORD   dMemAddress,
                                 UINT16  usData);

/*********************************************************************************************************
** Function name:           IsaInitIntrrupt
** Descriptions:            初始化中断
** Input parameters:        dwIRQNumber             中断号
**                                                  IRQ3、IRQ4...
**                          hIntrEvent              中断对应的事件                          
** Output parameters:       pdwSysIntr              系统分配的逻辑中断号
** Returned value:          ISA_NO_ERR              中断初始化成功
**                          ISA_NO_IRQ_NUM          PC/104总线不能使用该中断号
**                          ISA_INIT_INTR_FAIL      中断初始化失败
*********************************************************************************************************/
DWORD epcIsaInitIntrrupt( DWORD   dwIRQNumber, 
                                     HANDLE  hIntrEvent, 
                                     PDWORD  pdwSysIntr);

/*********************************************************************************************************
** Function name:           epcIsaIoWordW
** Descriptions:            在IO空间写入16  bit数据
** Input parameters:        dwIoAddress              要读取数据的IO地址               
**                          usData                  将要写入数据
** Output parameters:       
** Returned value:          ISA_NO_ERR              读取数据成功
**                          ISA_IO_OVER_RANGE       当前要访问的I/O地址超出范围
**                          ISA_ADDR_ODD            输入的地址为奇数
*********************************************************************************************************/
DWORD epcIsaIoWordW(  DWORD   dwIoAddress,
                                 UINT16  usData);


/*********************************************************************************************************
** Function name:           epcIsaIntrruptDisable
** Descriptions:            禁止中断    
** Input parameters:        dwSysIntrNumber         逻辑中断号
** Output parameters:
** Returned value:          ISA_NO_ERR              禁止中断成功
**                          ISA_INTR_UNINIT         中断还没有初始化
*********************************************************************************************************/
DWORD epcIsaIntrruptDisable( DWORD dwSysIntrNumber);

/*********************************************************************************************************
** Function name:           epcIsaIntrruptDone
** Descriptions:            重新使能中断    
** Input parameters:        dwSysIntrNumber         逻辑中断号
** Output parameters:             
** Returned value:          ISA_NO_ERR              重新使能中断成功              
**                          ISA_INTR_UNINIT         中断还没有初始化
*********************************************************************************************************/
DWORD epcIsaIntrruptDone(    DWORD dwSysIntrNumber);