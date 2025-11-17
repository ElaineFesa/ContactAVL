@echo off
chcp 65001 > nul
echo ========================================
echo    COMPILADOR AGENDA AVL - WEB
echo ========================================

echo Compilando contact.cpp...
g++ -c src\contact.cpp -Iinclude -std=c++17 -o contact.o

if %errorlevel% neq 0 (
    echo ERRO: Falha ao compilar contact.cpp
    goto error
)

echo Compilando servidor web...
g++ -c src\simple_server.cpp -Iinclude -std=c++17 -o server.o

if %errorlevel% neq 0 (
    echo ERRO: Falha ao compilar server.cpp
    goto error
)

echo Linkando servidor...
g++ server.o contact.o -o agenda_web.exe -lws2_32

if %errorlevel% equ 0 (
    echo.
    echo COMPILACAO BEM-SUCEDIDA!
    echo.
    echo Execute: agenda_web.exe
    echo Acesse: http://localhost:8080
    echo.
) else (
    goto error
)

del *.o 2>nul
goto end

:error
echo.
echo ERRO NA COMPILACAO!
echo.
del *.o 2>nul

:end
pause