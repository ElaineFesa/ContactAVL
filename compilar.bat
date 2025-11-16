@echo off
chcp 65001 > nul
echo ========================================
echo    COMPILADOR AGENDA AVL - CONSOLE
echo ========================================

echo Compilando contato.cpp...
g++ -c src/contact.cpp -Iinclude -std=c++17 -o contact.o

echo Compilando programa principal...
g++ -c src/main_console.cpp -Iinclude -std=c++17 -o main_console.o

echo Linkando executável...
g++ main_console.o contact.o -o agenda_avl.exe

if %errorlevel% equ 0 (
    echo.
    echo ✅ COMPILAÇÃO BEM-SUCEDIDA!
    echo.
    echo 🚀 Execute: agenda_avl.exe
    echo.
) else (
    echo.
    echo ❌ ERRO NA COMPILAÇÃO!
    echo.
)

del *.o 2>nul
echo.
pause