# -*- coding: cp936 -*-

import sys
import os
import pysvn
import xml.dom.minidom
import os.path,re,time

# 添加Subversion路径到Path
def AddSubversionToPath( ):
	listPath = []
	for l in os.environ['path'].split(';'):
		if 'Subversion' not in l:
			listPath.append(l)
			
	strPath = ';'.join(listPath)
	os.environ['path'] = strPath


#获取最新修改文件
def getReposeLastestFile(SVNReposPath) :
	t=time.strptime("2000-01-01 00:00:00", "%Y-%m-%d %H:%M:%S")
	t=time.mktime(t)
	if os.path.isfile(SVNReposPath) == False:
		entry_list = pysvn.Client().ls(SVNReposPath)
		for file in entry_list:
			if os.path.isfile(file.name) == True:
				if os.path.getmtime(file.name) > t:
					t = os.path.getmtime(file.name)
					filePath = file.name;
			else:
				tmp = getReposeLastestFile(file.name)
				if os.path.getmtime(tmp) > t:
					t = os.path.getmtime(tmp)
					filePath = tmp
		return filePath
	else:
		return SVNReposPath
	
#获取指定路径的Revision Number
def GetReposeRevNum(SVNReposPath):
	info = pysvn.Client().info(SVNReposPath)
	return info.commit_revision.number

def Max(a, b):
   if (a > b):
	   return a
   else:
	   return b

# 将xml的build字段的数字替换
def FixConfigXmlRevNumber( xmlfile, buildrev ):
	# 读取xml文件
	xmldoc = xml.dom.minidom.parse(xmlfile)

	# 找到目标node
	config_nodes = xmldoc.getElementsByTagName("config")
	buildrev_nodes = config_nodes[0].getElementsByTagName("softbuild")

	# 替换属性值
	buildrev_nodes[0].attributes["val"].value = "%d"%buildrev  # 数字转字符串

	# 保存xml文件
	writeFile = open(xmlfile, 'w')
	writeFile.write(xmldoc.toxml('utf-8'))

	
# python build_rev.py ./ ../bin/release_unicode/config.xml ../bin/debug_unicode/config.xml
# python build_rev.py  版本库目录  需要修正的xml文件1  需要修正的xml文件2 ...
if __name__ == "__main__":

	# 获得参数个数
	argc = len(sys.argv)

	# 获得当前本地目录下的SVN版本
	SVNReposPath = sys.argv[1]  
	AddSubversionToPath()
	lastestFile = getReposeLastestFile(SVNReposPath)
	ReposRevNumber = GetReposeRevNum(lastestFile)
	print "last File:", lastestFile
	print "svn rev:", ReposRevNumber

	# 替换所有的xml文件中的buildrev字段
	for i in range(2, argc, 1):
		print "fix:", sys.argv[i]
		FixConfigXmlRevNumber(sys.argv[i], ReposRevNumber)