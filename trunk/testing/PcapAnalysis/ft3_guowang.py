# -*- coding: cp936 -*-

'''
����FT3��չЭ��¼�����ĵ��ض�ͨ��ֵ, �����FT3¼���ļ�
ʹ��ʱ�ı� ch ����, �� chraw_2_ch2 ���� �� pcapfile ����
'''

import os
import os.path
from struct import *

# ���峣��
pcapfile = r"c:\FT3 2013-05-29_3Ae5A_57Ve100V.pcap"

sqrt2 = 1.414
sqrt3 = 1.732

# ���ݰ�ƫ��
packetlen = 37 * 2  # ÿ�������ֽڳ���
headerlen = 10 * 2  # ����ͷ�ֽڳ���(��һ��ͨ��ǰ������)
ch = 12  # ͨ����
chlen = 2  # ÿͨ���ֽڳ���
choff = headerlen + (ch - 1) * chlen + (ch - 1) / 8 * 2  # ����ͨ����ֵƫ��

Vrat1 = 110 * 1000  # һ�ζV (�ߵ�ѹ)
Vrat2 = 100  # ���ζV (�ߵ�ѹ)
Arat1 = 3000  # һ�ζA (�ߵ�ѹ)
Arat2 = 5  # ���ζA (�ߵ�ѹ)
Kscp = 0x1CF
Kscm = 0x2D41
Ksv = 0x2D41

chval_array = []  # ͨ����Ȼֵ����



def s16_2_u16(src):
    return src & 0xffff

# �ӱ���ֵתΪ����ֵ
def chraw_2_ch2(raw):
    return 1.0 * raw * (Vrat2/sqrt3) / Ksv  # ��ѹͨ��,  /sqrt3 ��Ϊ����ö���ѹ
    #return 1.0 * raw  * Arat2 / Kscp  # �����õ���ͨ��
    #return 1.0 * raw  * Arat2 / Kscm  # �����õ���ͨ��


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
