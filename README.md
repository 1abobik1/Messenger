## Сборка бэкэнда с помощью CMake(нужно поменять локальные пути для библиотек в CMakeLists.txt)
```powershell
mkdir build
cd build
cmake ..
cmake --build .
cd DEBUG
./ChatServer.exe
