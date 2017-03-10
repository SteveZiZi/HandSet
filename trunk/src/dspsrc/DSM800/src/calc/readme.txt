算法库使用说明：

本算法同时兼容WIN32和DSP，具体算法说明详见各个算法的源文件，在DSP端需要预定义_DSP宏方可引用快速算法库

calc.h                 包含每个算法的头文件，方便一次性导入所有算法
├─ad2                双极性
├─dispRate           报文离散度
├─effVal             有效值
├─fft                傅里叶变换
├─freq               频率计算
├─harm               谐波计算
├─inc                公共包含文件，用于兼容性
├─nuclear            核相
├─phaseSeq           序量
├─polar              极性
├─power              功率
├─sampleCreate       波形生成
└─utils              辅助工具

