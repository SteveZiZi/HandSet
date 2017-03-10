


/**********************************************************************************************
** 函数名称：epcAdGetSampleValue
** 功能描述：读取AD通道的采样值
** 入口参数：pValueArray 指向AD数据的指针，即用它返回AD数据,返回的值是直接从AD通道读到的
             采样值，为数字量，其范围视AD芯片的不同而有所不同，得到其AD采样值可根据需要
             转换为带有单位的模拟量的值
            
             要读取的每一个通道的值占用一个DWORD单元
**           dwBeginChannel  要读取的通道的开始
**           dwEndChannel    要读取的通道的结束
** 返回值  ：非0值  :成功
**			 =0     :错误
** 	 
************************************************************************************************/
extern "C" BOOL epcAdGetSampleValue(DWORD *pValueArray,DWORD dwBeginChannel,DWORD dwEndChannel);

/**********************************************************************************************
** 函数名称：epcDASetOutputValue
** 功能描述：设置DA通道
** 入口参数：pValueArray 指向欲设置DA的值数组，每一个通道要设置的值占用一个DWORD单元，
             并且如果要设置的值高于DA芯片的最大值，可能会被自动截取，pValueArray数组的元素
             从dwBeginChannel到dwEndChannel依次排列
**           dwBeginChannel  要设置的通道的开始
**           dwEndChannel    要设置的通道的结束
** 返回值  ：非0值  :成功
**			 =0     :错误
** 	 
************************************************************************************************/
extern "C" BOOL epcDASetOutputValue(DWORD *pValueArray,DWORD dwBeginChannel,DWORD dwEndChannel);

//打开AD/DA驱动句柄
extern "C" BOOL    epcAdDaOpenDriver();
//关闭AD/DA驱动句柄
extern "C" void    epcAdDaCloseDriver();