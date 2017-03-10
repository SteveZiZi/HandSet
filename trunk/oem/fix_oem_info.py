# -*- coding: cp936 -*-

import sys
import os
import pysvn
import xml.dom.minidom
import os.path,re,time


# ��xml��build�ֶε������滻
def FixConfigXmlRevNumber( infoXml, oemXml ):
	# ��ȡxml�ļ�
	xmldoc = xml.dom.minidom.parse(infoXml)
	oemdoc = xml.dom.minidom.parse(oemXml)

	# �ҵ�Ŀ��node
	config_nodes = xmldoc.getElementsByTagName("config")
	oem_config = oemdoc.getElementsByTagName("config")

	# �滻product����ֵ
	product_nodes = config_nodes[0].getElementsByTagName("product")
	oem_product = oem_config[0].getElementsByTagName("product")
	product_nodes[0].attributes["val"].value = oem_product[0].attributes["val"].value

	# �滻company����ֵ
	company_nodes = config_nodes[0].getElementsByTagName("company")
	oem_company = oem_config[0].getElementsByTagName("company")
	company_nodes[0].attributes["val"].value = oem_company[0].attributes["val"].value
    
	# �滻address����ֵ
	address_nodes = config_nodes[0].getElementsByTagName("address")
	oem_address = oem_config[0].getElementsByTagName("address")
	address_nodes[0].attributes["val"].value = oem_address[0].attributes["val"].value

	# �滻telephone����ֵ
	telephone_nodes = config_nodes[0].getElementsByTagName("telephone")
	oem_telephone = oem_config[0].getElementsByTagName("telephone")
	telephone_nodes[0].attributes["val"].value = oem_telephone[0].attributes["val"].value

	# �滻softwarever����ֵ
	softwarever_nodes = config_nodes[0].getElementsByTagName("softwarever")
	oem_version = oem_config[0].getElementsByTagName("oemVersion")
	version = softwarever_nodes[0].attributes["val"].value.split('.')
	oem_version_str = version[0] + '.' + version[1] + '.' + oem_version[0].attributes["val"].value 
	softwarever_nodes[0].attributes["val"].value = oem_version_str

	# ����xml�ļ�
	writeFile = open(infoXml, 'w')
	writeFile.write(xmldoc.toxml('utf-8'))

	
# python oem.py ../bin/release_unicode/info.xml ../oem/NSAR558/info.xml
# python oem.py  Ŀ��info.xml                   oem info.xml
if __name__ == "__main__":

	# ��ò�������
	argc = len(sys.argv)

	if (argc == 3) :
		FixConfigXmlRevNumber(sys.argv[1], sys.argv[2])