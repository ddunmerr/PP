@echo off
setlocal enabledelayedexpansion

rem ============================================================
rem  Прогон всех замеров: N = 1..16, ядра = 1..4, 5 повторов.
rem  Результат — results.csv в текущей папке.
rem
rem  Использование:
rem     run_all.bat <input.bmp> [radius]
rem ============================================================

if "%~1"=="" (
    echo usage: %~nx0 ^<input.bmp^> [radius]
    exit /b 1
)

set "INPUT=%~1"
set "RADIUS=%~2"
if "%RADIUS%"=="" set "RADIUS=15"

set "EXE=build/Release/blur.exe"
if not exist "%EXE%" (
    echo error: %EXE% not found
    exit /b 1
)

set "OUT=results.csv"
set "TMP=out_tmp.bmp"
set "REPEATS=5"

rem Заголовок CSV
echo threads,radius,hardware_concurrency,ms> "%OUT%"

rem Маски: 1, 3, 7, F  -> 1, 2, 3, 4 ядра
set "MASKS=1 3 7 F"

for %%M in (%MASKS%) do (
    for /L %%N in (1,1,16) do (
        for /L %%R in (1,1,%REPEATS%) do (
            rem start /wait /b /affinity <mask> blur.exe in out N radius
            rem Вывод программы (CSV-строка) летит в stdout, ловим через for /f.
            for /f "usebackq tokens=*" %%L in (
                `start /wait /b /affinity %%M %EXE% "%INPUT%" "%TMP%" %%N %RADIUS%`
            ) do (
                echo %%L>> "%OUT%"
            )
        )
    )
)

echo done: %OUT%
endlocal