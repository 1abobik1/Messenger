# Messenger

Мессенджер с обменом сообщениями в реальном времени: сервер на C++20 и [uWebSockets](https://github.com/uNetworking/uWebSockets),
данные в PostgreSQL, клиент на React.

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![uWebSockets](https://img.shields.io/badge/uWebSockets-20-orange)
![PostgreSQL](https://img.shields.io/badge/PostgreSQL-16-336791)
![React](https://img.shields.io/badge/React-18-61DAFB)
![Docker](https://img.shields.io/badge/Docker-compose-2496ED)

<!-- demo: перетащите GIF в редактор README на GitHub — он загрузится в user-attachments, а не в репозиторий -->

## Возможности

- **Регистрация и вход**: пароли хешируются bcrypt на сервере, вход по токену сессии.
- **Поиск пользователей** по email и **друзья** (дружба взаимная).
- **Личные сообщения в реальном времени** через WebSocket: сообщение сразу видно собеседнику и во всех ваших вкладках.
- **История переписки** с постраничной загрузкой.
- **Антиспам**: не больше 8 сообщений за 10 секунд, иначе отправка блокируется на 30 секунд.
- **Запуск одной командой** через Docker Compose.

## Как это устроено

```mermaid
flowchart LR
    B[Браузер<br/>React] -- "HTTP /api/*<br/>Authorization: Bearer" --> S[C++ сервер<br/>uWebSockets]
    B <-- "WebSocket /ws?token=…" --> S
    S -- libpq --> D[(PostgreSQL)]
```

- **HTTP API** — регистрация, вход, поиск, друзья, история сообщений. Пользователь определяется только по токену
  сессии, поэтому чужую переписку не прочитать, подставив чужой id в запрос.
- **WebSocket** — отправка и доставка сообщений. Каждое соединение подписано на тему `user:<id>`: сообщение публикуется
  в темы получателя и отправителя, так его видят все открытые вкладки обоих.
- **Сессии** — случайный токен (32 байта); в базе хранится только его SHA-256, срок жизни по умолчанию — 7 дней.
- Сервер однопоточный (цикл событий uWebSockets) и работает с базой через одно соединение, которое
  переподключается, если PostgreSQL перезапустился.

## Быстрый старт (Docker)

```bash
cp .env.example .env     # необязательно: без .env используются значения по умолчанию
docker compose up --build
```

Откройте http://localhost:3000, зарегистрируйте двух пользователей в разных браузерах (или в обычном и приватном окне),
найдите друг друга по email и напишите сообщение.

| Сервис | Адрес |
|---|---|
| Веб-клиент | http://localhost:3000 |
| API и WebSocket | http://localhost:9000, ws://localhost:9000/ws |
| PostgreSQL | внутри сети Docker, схема создаётся из `db/schema.sql` при первом запуске |

## Сборка без Docker

### База данных

```bash
createdb messenger
psql -d messenger -f db/schema.sql   # схема идемпотентна, её можно применять повторно
```

### Сервер

Нужны компилятор с C++20 (проверено на GCC 12 и 15), CMake 3.20+, git и libpq.
uWebSockets, nlohmann/json и GoogleTest CMake скачивает сам.

```bash
# Ubuntu/Debian: sudo apt install cmake g++ git libpq-dev
# macOS:         brew install cmake libpq  и добавить к cmake -DPostgreSQL_ROOT="$(brew --prefix libpq)"
cmake -S server -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build                 # юнит-тесты, база не нужна

DATABASE_URL=postgresql://user:password@localhost:5432/messenger ./build/messenger-server
```

Сборка проверена на Linux. На Windows проще всего запускать через Docker Desktop или WSL:
нативной сборке uSockets нужен libuv, и в `CMakeLists.txt` этот вариант пока не настроен.

### Клиент

```bash
cd front-end
npm ci
npm start                              # http://localhost:3000, API ожидается на http://localhost:9000
```

Другой адрес API задаётся при сборке: `REACT_APP_API_URL=https://api.example.com npm run build`.

## Настройки сервера

| Переменная | По умолчанию | Назначение |
|---|---|---|
| `DATABASE_URL` | — (обязательна) | строка подключения libpq или `postgresql://…` |
| `MESSENGER_PORT` | `9000` | порт HTTP и WebSocket |
| `CORS_ORIGIN` | `http://localhost:3000` | адрес клиента, которому разрешено обращаться к API |
| `SESSION_TTL_HOURS` | `168` | срок жизни сессии в часах |

## API

Все запросы, кроме регистрации и входа, требуют заголовок `Authorization: Bearer <token>`.
Ошибки возвращаются как `{"error": "описание"}` с кодом 400, 401, 404, 409 или 500.

| Метод | Путь | Тело / параметры | Ответ |
|---|---|---|---|
| POST | `/api/auth/signup` | `{username, email, password}` | `201 {token, user}` |
| POST | `/api/auth/login` | `{email, password}` | `200 {token, user}` |
| POST | `/api/auth/logout` | — | `204` |
| GET | `/api/me` | — | `{id, username, email}` |
| GET | `/api/users/search` | `?email=` | `{id, username}` или 404 |
| GET | `/api/users/:id` | — | `{id, username}` или 404 |
| GET | `/api/friends` | — | `[{id, username}]` |
| POST | `/api/friends` | `{friend_id}` | `201` (добавлен) или `200` (уже в друзьях) |
| GET | `/api/messages/:peerId` | `?limit=1..100&before=<id>` | сообщения, новые первыми |
| GET | `/api/health` | — | `{"status":"ok"}` или 503 |

Ограничения: имя — 1–20 символов и не только цифры, пароль — от 10 символов (не больше 72 байт, ограничение bcrypt),
сообщение — 1–4000 символов.

### WebSocket `/ws?token=<token>`

Клиент отправляет:

```json
{"type": "message", "to": 2, "body": "Привет!"}
```

Сервер присылает сохранённое сообщение получателю, отправителю и во все их вкладки:

```json
{"type": "message", "message": {"id": 15, "from": 1, "to": 2, "body": "Привет!", "sent_at": "2024-06-14T10:00:00.123Z"}}
```

или ошибку, после которой соединение остаётся открытым:

```json
{"type": "error", "code": "rate_limited", "message": "too many messages, you are blocked for a while", "retry_after": 30}
```

Коды ошибок: `bad_request`, `unknown_type`, `not_found`, `rate_limited`, `internal_error`.

## Структура проекта

```
server/
  src/main.cpp            точка входа: настройки → runServer()
  src/server.cpp          сборка зависимостей и запуск uWebSockets
  src/api.cpp             маршруты HTTP API
  src/chat.cpp            WebSocket: доставка сообщений, антиспам
  src/auth_service.cpp    регистрация, вход, сессии
  src/http.cpp            чтение JSON-тела, ответы, CORS
  src/db/                 соединение с PostgreSQL и репозитории
  src/rate_limiter.cpp    скользящее окно для антиспама
  src/validation.cpp      проверки имени, email, пароля, сообщения
  tests/                  юнит-тесты (GoogleTest)
  third_party/bcrypt/     bcrypt (hilch/Bcrypt.cpp, BSD)
front-end/
  src/api.js              обращения к API
  src/auth/               сессия и защищённые маршруты
  src/hooks/useChatSocket.js  WebSocket с переподключением
  src/components/         интерфейс
db/schema.sql             схема базы
docker-compose.yml        PostgreSQL + сервер + клиент
```

## Ограничения

- В конфигурации для разработки нет HTTPS: для настоящего развёртывания нужен обратный прокси (nginx, Caddy) с TLS.
- Токен хранится в `localStorage` браузера.
- Сервер однопоточный и обращается к базе синхронно — для учебного проекта этого достаточно, но под нагрузкой
  запросы к базе будут задерживать остальные.
- Сообщения хранятся в открытом виде, сквозного шифрования нет.
- Данные старой версии (до перехода на `db/schema.sql`) не переносятся: схема базы изменилась.

## Что можно улучшить

- Список чатов с последним сообщением и счётчиком непрочитанных.
- Статус «в сети» и «печатает…», отметки о прочтении.
- Заявки в друзья вместо мгновенного добавления.
- Групповые чаты.
- Переход клиента с Create React App на Vite.

## Авторы

- **Дмитрий** ([@1abobik1](https://github.com/1abobik1)) — сервер, база данных.
- **Вячеслав** ([@Juryxa](https://github.com/Juryxa)) — клиент на React.
