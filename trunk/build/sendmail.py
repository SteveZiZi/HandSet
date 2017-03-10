# -*- coding: cp936 -*-

import sys
import os


    
# python sendmail.py ..\setup\deploy_release\flashdisk2\dsm\change_log.txt ..\setup\deploy_release_pkg
# python sendmail.py  邮件内容文件  附件文件夹
if __name__ == "__main__":

    logfile = sys.argv[1]
    attachdir = sys.argv[2]
    sendwho = ["781400513@qq.cn"]
    
    body_argv = " " + logfile + " -to "
    subject_argv = ""
    attach_argv = ""
    
    # 获得所有附件
    for root,dirs,files in os.walk(attachdir):
        for filename in files:
            fullpath = os.path.join(root, filename)
            subject_argv = " -subject \"the new dsm " + filename + "\" "
            attach_argv = attach_argv + " -attach " + fullpath
            

    for mailaddr in sendwho:
        cmd = "blat" + body_argv + mailaddr + subject_argv + attach_argv
        print cmd
        os.system(cmd)
            

        
        