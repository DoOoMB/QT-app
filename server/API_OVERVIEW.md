**Общие сведения**

- **Хост:** любой (слушает на всех интерфейсах)  
- **Порт:** 44444  
- **Протокол:** TCP (чтение/запись текстовых команд)  
- **Формат команд:** `COMMAND&ARG1&ARG2&...` (разделитель `&`)  
- **Ответы сервера:** текстовая строка, всегда завершается `\r\n`  
- **TTL токена:** 1800 секунд (30 минут) — см. dbmanager.cpp  
- **Где смотреть реализацию:** mytcpserver.cpp, serverfunctionsmanager.cpp, dbmanager.cpp

**Общий принцип**
- Клиент отправляет одну текстовую команду (UTF-8), сервер парсит строку по символу `&` и вызывает соответствующую функцию. Ответы формируются в формате `TYPE&DATA...\r\n`.

**Команды (эндпоинты)**

- **auth:** Авторизация  
  - Формат запроса: `auth&LOGIN&PASSWORD`  
  - Аргументы: `LOGIN` (строка), `PASSWORD` (строка)  
  - Успех: `auth_success&TOKEN\r\n` — `TOKEN` генерируется в DBManager::generateToken  
  - Ошибки: `auth_args_error\r\n` (неверное число аргументов), `auth_error\r\n` (включая ошибки БД или неверные креды)

- **reg:** Регистрация  
  - Формат запроса: `reg&LOGIN&PASSWORD`  
  - Аргументы: `LOGIN` (строка), `PASSWORD` (строка)  
  - Успех: `reg_success\r\n`  
  - Ошибки: `reg_args_error\r\n` (неверное число аргументов), `reg_error\r\n` (включая существующий логин, ошибки БД)

- **stats:** Получение статистики пользователя  
  - Формат запроса: `stats&LOGIN&TOKEN`  
  - Аргументы: `LOGIN`, `TOKEN`  
  - Успех: `stats&TOTAL&CORRECT&FAILED\r\n`  
    - `TOTAL` — всего записей, `CORRECT` — количество верных, `FAILED` — количество неверных  
    - Эти поля формируются в DBManager::getStats как `"%1&%2&%3"`  
  - Ошибки: `stats_args_error\r\n` (неверное число аргументов), `token_expired\r\n` (токен не валиден/истёк), `get_stats_error\r\n` (ошибка запроса к БД)

- **set_stats:** Сохранение результата задания (логирование)  
  - Формат запроса: `set_stats&LOGIN&TOKEN&TASK_ID&IS_CORRECT`  
  - Аргументы:
    - `LOGIN` — логин
    - `TOKEN` — токен авторизации
    - `TASK_ID` — целое число (id задания)
    - `IS_CORRECT` — `1` (верно) или `0` (неверно)  
  - Успех: `stats&set_stats_success\r\n` (сервер оборачивает ответ от БД в префикс `stats&`)  
  - Ошибки: `set_stats_args_error\r\n` (неверные/недопустимые аргументы), `token_expired\r\n`, `set_stats_error\r\n` (ошибка записи/БД)

- **findNodesByDepth:** Получение задания «findNodesByDepth» (сервер выбирает случайную задачу из БД)  
  - Формат запроса: `findNodesByDepth&LOGIN&TOKEN`  
  - Аргументы: `LOGIN`, `TOKEN`  
  - Успех: `findNodesByDepth&TASK_ID&TASK_TEXT&CORRECT_ANSWER\r\n`  
    - `TASK_ID` — id задачи в БД, `TASK_TEXT` — текст задания (может содержать JSON/описание графа), `CORRECT_ANSWER` — эталонный ответ  
  - Ошибки: `findNodesByDepth_args_error\r\n`, `token_expired\r\n`, `findNodesByDepth_error\r\n` (например, нет задач/ошибка БД)

- **isPathShortest:** Задание проверки кратчайшего пути  
  - Формат запроса: `isPathShortest&LOGIN&TOKEN`  
  - Аргументы: `LOGIN`, `TOKEN`  
  - Успех: `isPathShortest&TASK_TEXT&CORRECT_ANSWER\r\n`  
    - Возвращается текст задания и правильный ответ (код в serverfunctionsmanager.cpp)  
  - Ошибки: `isPathShortest_args_error\r\n`, `token_expired\r\n`, `isPathShortest_error\r\n`

- **primesAlgorithm:** Задание по алгоритму Прима (или похожее)  
  - Формат запроса: `primesAlgorithm&LOGIN&TOKEN`  
  - Аргументы: `LOGIN`, `TOKEN`  
  - Успех: `primesAlgorithm&TASK_TEXT&CORRECT_ANSWER\r\n`  
  - Ошибки: `primesAlgorithm_args_error\r\n`, `token_expired\r\n`, `primesAlgorithm_error\r\n`

- **kruskalsAlgorithm:** Задание по алгоритму Крускала  
  - Формат запроса: `kruskalsAlgorithm&LOGIN&TOKEN`  
  - Аргументы: `LOGIN`, `TOKEN`  
  - Успех: `kruskalsAlgorithm&TASK_TEXT&CORRECT_ANSWER\r\n`  
  - Особенность: при неверном числе аргументов функция возвращает строку с небольшим опечаточным именем ошибки `kruskalssAlgorithm_args_error\r\n` (это реальная строка в коде)  
  - Ошибки: `kruskalssAlgorithm_args_error\r\n` (опечатка в коде), `token_expired\r\n`, `kruskalsAlgorithm_error\r\n`

**Общие ответы/ошибки**
- `Unknown_command\r\n` — команда не найдена (парсер в `ServerFunctionsManager::parse`)  
- Любая команда может вернуть `_args_error\r\n` при неверном числе/формате аргументов.  
- При ошибках БД код обычно возвращает `_error\r\n` или аналог.

**Примечания по реализации**
- Сервер не отправляет автоматического приветственного сообщения при подключении (в текущем коде нет записи на сокет в `slotNewConnection`) — см. mytcpserver.cpp.  
- Команды строго разделяются символом `&`; сервер делает `trimmed()` перед разбором. Пустая команда логируется, но не обрабатывается.  
- Проверка токена выполняется в `DBManager::checkAuth` (удаляет истёкшие токены и возвращает false, если токен неверен).

**Короткий пример обмена (псевдокод)**

- Авторизация:
  - Отправить: `auth&alice&secret`  
  - Получить: `auth_success&550e8400-e29b-41d4-a716-446655440000\r\n` (пример токена)

- Запрос статистики:
  - Отправить: `stats&alice&550e8400-e29b-41d4-a716-446655440000`  
  - Получить: `stats&10&7&3\r\n`  (10 всего, 7 верных, 3 неверных)

- Запрос задания:
  - Отправить: `findNodesByDepth&alice&550e8400-e29b-41d4-a716-446655440000`  
  - Получить: `findNodesByDepth&42&<task_text>&<correct_answer>\r\n`

**Рекомендации для клиента**
- Всегда сохранять и прикреплять токен к защищённым запросам.  
- Обрабатывать `token_expired\r\n` как требование повторной авторизации.  
- Разбирать ответ по `&` и отбрасывать завершающий `\r\n`.  
- Учесть, что содержимое `TASK_TEXT` и `CORRECT_ANSWER` приходит из БД текстом