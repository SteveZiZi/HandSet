# -*- coding: cp936 -*-

import sys
import os
import zipfile
import xml.dom.minidom


# ���ļ���foldername ����ѹ��, ѹ������ļ�filename
def zipfolder( foldername, filename ):
    '''
        zip folder foldername and all its subfiles and folders into
        a zipfile named filename
    '''
    empty_dirs = []
    zip = zipfile.ZipFile(filename, 'w', zipfile.ZIP_DEFLATED)
    for root,dirs,files in os.walk(foldername):
    
        # ������еĿ�Ŀ¼
        for dir in dirs:
            if os.listdir(os.path.join(root, dir)) == []:
                empty_dirs.append(os.path.join(root, dir)[len(foldername)+1:])
   
    
        # ע��, ����������յ��ļ���ʱ, ��������ļ��м���zip�ļ�, ����Ҫ, ������ӶԿ��ļ��еĴ������
        for filename in files:
            srcfile = os.path.join(root, filename)
            print "compressing", srcfile
            zip.write(srcfile, srcfile[len(foldername):])
    
    # ����Ŀ¼��Ϊ���ļ�����zip
    for dir in empty_dirs:
        zip.writestr(dir+"\\", "")

    zip.close()
    print "Finish compressing"

    


# ��xml��build�ֶε������滻
def GetDSMVersion( xmlfile ):
    # ��ȡxml�ļ�
    xmldoc = xml.dom.minidom.parse(xmlfile)

    # �ҵ�Ŀ��node
    config_nodes = xmldoc.getElementsByTagName("config")
    softwarever_nodes = config_nodes[0].getElementsByTagName("softwarever")
    buildrev_nodes = config_nodes[0].getElementsByTagName("softbuild")
    
    return softwarever_nodes[0].attributes["val"].value + "." +buildrev_nodes[0].attributes["val"].value
    
    
# python install_package.py ..\setup\deploy_release ..\setup\deploy_release\flashdisk2\DSM\config.xml
# python install_package.py  ��Ҫѹ�����ļ���  ��ȡ�汾��Ϣ��xml�ļ�
if __name__ == "__main__":

    # ��ò�������
    argc = len(sys.argv)
    
    for i in range(1, argc, 2):
    
        version = GetDSMVersion(sys.argv[i+1])
        
        # ���������ڵ�Ŀ¼
        setupdir = sys.argv[i] + "_pkg"
        if os.path.exists(setupdir) == False:
            os.mkdir(setupdir)
            
        os.system("del " + setupdir + "\\*.* /Q")  # ɾ���ļ����������ļ�

            
        zipfolder(sys.argv[i], setupdir + "\\Setup_" + version + ".zip")
   