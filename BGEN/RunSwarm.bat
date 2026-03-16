@echo off
setlocal enabledelayedexpansion

:: --- HARDCODED PATHS ---
set "EDITOR=C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe"
:: NEW: Point specifically to the command-line version for the Master Server
set "EDITOR_CMD=C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
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
echo        LAUNCHING GENETIC CENTRAL SERVER
echo =================================================
:: 1. Launch the Server Commandlet asynchronously using the CMD executable.
:: Added lightweight flags to completely disable rendering, audio, and background shader compiling.
start "Genetic Central Server" "%EDITOR_CMD%" "%PROJECT%" -run=GeneticServer -NoUI -NoSound -NullRHI -NoShaderCompile -NoTextureStreaming -AllowCommandletRendering=false -nosplash -FORCELOGFLUSH -log

:: 2. Give the server a few seconds to boot up and bind to port 8080 
:: before the swarm starts hammering it with HTTP requests.
echo Waiting 3 seconds for the server to initialize...
timeout /t 3 >nul

echo =================================================
echo        LAUNCHING GENETIC SWARM
echo        Instances: %COUNT%
echo        Switches: %ARGS%
echo =================================================

:: Loop from 1 to COUNT
for /L %%i in (1,1,%COUNT%) do (
    echo [%%i/%COUNT%] Spawning Island %%i...
    
    :: Launch the instance with the passed arguments (Workers use standard Editor)
    start "Swarm_Island_%%i" "%EDITOR%" "%PROJECT%" -InstanceID=%%i %ARGS%
    
    :: Wait 1 second between launches to prevent CPU choking during boot
    timeout /t 1 >nul
)

echo.
echo Swarm Deployed. You can close this terminal now.
pause