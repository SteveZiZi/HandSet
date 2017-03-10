# -*- coding: cp936 -*-

import sys
import os
import pysvn
import xml.dom.minidom
import os.path,re,time


# 将xml的build字段的数字替换
def FixConfigXmlRevNumber( infoXml, oemXml ):
	# 读取xml文件
	xmldoc = xml.dom.minidom.parse(infoXml)
	oemdoc = xml.dom.minidom.parse(oemXml)

	# 找到目标node
	config_nodes = xmldoc.getElementsByTagName("config")
	oem_config = oemdoc.getElementsByTagName("config")

	# 替换product属性值
	product_nodes = config_nodes[0].getElementsByTagName("product")
	oem_product = oem_config[0].getElementsByTagName("product")
	product_nodes[0].attributes["val"].value = oem_product[0].attributes["val"].value

	# 替换company属性值
	company_nodes = config_nodes[0].getElementsByTagName("company")
	oem_company = oem_config[0].getElementsByTagName("company")
	company_nodes[0].attributes["val"].value = oem_company[0].attributes["val"].value
    
	# 替换address属性值
	address_nodes = config_nodes[0].getElementsByTagName("address")
	oem_address = oem_config[0].getElementsByTagName("address")
	address_nodes[0].attributes["val"].value = oem_address[0].attributes["val"].value

	# 替换telephone属性值
	telephone_nodes = config_nodes[0].getElementsByTagName("telephone")
	oem_telephone = oem_config[0].getElementsByTagName("telephone")
	telephone_nodes[0].attributes["val"].value = oem_telephone[0].attributes["val"].value

	# 替换softwarever属性值
	softwarever_nodes = config_nodes[0].getElementsByTagName("softwarever")
	oem_version = oem_config[0].getElementsByTagName("oemVersion")
	version = softwarever_nodes[0].attributes["val"].value.split('.')
	oem_version_str = version[0] + '.' + version[1] + '.' + oem_version[0].attributes["val"].value 
	softwarever_nodes[0].attributes["val"].value = oem_version_str

	# 保存xml文件
	writeFile = open(infoXml, 'w')
	writeFile.write(xmldoc.toxml('utf-8'))

	
# python oem.py ../bin/release_unicode/info.xml ../oem/NSAR558/info.xml
# python oem.py  目标info.xml                   oem info.xml
if __name__ == "__main__":

	# 获得参数个数
	argc = len(sys.argv)

	if (argc == 3) :
		FixConfigXmlRevNumber(sys.argv[1], sys.argv[2])