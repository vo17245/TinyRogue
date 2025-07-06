cd ..
cmake  -B BuildDebug -S .    -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 17 2022" -A x64 -DAETHER_RUNTIME_CHECK=1 -DAETHER_ENABLE_DEBUG_LOG=1 
cmake  -B DummyBuildDebug -S .   -DCMAKE_EXPORT_COMPILE_COMMANDS=on -DCMAKE_BUILD_TYPE=Debug -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DAETHER_RUNTIME_CHECK=1 -DAETHER_ENABLE_DEBUG_LOG=1 
cp DummyBuildDebug/compile_commands.json .
cd Scripts
pause
