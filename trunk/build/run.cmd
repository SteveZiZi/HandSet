@echo set vs variable

@set sln=.\sln\dsm.sln

@set CR_VC_VER=8.0
@set CR_BUILD_PLATFROM=ARM
@set CR_ERR_INIT_COLOR=4F
@set path=%path%;D:\Python27

@echo check Visual C++ %CR_VC_VER%
@call :GetVCDir
@if "%CR_VC_INSTALL_DIR%"=="" (
@echo Visual C++ %CR_VC_VER% not install
goto err_exit
)

@echo %CR_VC_INSTALL_DIR%vcvarsall.bat
@call "%CR_VC_INSTALL_DIR%vcvarsall.bat


@if "%1"=="debug" (

    @echo ==pre_build==
    cd ..\
    call pre_build.bat
    cd %~dp0


    @echo ==%sln%==
    del .\intermediate\*.* /s /q
    devenv %sln% /ReBuild  "Debug|EPCS-9000 (ARMV4I)"
    devenv %sln% /Build  "Debug|EPCS-9000 (ARMV4I)"


    @echo ==deploy==
    cd ..\Setup
    call deploy.bat
    cd %~dp0
    
    @echo ==build_rev==
    python build_rev.py ./ ..\setup\deploy_debug\flashdisk2\DSTester\info.xml

    @echo ==fixversion==
    python fixversion.py ..\setup\deploy_debug\flashdisk2\DSTester\info.xml ..\setup\deploy_debug\flashdisk2\DSTester\DSTester.exe ..\setup\deploy_debug\flashdisk2\DSTester\KeyboardAux.exe

) else (

    @echo ==pre_build==
    cd ..\
    call pre_build.bat
    cd %~dp0


    @echo ==%sln%==
    del .\intermediate\*.* /s /q
    devenv %sln% /ReBuild  "Release|EPCS-9000 (ARMV4I)"
    devenv %sln% /Build  "Release|EPCS-9000 (ARMV4I)"


    @echo ==deploy==
    cd ..\Setup
    call deploy.bat
    cd %~dp0


    @echo ==build_rev==
    python build_rev.py ./ ..\setup\deploy_release\flashdisk2\DSTester\info.xml

    @echo ==fixversion==
    python fixversion.py ..\setup\deploy_release\flashdisk2\DSTester\info.xml ..\setup\deploy_release\flashdisk2\DSTester\dsm.exe ..\setup\deploy_release\flashdisk2\DSTester\KeyboardAux.exe

    @echo ==pkg==
    python install_package.py ..\setup\deploy_release ..\setup\deploy_release\flashdisk2\DSTester\info.xml

    @echo ==sendmail==
    python sendmail.py ..\setup\deploy_release\flashdisk2\dsm\change_log.txt ..\setup\deploy_release_pkg

)


@echo 
@goto end

:GetVCDir
@echo GetVCDir
@call :GetVCDirHelper HKLM > nul 2>&1
@if errorlevel 1 call :GetVCDirHelper HKCU > nul 2>&1
@exit /B 0

:GetVCDirHelper
@echo GetVCDirHelper
@if %PROCESSOR_ARCHITECTURE:~-1%==4 (set "ZCLIENT_REG_ARCH_KEY=Wow6432Node\" ) else "ZCLIENT_REG_ARCH_KEY="
@for /F "tokens=1,2*" %%i in ('reg query "%1\SOFTWARE\%ZCLIENT_REG_ARCH_KEY%Microsoft\VisualStudio\%CR_VC_VER%\Setup\VC" /v "ProductDir"') DO (
	if "%%i"=="ProductDir" (
		SET "CR_VC_INSTALL_DIR=%%k"
	)
)

:end 
@echo on
