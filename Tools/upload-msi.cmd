@echo off
REM autorun
SETLOCAL EnableDelayedExpansion

SET ARCH=%1

REM - Check arguments -
if NOT "%1"=="32" (
if NOT "%1"=="64" (
echo "First argument could be architecture, i.e 32 or 64 bits, to upload"
echo "But for now the default of 64 will be assumed."
SET ARCH=64
))

FOR /F "tokens=*" %%g IN ('git rev-parse --abbrev-ref HEAD') do (SET GIT_BRANCH=%%g)
FOR /F "tokens=*" %%h IN ('git rev-parse --verify HEAD')     do (SET GIT_COMMIT=%%h)

SET DEST_FILE=JASP-nightly-%GIT_BRANCH%-%GIT_COMMIT%.msi
echo "Copying MSI to jasp-static with filename : %DEST_FILE%"

if "%ARCH%"=="32" (
	scp ..\..\jasp-wix-installer-%ARCH%\JASP.msi nachtjapon@static.jasp-stats.org:~/Nightlies/Windows32/%DEST_FILE%
) else (
	scp ..\..\jasp-wix-installer-%ARCH%\JASP.msi nachtjapon@static.jasp-stats.org:~/Nightlies/Windows/%DEST_FILE%
)
endlocal
