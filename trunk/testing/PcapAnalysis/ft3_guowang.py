# -*- coding: cp936 -*-

'''
分析FT3扩展协议录波报文的特定通道值, 博电的FT3录波文件
使用时改变 ch 变量, 和 chraw_2_ch2 函数 和 pcapfile 变量
'''

import os
import os.path
from struct import *

# 定义常量
pcapfile = r"c:\FT3 2013-05-29_3Ae5A_57Ve100V.pcap"

sqrt2 = 1.414
sqrt3 = 1.732

# 数据包偏移
packetlen = 37 * 2  # 每个报文字节长度
headerlen = 10 * 2  # 报文头字节长度(第一个通道前的数据)
ch = 12  # 通道号
chlen = 2  # 每通道字节长度
choff = headerlen + (ch - 1) * chlen + (ch - 1) / 8 * 2  # 计算通道数值偏移

Vrat1 = 110 * 1000  # 一次额定V (线电压)
Vrat2 = 100  # 二次额定V (线电压)
Arat1 = 3000  # 一次额定A (线电压)
Arat2 = 5  # 二次额定A (线电压)
Kscp = 0x1CF
Kscm = 0x2D41
Ksv = 0x2D41

chval_array = []  # 通道自然值返回



def s16_2_u16(src):
    return src & 0xffff

# 从报文值转为二次值
def chraw_2_ch2(raw):
    return 1.0 * raw * (Vrat2/sqrt3) / Ksv  # 电压通道,  /sqrt3 是为了算得额定相电压
    #return 1.0 * raw  * Arat2 / Kscp  # 保护用电流通道
    #return 1.0 * raw  * Arat2 / Kscm  # 测量用电流通道


file = open(pcapfile, "rb")
binline = file.read(packetlen)
while len(binline) != 0:
    nums = unpack('h', binline[choff: choff+chlen])
    chval_raw = nums[0]
    #print "0x%04X %d" %(s16_2_u16(chval_raw), chval_raw)

    chval_2 = chraw_2_ch2(chval_raw)
    chval_array.append(chval_2)

    binline = file.read(packetlen)  # next


peakMax = max(chval_array)
peakMin = min(chval_array)
rms = peakMax / sqrt2
print peakMax, rms
