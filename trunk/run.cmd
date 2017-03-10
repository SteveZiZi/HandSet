rem
rem 为了配合知识管理部门的自动编译系统, 必须把run.cmd放到trunk目录下
rem

rem 进入当前bat的目录
%~d0
cd %~dp0

cd build
call run.cmd
cd %~dp0

pause
