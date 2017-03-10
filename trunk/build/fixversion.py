# -*- coding: cp936 -*-

import sys
import os
import xml.dom.minidom
import pefile


class Version:
    pass


# 将xml的build字段的数字替换
def GetDSMVersion( xmlfile ):
    # 读取xml文件
    xmldoc = xml.dom.minidom.parse(xmlfile)

    # 找到目标node
    config_nodes = xmldoc.getElementsByTagName("config")
    softwarever_nodes = config_nodes[0].getElementsByTagName("softwarever")
    buildrev_nodes = config_nodes[0].getElementsByTagName("softbuild")
    
    # 合成版本号字符串 1.2.3.4, 并分割到list
    verstr = softwarever_nodes[0].attributes["val"].value + "." +buildrev_nodes[0].attributes["val"].value
    verlist = verstr.split(".")
    
    # 生成返回的版本对象
    ver = Version()
    ver.major = int(verlist[0])
    ver.minor = int(verlist[1])
    ver.revision = int(verlist[2])
    ver.build = int(verlist[3])    
    return ver
    
    
def FixPEFileVersion( exefile, version ):
    pe = pefile.PE(exefile)
    pe.VS_FIXEDFILEINFO.FileVersionMS = version.major * 0x10000 + version.minor
    pe.VS_FIXEDFILEINFO.FileVersionLS = version.revision * 0x10000 + version.build
    new_file_data = pe.write()
    pe.close()

    f = file(exefile, 'wb+')
    f.write(new_file_data)
    f.close()

    
# python fixversion.py ..\setup\deploy_release\flashdisk2\DSTester\info.xml ..\setup\deploy_release\flashdisk2\DSTester\dsm.exe
# python fixversion.py  版本信息所在的xml  修要修正的exe文件
# 用于修改exe的文件版本号
if __name__ == "__main__":

    # 获得参数个数
    argc = len(sys.argv)
    
    # 获得软件版本号
    ver = GetDSMVersion(sys.argv[1])
    print ver.major, ver.minor, ver.revision, ver.build
    
    # 修改PE文件的文件版本号
    for i in range(2, argc, 1):
        print "fix:", sys.argv[i]
        FixPEFileVersion(sys.argv[i], ver)