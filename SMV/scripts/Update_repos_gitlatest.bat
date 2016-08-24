@echo off

:: batch file used to update Windows, Linux and OSX GIT repos

set envfile="%userprofile%"\fds_smv_env.bat
IF EXIST %envfile% GOTO endif_envexist
echo ***Fatal error.  The environment setup file %envfile% does not exist. 
echo Create a file named %envfile% and use SMV/scripts/fds_smv_env_template.bat
echo as an example.
echo.
echo Aborting now...
pause>NUL
goto:eof

:endif_envexist

:: location of batch files used to set up Intel compilation environment

call %envfile%

echo.
echo ------------------------------------------------------------------------
echo Updating the Windows Git repository, %svn_root%, to the latest revision

%svn_drive%
cd %svn_root%
echo Updating the repo:%svn_root%
git remote update
git checkout development
git merge origin/development
git merge firemodels/development
git describe --dirty

echo.
echo ------------------------------------------------------------------------
echo Updating the Windows Git repository, %svn_webroot%, to the latest revision

%svn_drive%
cd %svn_webroot%
echo Updating the repo:%svn_webroot%
git remote update
git checkout nist-pages
git merge origin/nist-pages
git describe --dirty

set scriptdir=%linux_svn_root%/Utilities/Scripts/
set linux_fdsdir=%linux_svn_root%

echo.
echo ------------------------------------------------------------------------
echo Updating the Linux Git repository, %linux_svn_root%, on %linux_hostname% to the latest revision
plink %linux_logon% %scriptdir%/UPDATE_thishost.sh  %linux_svn_root% development %linux_hostname%

echo.
echo ------------------------------------------------------------------------
echo Updating the Linux Git repository, %linux_svn_webroot%, on %linux_hostname% to the latest revision
plink %linux_logon% %scriptdir%/UPDATE_thishost.sh  %linux_svn_webroot% nist-pages %linux_hostname%

echo.
echo ------------------------------------------------------------------------
echo Updating the OSX GIT repository, %linux_svn_root%, on %osx_hostname% to the latest revision
plink %osx_logon% %scriptdir%/UPDATE_latest_fds_onhost.csh  %linux_svn_root% development %osx_hostname%

echo.
echo ------------------------------------------------------------------------
echo Updating the OSX GIT repository, %linux_svn_webroot%, on %osx_hostname% to the latest revision
plink %osx_logon% %scriptdir%/UPDATE_latest_fds_onhost.csh  %linux_svn_webroot% nist-pages %osx_hostname%

pause
