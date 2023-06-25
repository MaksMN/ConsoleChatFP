# Консольный чат
## 20.11. Домашнее задание

#### ЗАДАЧА

> Сделайте ваш чат сетевым, то есть доработайте его так, чтобы два экземпляра программы могли отправлять и принимать сообщения между собой.
> - Используйте протокол UDP или TCP на свое усмотрение.
> - Можно использовать архитектуру клиент-сервер, в этом случае будут две программы — клиент и сервер.
Можно использовать децентрализованную (англ. peer-to-peer, P2P — равный к равному) сеть, в которой каждый узел является как клиентом, так и выполняет функции сервера.
#### Задание повышенной сложности
> Сделайте ваш чат кроссплатформенным, т.е. чтобы он компилировался и работал в Linux и в Windows.

**Реализовано**
> 1. Основная задача. Сетевой чат. Протокол UDP. Архитектура клиент-сервер.
> 2. Задание повышенной сложности. Чат кроссплатформенный. Полная межплатформенная совместимость. Клиенты в Linux могут общаться с клиентами Windows и наоборот. Сервер тоже можно запускать в любой ОС.

### Документация пользователя
**1. Запуск клиента.**
   
Программу - клиент можно запустить с 1 или 2 параметрами
   - один параметр - порт сервера, адрес автоматически 127.0.0.1
   - два параметра - адрес порт
   - без параметров по умолчанию 127.0.0.1:7777

**2. Запуск сервера.**
Программу - сервер можно запустить с одним аргументом. Он указывает порт который слушает сервер. Если запустить без аргументов, по умолчанию будет использоваться порт 7777.

**3. Пользовательский интерфейс.**
Чат имеет несколько страниц: общий чат, список пользователей, личные сообщения.

Функционал реализован при помощи слэш-команд */command* или */command:value1:value2:value..n*
В процессе работы программа будет запрашивать определенные команды, которые вам надо будет вводить.

Введите команду /hello чтобы проверить соединение с сервером.

Есть ряд команд, которые работают всегда даже когда вас просят ввести что-то другое:
 - Команды, которые можно вызвать в любое время:
   - Команда: /chat - перейти на главную страницу из любого раздела;
   - Команда: /hello - опрос сервера;
   - Команда: /help - справка;
   - Команда: /quit - закрыть клиент;
 - Команды, которые можно вызвать в любое время авторизованным пользователям:
   - Команда: /profile - сменить имя или пароль;
   - Команда: /logout - выйти из чата;
   - Команда: /sv_quit - (admin)завершить работу сервера;

В чате есть сервисный администратор: логин - admin, пароль - 1234. Авторизуйтесь с этими данными в клиенте и смените пароль командой /profile

Сервисный администратор может может назначать других администраторов.

Все администраторы могут скрывать сообщения общего чата и блокировать пользователей.

На страницах чата у администраторов есть дополнительные команды управления сообщениями и пользователями.

В папке PRESETS есть файлы базы данных с предустановленными пользователями и сообщениями публичного чата. Скопируйте их в папку сервера. Все предустановленные пользователи имеют пароль pass. Их список можно получить командой /users в общем чате.

**4. Сборка приложений.**

Этот проект использует методы и функции стандарта c++20 и должно собираться с флагом *-std=c++20*

Для сборки сервера надо собрать файлы проекта в папках: *Server, Misc, DB*

Для сборки клиента надо собрать файлы проекта в папках: *Client, Misc*

Тк же имеются готовые файлы сценариев сборки проектов:
 - Для Linux - *Makefile*
 - Для Windows - *MakefileWin.cmd*

#
### Описание реализации

Приложение использует клиент-серверную архитектуру. Принцип работы сходный с работой вебсайтов. 
Клиент получает команду от пользователя, отправляет её на сервер. Сервер обрабатывает полученную команду и возвращает ответ клиенту. Клиент печатает в консоли полученные данные. После чего связь разрывается. По сути клиент играет роль веб-браузера, а сервер - веб-сервера.

Для идентификации клиентов используется уникальный сессионный ключ, который генерируется на клиенте и отправляется на сервер во время авторизации. При успешной авторизации сервер сохраняет ключ в БД в профиле пользователя. При каждом последующем обращении клиента, сервер проверяет сессионный ключ.

Такая система позволяет не поддерживать постоянное соединение сервера с клиентом. Несколько клиентов одновременно могут обращаться к серверу.

Система не многопоточная, поэтому клиенты при одновременном обращении выстраиваются в очередь. Сервер мгновенно отвечает на все запросы задержек практически нет.

Приложения работают устойчиво. В случае перебоев на сервере или в сети клиенты не завершают работу аварийно, а продолжают ожидать восстановления соединения с сервером.
При перезапуске сервера он сбрасывает сессионные ключи и клиентам придется заново авторизоваться.

**Структура данных передаваемых по сети**

Клиент с сервером передают друг другу два массива состоящих из бинарно сериализованных данных. В первом буфере передаются служебные заголовки и команды, во втором буфере передаются данные для отображения пользователю.

 - Командный буфер:
```html
|флаги(1)|data packets(1)|session_key (8)|pg_mode(1)|pg_start(4)|pg_perPage(4)|pg_end(4)| pm_user_id (8) |login_size(4)|login|page_size(4)|PAGE_TEXT|cmd_size(4)|CMD_TEXT|
0        1               2               10         11          15            19        23               31                                                              |
|________________________________________________________________________________________________________|                                                               |
|                               static sizes                                                             |                         dynamic sizes                         |
```
В начале командного буфера идут данные которые можно хранить в виде чисел. Числа это данные постоянного размера. К ним можно обращаться по их позиции в массиве.

Например в первом байте буфера хранятся флаги которые сообщают клиенту, что пользователю надо ввести строку или число и что это буфер команд, а не что-то другое. Второй байт сообщает клиенту, какое количество текстовых данных он должен принять после получения командного буфера.
Далее идут сессионный ключ, параметры пагинации итп.

В зоне данных переменного размера хранятся логин, названия страниц чата, команды введенные пользователем. Каждая подсекция делится на |размер|данные|. Зная начальный адрес зоны динамических данных, мы можем осуществлять быстрый поиск переходя на другие подсекции ориентируясь на размер данных.

Буфер данных имеет более простую структуру. В первом байте лежит флаг, сообщающий клиенту, что это буфер данных, а не что-то другое. Следующие 4 байта это 32битное число с размером текстовых данных в буфере.

Таким образом клиент отправляет серверу командный буфер, сервер эти команды отрабатывает и возвращает клиенту этот же командный буфер, а за ним буферы текстовых данных, которые надо напечатать в консоли клиента.

 - **Misc\BufferActions.h** - класс, обеспечивающий методы работы с буфером обмена данных.

### Описание файлов

Client/WinClient Client/LinuxClient - сокеты клиента

Client/Handler - класс обработки входящих данных и печать контента чата


Server/WinServer Server/LinuxServer сокеты сервера.

Server\Handler\ServerHandler - класс обработки входящих данных и отправка данных клиенту.

Набор классов обеспечивающих пользовательский интерфейс:

> Server\Handler\IChatInterface.h
> 
> Server\Handler\ChatPublicPage.h
> 
> Server\Handler\ChatPrivatePageUsers.h
> 
> Server\Handler\ChatPrivatePageMessages.h
> 
> Server\Handler\ChatGuestPage.h
> 
> Server\Handler\ChatEditProfile.h

DB - Классы, обеспечивающие работу с базой данных

Misc - Классы и функции необходимые для работы и в клиенте и в сервере.