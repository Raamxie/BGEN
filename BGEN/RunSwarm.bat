@echo off
:: --- CONFIGURATION ---
set "EDITOR=C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe"
set "PROJECT=C:\Users\Raamxie\Documents\Unreal Projects\BGEN\BGEN\BGEN.uproject"
set "ARGS=-game -nullrhi -nosound -benchmark -unattended -novsync -noverifygc -GeneticRun -log"
set COUNT=10

echo =================================================
echo        LAUNCHING GENETIC SWARM (%COUNT% Instances)
echo =================================================

:: Loop from 1 to COUNT
for /L %%i in (1,1,%COUNT%) do (
    echo [%%i/%COUNT%] Spawning Island %%i...
    
    :: "start" launches a separate window and doesn't wait for it to close
    start "" "%EDITOR%" "%PROJECT%" -InstanceID=%%i %ARGS%
    
    :: Wait 1 second between launches to prevent lag
    timeout /t 1 >nul
)

echo.
echo Swarm Deployed. You can close this terminal now.
pause