目录说明

├─dsp                     DSP通信算法实现目录
├─dsplib                  DSP相关文件目录
│  ├─internal            DSP标准库
│  ├─OMAP3530            DSP标准库
│  └─usr                 DSP标准库
└─include                 提供DSP算法接口，供外部调用（详细的函数注释请参看对应的.cpp文件）

通信接口说明

本模块提供给外部的接口位于include文件夹中

    用户需要在程序开始手动调用CDspController类中的DspInit方法初始化Dsp，调用DspDelete方法释放Dsp, 其他帧的发送和接收函数，如果没有特殊需要，一般可以不用，只需调用CDsp类中封装的方法即可

    用户调用CDsp类中定义的各种方法获取所得结果。其中，每一种计算的发送过程都封装为一个函数，如SendSmv，GetDspInfo函数，用户可以采用多个线程不断的执行某个发送函数执行发送动作，而接收动作统一由一个接收线程来执行接收过程（使用StartRead开始读取线程，使用StopRead停止读取线程），接收的数据会存入DspDataSet.h中定义的数据集结构体中，用户需要在外边创建该结构体类型缓冲区，并将引用传给Dsp类（在测试工程中缓存区是在CDataManager单例类中创建的）来构造Dsp实例，然后进行操作。

    具体的使用方法请参看测试工程