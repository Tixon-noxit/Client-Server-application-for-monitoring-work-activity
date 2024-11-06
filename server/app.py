from flask import Flask, render_template, jsonify, request
import sqlite3
import os

app = Flask(__name__)

# БД для хранения информации о клиентах
def init_db():
    if not os.path.exists("clients.db"):
        conn = sqlite3.connect("clients.db")
        c = conn.cursor()
        c.execute('''CREATE TABLE clients (
                        id INTEGER PRIMARY KEY AUTOINCREMENT,
                        ip TEXT,
                        username TEXT,
                        last_active TIMESTAMP
                    )''')
        conn.commit()
        conn.close()

# Функция для получения всех клиентов из базы данных
def get_all_clients():
    conn = sqlite3.connect("clients.db")
    c = conn.cursor()
    c.execute("SELECT * FROM clients")
    clients = c.fetchall()
    conn.close()
    return clients

# Функция для добавления нового клиента
def add_client(ip, username):
    conn = sqlite3.connect("clients.db")
    c = conn.cursor()
    c.execute("INSERT INTO clients (ip, username, last_active) VALUES (?, ?, datetime('now'))", (ip, username))
    conn.commit()
    conn.close()

# Функция для обновления времени последней активности клиента
def update_last_active(ip):
    conn = sqlite3.connect("clients.db")
    c = conn.cursor()
    c.execute("UPDATE clients SET last_active = datetime('now') WHERE ip = ?", (ip,))
    conn.commit()
    conn.close()

# Главная страница с интерфейсом
@app.route('/')
def index():
    clients = get_all_clients()
    return render_template('index.html', clients=clients)

# API для получения списка клиентов
@app.route('/api/clients', methods=['GET'])
def api_clients():
    clients = get_all_clients()
    return jsonify(clients)

# API для добавления нового клиента
@app.route('/api/add_client', methods=['POST'])
def api_add_client():
    ip = request.json['ip']
    username = request.json['username']
    add_client(ip, username)
    return jsonify({"status": "success"}), 201

# API для обновления последней активности клиента
@app.route('/api/update_activity', methods=['POST'])
def api_update_activity():
    ip = request.json['ip']
    update_last_active(ip)
    return jsonify({"status": "success"}), 200

if __name__ == "__main__":
    init_db()  # Инициализация БД при запуске
    app.run(host="0.0.0.0", port=5001, debug=True)
