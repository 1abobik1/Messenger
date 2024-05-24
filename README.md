## Сборка бэкэнда по этапам:
### 1) скачайте [vcpkg](https://github.com/microsoft/vcpkg) c github (там есть подробная инструкция по скачиванию) 
### 2) после успешной установки перейдите в папку, где установлен vcpkg и скачайте библиотеки uwebsockets, nlohmann-json, libpq. Пример скачивания
``` C:\vcpkg>vcpkg.exe install uwebsockets ```
### 3) восстановите базу данных с именем Messenger_DB с использованием database_setup.sql, который находится в папке create_db.
### 4) я использовал pgAdmin4(после создания бд с именем Messenger_DB, нужно нажать на него правой кнопкой мыши, нажать на вкладку восстановить и выбрать путь файла database_setup.sql)
### 5) в файле CommonConst.h укажите свои данные, пример моих данных: 
``` constexpr std::string_view CONNECTION_DB = "host=localhost dbname=Messenger_DB user=postgres password=Zopa_kek12"; ```
### 6) скачайте CMake с официального [сайта](https://cmake.org/download/)
### 7) настройка CMake(также нужно поменять локальные пути для библиотек(uwebsockets, nlohmann-json) в CMakeLists.txt, в строчках 32-33)
### 8) найдите путь установки нашего проекта, скопируйте код ниже и вставьте его в powershell терминал  
```powershell
mkdir build
cd build
cmake ..
cmake --build .
cd DEBUG
./ChatServer.exe
```
### 9) если всё устновилось, то в терминале будет написано Server is running - это означает, что всё корректно работает
