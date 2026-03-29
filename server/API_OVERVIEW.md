# API Сервера QT-TCP-Server

## Общие сведения

**Хост:** любой (слушает на всех интерфейсах)  
**Порт:** 44444  
**Формат команд:** `COMMAND&ARG1&ARG2&...` (разделитель `&`)  
**Формат ответа:** `RESPONSE_TYPE&DATA\r\n` (завершается `\r\n`)

---

## Команды

### 1. AUTH — Авторизация пользователя

**Формат запроса:**
```
auth&LOGIN&PASSWORD
```

**Аргументы:**
- `LOGIN` — логин пользователя (строка)
- `PASSWORD` — пароль пользователя (строка)

**Ответ при успехе:**
```
auth_success&TOKEN
```
где `TOKEN` — уникальный токен для использования защищённых функций (действует 30 минут)

**Ответ при ошибке:**
```
auth_error
```

---

### 2. REG — Регистрация пользователя

**Формат запроса:**
```
reg&LOGIN&PASSWORD
```

**Аргументы:**
- `LOGIN` — новый логин (строка)
- `PASSWORD` — пароль (строка)

**Ответ при успехе:**
```
reg_success
```

**Ответ при ошибке:**
```
reg_error
```

---

### 3. STATS — Получение статистики пользователя

**Формат запроса:**
```
stats&LOGIN&TOKEN
```

**Аргументы:**
- `LOGIN` — логин пользователя (строка)
- `TOKEN` — токен авторизации (строка)

**Ответ при успехе:**
```
stats&STAT1,DATE1&STAT2,DATE2&...
```
где каждый элемент `STATx,DATEx` — статистика и дата (разделитель `&`)

**Ответы при ошибке:**
```
token_expired        — токен истёк
get_stats_error      — ошибка сервера/БД
```

---

### 4. SET_STATS — Сохранение статистики

**Формат запроса:**
```
set_stats&LOGIN&TOKEN&STATS_DATA
```

**Аргументы:**
- `LOGIN` — логин пользователя (строка)
- `TOKEN` — токен авторизации (строка)
- `STATS_DATA` — произвольные данные статистики (строка/JSON)

**Ответ при успехе:**
```
stats&set_stats_success
```

**Ответы при ошибке:**
```
token_expired       — токен истёк
set_stats_error     — ошибка сервера/БД
```

---

### 5. FINDNODESBYDEPTH — Поиск узлов графа на глубине N

**Формат запроса:**
```
findNodesByDepth&LOGIN&TOKEN&START_NODE_ID&GRAPH_JSON
```

**Аргументы:**
- `LOGIN` — логин пользователя (строка)
- `TOKEN` — токен авторизации (строка)
- `START_NODE_ID` — ID начального узла (число)
- `GRAPH_JSON` — граф в формате JSON массива

**Формат GRAPH_JSON:**
```json
[
  [NODE_ID, [[CONNECTED_NODE_ID, WEIGHT], ...] ],
  ...
]
```
Пример:
```json
[[1, [[2, 2], [3, 3]]], [2, [[1, 2], [3, 1]]], [3, [[1, 3], [2, 1]]]]
```
(Для невзвешенного графа используйте вес `1`)

**Ответ при успехе:**
```
findNodesByDepth&NODE_ID1&NODE_ID2&...
```
где найденные узлы на глубине 2 (разделитель `&`)

**Ответы при ошибке:**
```
token_expired            — токен истёк
findNodesByDepth_error   — ошибка парсинга/обработки
```

---

## Обработка ответов (клиентская сторона)

### Алгоритм:

1. **Отправить команду:**
   ```
   socket.write("auth&user123&pass123");
   ```

2. **Получить ответ:**
   ```
   response = socket.read()  // получить данные до \r\n
   ```

3. **Парсить ответ:**
   - Разделить по `&` на части
   - Первая часть — тип ответа (status)
   - Остальные части — данные

4. **Обработать статус:**
   ```
   if response.contains("auth_success"):
       token = response.split("&")[1]
       // сохранить token для дальнейших запросов
   
   if response.contains("token_expired"):
       // требуется новая авторизация
   
   if response.contains("error"):
       // обработать ошибку
   ```

### Коды ответов:

| Код | Значение |
|-----|----------|
| `auth_success` | Авторизация успешна |
| `auth_error` | Ошибка авторизации |
| `reg_success` | Регистрация успешна |
| `reg_error` | Ошибка регистрации |
| `stats` | Статистика получена/сохранена |
| `token_expired` | Токен истёк — нужна переавторизация |
| `get_stats_error` | Ошибка при получении статистики |
| `set_stats_error` | Ошибка при сохранении статистики |
| `findNodesByDepth` | Результат поиска в графе |
| `findNodesByDepth_error` | Ошибка при обработке графа |
| `Unknown_command` | Неизвестная команда |

---

## Жизненный цикл сеанса

1. Клиент подключается к серверу на порт 44444
2. Сервер отправляет приветствие: `"Hello, World!!! I am echo server!\r\n"`
3. Клиент отправляет `auth` для авторизации и получает `TOKEN`
4. Клиент использует `TOKEN` во всех защищённых запросах (`stats`, `set_stats`, `findNodesByDepth`)
5. При истечении 30 минут требуется повторная авторизация
6. Соединение закрывается по инициативе клиента или сервера

---

## Пример использования (псевдокод)

```python
# Подключение
socket = connect("localhost", 44444)
welcome = socket.read()  # "Hello, World!!! I am echo server!\r\n"

# Авторизация
socket.write("auth&user1&pass123")
response = socket.read()  # "auth_success&a1b2c3d4...\r\n"
token = response.split("&")[1]

# Получение статистики
socket.write(f"stats&user1&{token}")
response = socket.read()  # "stats&stat1,2026-03-29&stat2,2026-03-28\r\n"
stats = response.split("&")[1:]

# Сохранение статистики
socket.write(f"set_stats&user1&{token}&new_stat_data")
response = socket.read()  # "stats&set_stats_success\r\n"

# Поиск в графе
graph_json = "[[1,[[2,1],[3,1]]],[2,[[1,1]]],[3,[[1,1]]]]"
socket.write(f"findNodesByDepth&user1&{token}&1&{graph_json}")
response = socket.read()  # "findNodesByDepth&2&3\r\n"
result = response.split("&")[1:]

socket.close()
```
