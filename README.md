## Сборка бэкэнда по этапам:
### 1) настройка CMake(также нужно поменять локальные пути для библиотек в CMakeLists.txt)
```powershell
mkdir build
cd build
cmake ..
cmake --build .
cd DEBUG
./ChatServer.exe
```
### 2) Скачайте файл database_setup.sql из репозитория.
### 3) Восстановите базу данных с именем Messenger_DB с использованием скачанного файла.
### 4) в файле CommonConst.h укажите свои данные constexpr ``` std::string_view CONNECTION_DB = "host=localhost dbname=Messenger_DB user=postgres password=Zopa_kek12"; ```

