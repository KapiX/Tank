cd ..
mkdir tank-build-x86
cd tank-build-x86
if "%1"=="Debug" mkdir Debug & cd Debug
if "%1"=="Release" mkdir Release & cd Release
cmake -DCMAKE_BUILD_TYPE=%1 ../../tank 
nmake
cd ../../tank