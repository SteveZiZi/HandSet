wince通用小工具

ActiveSyncMSCSwitch.exe  用于切换usb模式

LoadBootCodeDemo.exe 是烧写FPGA的程序

DM8000LogoUpdate.exe  用于替换开机LOGO, 需要拷贝到内存文件夹中运行, 图片文件必须900K以内

epcSysInfoExam.exe  用于查看硬件版本号与nk的版本号


CrashFinder.exe 可以根据串口打印的DataAbort信息, 找到是哪个函数的问题
亲，是否有这种情况：
Exception 'Data Abort' (4): Thread-Id=04a2002a(pth=836385b8), Proc-Id=04a1002a(pprc=833a63c0) 'Dsm_d.exe', VM-active=04a1002a(pprc=833a63c0) 'Dsm_d.exe'
PC=00161d38(Dsm_d.exe+0x00151d38) RA=00012034(Dsm_d.exe+0x00002034) SP=007fed08, BVA=00000000
Exception 'Raised Exception' (-1): Thread-Id=04a2002a(pth=836385b8), Proc-Id=00400002(pprc=81761308) 'NK.EXE', VM-active=04a1002a(pprc=833a63c0) 'Dsm_d.exe'
PC=c006b9f0(k.coredll.dll+0x0001b9f0) RA=80035520(kernel.dll+0x00006520) SP=d4d2f558, BVA=ffffffff


额。发生异常怎么办？
能够定位到错误位置，XX文件的第XX行。
需要用到dll/exe 与 相应的pdb文件, 以及PC=00161d38
用法: New-->Add Image(xxx.exe)-->Find Crash(00161d38)


MulThread:  用于测试wince的浮点运算问题



