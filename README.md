## Сборка бэкэнда пошагово:
### 1) Скачайте [vcpkg](https://github.com/microsoft/vcpkg) с GitHub (подробные инструкции по установке находятся там).
### 2) После успешной установки перейдите в папку, где установлен vcpkg, и скачайте необходимые библиотеки: uwebsockets, nlohmann-json, libpq. Пример:
```
C:\vcpkg>vcpkg.exe install uwebsockets
```
### 3) Восстановите базу данных с именем Messenger_DB, используя файл database_setup.sql в папке create_db.
### 4) Рекомендуется использовать pgAdmin4 -в этом приложении после создания базы данных Messenger_DB, нажмите на нее правой кнопкой мыши, выберите вкладку "Восстановить" и укажите путь до файла database_setup.sql для восстановления.
### 5) В файле CommonConst.h укажите свои данные для подключения к базе данных, например:
```
constexpr std::string_view CONNECTION_DB = "host=localhost dbname=Messenger_DB user=postgres password=Zopa_kek12";
```
### 6) Скачайте [CMake](https://cmake.org/download/) с официального сайта.
### 7) Настройте CMake (не забудьте изменить локальные пути для библиотек uwebsockets и nlohmann-json в файле CMakeLists.txt, строки 32-33).
### 8) Найдите путь установки нашего проекта, скопируйте и выполните следующий код в PowerShell:
```powershell
mkdir build
cd build
cmake ..
cmake --build .
cd DEBUG
./ChatServer.exe
```
### 9) Если все прошло успешно, в терминале появится сообщение "Server is running" - это означает, что все работает корректно.

## Сборка фронтэнда пошагово:
### 1) Скачайте Node.js и npm с официального [сайта](https://nodejs.org/en) (достаточно скачать только Node.js, npm будет установлен вместе с ним).
### 2) Находясь в директории проекта /Messenger, откройте новый PowerShell-терминал и выполните следующие команды:
```powershell
cd front-end
npm start
```
### 3) После успешной установки и запуска откроется регистрационное окно.
