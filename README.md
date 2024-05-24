## Сборка бэкэнда по этапам:
### 1) скачайте [vcpkg](https://github.com/microsoft/vcpkg) c github (там есть подробная инструкция по скачиванию) 
### 2) после успешной установки перейдите в папку, где установлен vcpkg и скачайте библиотеки uwebsockets, nlohmann-json, libpq. Пример скачивания
``` C:\vcpkg>vcpkg.exe install uwebsockets ```
### 3) восстановите базу данных с именем Messenger_DB с использованием одного из файлов в папке create_db.
### 4) я использовал pgAdmin4(после создания бд с именем Messenger_DB, нужно нажать на него правой кнопкой мыши, нажать на вкладку восстановить и выбрать путь файла database_setup_method2.sql)
### 5) если при восстановлении вызвалась ошибка, то выполните sql-скрипт в бд Messenger_DB, который лежит в файле database_setup_method1.sql
### 6) в файле CommonConst.h укажите свои данные, пример моих данных: 
``` constexpr std::string_view CONNECTION_DB = "host=localhost dbname=Messenger_DB user=postgres password=Zopa_kek12"; ```
### 7) скачайте [CMake](https://cmake.org/download/) с официального сайта
### 8) настройка CMake(также нужно поменять локальные пути для библиотек(uwebsockets, nlohmann-json) в CMakeLists.txt, в строчках 32-33)
### 9) найдите путь установки нашего проекта, скопируйте код ниже и вставьте его в powershell терминал  
```powershell
mkdir build
cd build
cmake ..
cmake --build .
cd DEBUG
./ChatServer.exe
```
### 10) если всё установилось, то в терминале будет написано Server is running - это означает, что всё корректно работает

## Сборка фронтэнда по этапам:
### 1) скачайте node.js и npm с официального [сайта]((https://nodejs.org/en)) (достаточно скачать только node.js, npm скачается с ним).
### 2) находясь в директории проекта /Messenger введите **обязательно в другом терминале powershell(не в котором запускался бэкэнд)** эти команды:
```powershell
cd front-end
npm start
``` 
### 3) после успешной установки и запуска откроется регистрационное окно
