rem
rem 部署应用程序到板子, 按目录安排好, 
rem 需要先pre_build.bat, 然后生成dsm工程, 最后执行这个部署
rem

rem
rem 建立部署的目录
rem
del .\deploy_debug\*.* /s /q
rmdir .\deploy_debug /s /q
md .\deploy_debug

md .\deploy_debug\FlashDisk2
md .\deploy_debug\FlashDisk2\DSTester

del .\deploy_release\*.* /s /q
rmdir .\deploy_release /s /q
md .\deploy_release

md .\deploy_release\FlashDisk2
md .\deploy_release\FlashDisk2\DSTester


rem
rem 拷贝运行时目录结构, 包括所有配置文件
rem
xcopy ..\bin\debug_unicode\*.*    .\deploy_debug\FlashDisk2\DSTester /e /y /EXCLUDE:deploy_uncopy.txt
xcopy ..\bin\release_unicode\*.*  .\deploy_release\FlashDisk2\DSTester /e /y /EXCLUDE:deploy_uncopy.txt


rem
rem 移动资源文件, 按真实的目录布局
rem
move .\deploy_debug\FlashDisk2\DSTester\ImageView_d.exe    .\deploy_debug\FlashDisk2\ImageView.exe
move .\deploy_debug\FlashDisk2\DSTester\MemInfo_d.exe      .\deploy_debug\FlashDisk2\MemInfo.exe
move .\deploy_debug\FlashDisk2\DSTester\DSTester_d.exe     .\deploy_debug\FlashDisk2\DSTester\DSTester.exe

move .\deploy_release\FlashDisk2\DSTester\ImageView.exe    .\deploy_release\FlashDisk2\ImageView.exe
move .\deploy_release\FlashDisk2\DSTester\MemInfo.exe      .\deploy_release\FlashDisk2\MemInfo.exe


rem
rem 拷贝一级loader程序
rem
xcopy ..\tools\loader\bin\*.*  .\deploy_debug\FlashDisk2\loader\ /e /y
xcopy ..\tools\loader\bin\*.*  .\deploy_release\FlashDisk2\loader\ /e /y

rem
rem 拷贝二级loader程序
rem
rem xcopy ..\tools\loader\bin\*.*  .\deploy_debug\FlashDisk2\DSTester\loader\ /e /y
rem xcopy ..\tools\loader\bin\*.*  .\deploy_release\FlashDisk2\DSTester\loader\ /e /y

rem
rem 拷贝DeviceUpdater程序
rem
rem xcopy ..\tools\DeviceUpdater\bin\*.*  .\deploy_debug\FlashDisk2\DSTester\DeviceUpdater\ /e /y
rem xcopy ..\tools\DeviceUpdater\bin\*.*  .\deploy_release\FlashDisk2\DSTester\DeviceUpdater\ /e /y

rem xcopy ..\tools\FpgaUpdater\bin\*.*  .\deploy_debug\FlashDisk2\DSTester\DeviceUpdater\ /e /y
rem xcopy ..\tools\FpgaUpdater\bin\*.*  .\deploy_release\FlashDisk2\DSTester\DeviceUpdater\ /e /y


rem
rem 拷贝备用应用程序
rem
rem xcopy .\deploy_debug\FlashDisk2\DSTester    .\deploy_debug\FlashDisk2\loader\DSTester_BAK\ /e /y
rem xcopy .\deploy_release\FlashDisk2\DSTester  .\deploy_release\FlashDisk2\loader\DSTester_BAK\ /e /y


rem
rem 删除无用的文件
rem
