@echo off
setlocal enabledelayedexpansion

:: --- HARDCODED PATHS ---
set "EDITOR=C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe"
set "PROJECT=C:\Users\Raamxie\Documents\Unreal Projects\BGEN\BGEN\BGEN.uproject"

:: --- PARAMETER PARSING ---
:: Grab the first argument (e.g., "-17" or "17")
set "RAW_COUNT=%~1"

:: Remove the dash if you typed "-17" so the loop works properly
set "COUNT=%RAW_COUNT:-=%"

:: Shift the arguments down (so %2 becomes %1, %3 becomes %2, etc.)
SHIFT

:: Loop through all remaining arguments and combine them into one string
set "ARGS="
:CollectArgs
if "%~1"=="" goto DoneArgs
set "ARGS=%ARGS% %1"
SHIFT
goto CollectArgs
:DoneArgs

echo =================================================
echo        LAUNCHING GENETIC SWARM
echo        Instances: %COUNT%
echo        Switches: %ARGS%
echo =================================================

:: Loop from 1 to COUNT
for /L %%i in (1,1,%COUNT%) do (
    echo [%%i/%COUNT%] Spawning Island %%i...
    
    :: Launch the instance with the passed arguments
    start "" "%EDITOR%" "%PROJECT%" -InstanceID=%%i %ARGS%
    
    :: Wait 1 second between launches
    timeout /t 1 >nul
)

echo.
echo Swarm Deployed. You can close this terminal now.
pause