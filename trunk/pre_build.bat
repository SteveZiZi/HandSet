rem
rem �������Ŀ¼
rem
md .\lib
md .\bin

del .\bin\debug_unicode\*.* /s /q
md .\bin\debug_unicode

del .\bin\release_unicode\*.* /s /q
md .\bin\release_unicode

del .\bin\win32\debug\*.* /s /q
md .\bin\win32\debug

del .\bin\win32\release\*.* /s /q
md .\bin\win32\release

rem
rem ���������޸���־
rem
rem copy .\change_log.txt  .\bin\debug_unicode
rem copy .\change_log.txt  .\bin\release_unicode

rem
rem ��������ʱĿ¼�ṹ, �������������ļ�
rem
xcopy .\runtime_config\*.*  .\bin\debug_unicode /e /y
xcopy .\runtime_config\*.*  .\bin\release_unicode /e /y

xcopy .\runtime_config_win32\*.*  .\bin\win32\debug /e /y
xcopy .\runtime_config_win32\*.*  .\bin\\win32\release /e /y


rem
rem ������������ - dsploop
rem
copy .\third_party\dsploop\lib\Loop.lib  .\lib
copy .\third_party\dsploop\bin\Loop.dll  .\bin\debug_unicode
copy .\third_party\dsploop\bin\Loop.dll  .\bin\release_unicode