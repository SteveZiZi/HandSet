rem
rem ����Ӧ�ó��򵽰���, ��Ŀ¼���ź�, 
rem ��Ҫ��pre_build.bat, Ȼ������dsm����, ���ִ���������
rem

rem
rem ���������Ŀ¼
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
rem ��������ʱĿ¼�ṹ, �������������ļ�
rem
xcopy ..\bin\debug_unicode\*.*    .\deploy_debug\FlashDisk2\DSTester /e /y /EXCLUDE:deploy_uncopy.txt
xcopy ..\bin\release_unicode\*.*  .\deploy_release\FlashDisk2\DSTester /e /y /EXCLUDE:deploy_uncopy.txt


rem
rem �ƶ���Դ�ļ�, ����ʵ��Ŀ¼����
rem
move .\deploy_debug\FlashDisk2\DSTester\ImageView_d.exe    .\deploy_debug\FlashDisk2\ImageView.exe
move .\deploy_debug\FlashDisk2\DSTester\MemInfo_d.exe      .\deploy_debug\FlashDisk2\MemInfo.exe
move .\deploy_debug\FlashDisk2\DSTester\DSTester_d.exe     .\deploy_debug\FlashDisk2\DSTester\DSTester.exe

move .\deploy_release\FlashDisk2\DSTester\ImageView.exe    .\deploy_release\FlashDisk2\ImageView.exe
move .\deploy_release\FlashDisk2\DSTester\MemInfo.exe      .\deploy_release\FlashDisk2\MemInfo.exe


rem
rem ����һ��loader����
rem
xcopy ..\tools\loader\bin\*.*  .\deploy_debug\FlashDisk2\loader\ /e /y
xcopy ..\tools\loader\bin\*.*  .\deploy_release\FlashDisk2\loader\ /e /y

rem
rem ��������loader����
rem
rem xcopy ..\tools\loader\bin\*.*  .\deploy_debug\FlashDisk2\DSTester\loader\ /e /y
rem xcopy ..\tools\loader\bin\*.*  .\deploy_release\FlashDisk2\DSTester\loader\ /e /y

rem
rem ����DeviceUpdater����
rem
rem xcopy ..\tools\DeviceUpdater\bin\*.*  .\deploy_debug\FlashDisk2\DSTester\DeviceUpdater\ /e /y
rem xcopy ..\tools\DeviceUpdater\bin\*.*  .\deploy_release\FlashDisk2\DSTester\DeviceUpdater\ /e /y

rem xcopy ..\tools\FpgaUpdater\bin\*.*  .\deploy_debug\FlashDisk2\DSTester\DeviceUpdater\ /e /y
rem xcopy ..\tools\FpgaUpdater\bin\*.*  .\deploy_release\FlashDisk2\DSTester\DeviceUpdater\ /e /y


rem
rem ��������Ӧ�ó���
rem
rem xcopy .\deploy_debug\FlashDisk2\DSTester    .\deploy_debug\FlashDisk2\loader\DSTester_BAK\ /e /y
rem xcopy .\deploy_release\FlashDisk2\DSTester  .\deploy_release\FlashDisk2\loader\DSTester_BAK\ /e /y


rem
rem ɾ�����õ��ļ�
rem
