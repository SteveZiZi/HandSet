# -*- coding: cp936 -*-

import sys
import os
import pysvn
import xml.dom.minidom
import os.path,re,time

# ���Subversion·����Path
def AddSubversionToPath( ):
	listPath = []
	for l in os.environ['path'].split(';'):
		if 'Subversion' not in l:
			listPath.append(l)
			
	strPath = ';'.join(listPath)
	os.environ['path'] = strPath


#��ȡ�����޸��ļ�
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
	
#��ȡָ��·����Revision Number
def GetReposeRevNum(SVNReposPath):
	info = pysvn.Client().info(SVNReposPath)
	return info.commit_revision.number

def Max(a, b):
   if (a > b):
	   return a
   else:
	   return b

# ��xml��build�ֶε������滻
def FixConfigXmlRevNumber( xmlfile, buildrev ):
	# ��ȡxml�ļ�
	xmldoc = xml.dom.minidom.parse(xmlfile)

	# �ҵ�Ŀ��node
	config_nodes = xmldoc.getElementsByTagName("config")
	buildrev_nodes = config_nodes[0].getElementsByTagName("softbuild")

	# �滻����ֵ
	buildrev_nodes[0].attributes["val"].value = "%d"%buildrev  # ����ת�ַ���

	# ����xml�ļ�
	writeFile = open(xmlfile, 'w')
	writeFile.write(xmldoc.toxml('utf-8'))

	
# python build_rev.py ./ ../bin/release_unicode/config.xml ../bin/debug_unicode/config.xml
# python build_rev.py  �汾��Ŀ¼  ��Ҫ������xml�ļ�1  ��Ҫ������xml�ļ�2 ...
if __name__ == "__main__":

	# ��ò�������
	argc = len(sys.argv)

	# ��õ�ǰ����Ŀ¼�µ�SVN�汾
	SVNReposPath = sys.argv[1]  
	AddSubversionToPath()
	lastestFile = getReposeLastestFile(SVNReposPath)
	ReposRevNumber = GetReposeRevNum(lastestFile)
	print "last File:", lastestFile
	print "svn rev:", ReposRevNumber

	# �滻���е�xml�ļ��е�buildrev�ֶ�
	for i in range(2, argc, 1):
		print "fix:", sys.argv[i]
		FixConfigXmlRevNumber(sys.argv[i], ReposRevNumber)