# -*- coding: cp936 -*-

import sys
import os
import xml.dom.minidom
import pefile


class Version:
    pass


# ��xml��build�ֶε������滻
def GetDSMVersion( xmlfile ):
    # ��ȡxml�ļ�
    xmldoc = xml.dom.minidom.parse(xmlfile)

    # �ҵ�Ŀ��node
    config_nodes = xmldoc.getElementsByTagName("config")
    softwarever_nodes = config_nodes[0].getElementsByTagName("softwarever")
    buildrev_nodes = config_nodes[0].getElementsByTagName("softbuild")
    
    # �ϳɰ汾���ַ��� 1.2.3.4, ���ָlist
    verstr = softwarever_nodes[0].attributes["val"].value + "." +buildrev_nodes[0].attributes["val"].value
    verlist = verstr.split(".")
    
    # ���ɷ��صİ汾����
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
# python fixversion.py  �汾��Ϣ���ڵ�xml  ��Ҫ������exe�ļ�
# �����޸�exe���ļ��汾��
if __name__ == "__main__":

    # ��ò�������
    argc = len(sys.argv)
    
    # �������汾��
    ver = GetDSMVersion(sys.argv[1])
    print ver.major, ver.minor, ver.revision, ver.build
    
    # �޸�PE�ļ����ļ��汾��
    for i in range(2, argc, 1):
        print "fix:", sys.argv[i]
        FixPEFileVersion(sys.argv[i], ver)