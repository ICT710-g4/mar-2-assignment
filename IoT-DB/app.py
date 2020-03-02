from flask import Flask, request, jsonify
import sqlite3

app = Flask(__name__)
app.config["TEMPLATES_AUTO_RELOAD"] = True

@app.route('/input', methods=["POST"])
def register():
    payload = request.get_json(force=True)
    temp = payload['temp']
    humid = payload['humid']
    conn = sqlite3.connect('dataset.db')
    c = conn.cursor()
    c.execute('INSERT INTO records(temp,humid) VALUES(?,?)', (temp,humid,))
    conn.commit()
    conn.close()
    resp = {'status':'OK'}
    return jsonify(resp)

@app.route('/summary')
def summary():
    name = request.args.get('time')
    conn = sqlite3.connect('dataset.db')
    c = conn.cursor()
    c.execute('SELECT * FROM records')
    records = c.fetchall()
    results = []
    for r in records:
        results.append({'timestamp':r[1], 'temp':r[2], 'humid':r[3]})
    conn.commit()
    conn.close()
    resp = {'status':'OK', 'results':results}
    return jsonify(resp)

if __name__ == '__main__':
    conn = sqlite3.connect('dataset.db')
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS records
             (_id INTEGER PRIMARY KEY AUTOINCREMENT,
             timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
             temp REAL,
             humid REAL)''')
    conn.commit()
    conn.close()
    app.run(debug=True, port=8000)
