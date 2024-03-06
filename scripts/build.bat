@echo off

cd ..

echo.
echo ::Build ninja
cmake -S . -B build/ninja/ -GNinja

echo.
echo ::Gathering compilation database information!
copy build\ninja\compile_commands.json .

echo.
echo ::Build windows
cmake -S . -B build/windows/
