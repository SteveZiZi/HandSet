# -*- coding: cp936 -*-

import sys
import os
import zipfile
import xml.dom.minidom


# 对文件夹foldername 进行压缩, 压缩后的文件filename
def zipfolder( foldername, filename ):
    '''
        zip folder foldername and all its subfiles and folders into
        a zipfile named filename
    '''
    empty_dirs = []
    zip = zipfile.ZipFile(filename, 'w', zipfile.ZIP_DEFLATED)
    for root,dirs,files in os.walk(foldername):
    
        # 获得所有的空目录
        for dir in dirs:
            if os.listdir(os.path.join(root, dir)) == []:
                empty_dirs.append(os.path.join(root, dir)[len(foldername)+1:])
   
    
        # 注意, 这里遍历到空的文件夹时, 并不会把文件夹加入zip文件, 如需要, 额外添加对空文件夹的处理代码
        for filename in files:
            srcfile = os.path.join(root, filename)
            print "compressing", srcfile
            zip.write(srcfile, srcfile[len(foldername):])
    
    # 将空目录作为空文件加入zip
    for dir in empty_dirs:
        zip.writestr(dir+"\\", "")

    zip.close()
    print "Finish compressing"

    


# 将xml的build字段的数字替换
def GetDSMVersion( xmlfile ):
    # 读取xml文件
    xmldoc = xml.dom.minidom.parse(xmlfile)

    # 找到目标node
    config_nodes = xmldoc.getElementsByTagName("config")
    softwarever_nodes = config_nodes[0].getElementsByTagName("softwarever")
    buildrev_nodes = config_nodes[0].getElementsByTagName("softbuild")
    
    return softwarever_nodes[0].attributes["val"].value + "." +buildrev_nodes[0].attributes["val"].value
    
    
# python install_package.py ..\setup\deploy_release ..\setup\deploy_release\flashdisk2\DSM\config.xml
# python install_package.py  需要压缩的文件夹  获取版本信息的xml文件
if __name__ == "__main__":

    # 获得参数个数
    argc = len(sys.argv)
    
    for i in range(1, argc, 2):
    
        version = GetDSMVersion(sys.argv[i+1])
        
        # 创建包所在的目录
        setupdir = sys.argv[i] + "_pkg"
        if os.path.exists(setupdir) == False:
            os.mkdir(setupdir)
            
        os.system("del " + setupdir + "\\*.* /Q")  # 删除文件夹下所有文件

            
        zipfolder(sys.argv[i], setupdir + "\\Setup_" + version + ".zip")
   