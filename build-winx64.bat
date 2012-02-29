cd ..
mkdir tank-build-x64
cd tank-build-x64
if "%1"=="Debug" mkdir Debug & cd Debug
if "%1"=="Release" mkdir Release & cd Release
cmake -DCMAKE_BUILD_TYPE=%1 ../../tank 
nmake
cd ../../tank