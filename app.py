import os
import psycopg2
from flask import Flask, jsonify, request, render_template
from flask_cors import CORS
from dotenv import load_dotenv

# Carrega as variáveis de ambiente do arquivo .env
load_dotenv()

app = Flask(__name__)
# Permite que a API seja acessada de qualquer origem (necessario para o ESP32)
CORS(app)

# Funcao para conectar ao banco de dados PostgreSQL
def get_db_connection():
    conn = psycopg2.connect(
        host="localhost",
        database="projeto_gondola",
        user=os.environ['DB_USER'],
        password=os.environ['DB_PASSWORD']
    )
    return conn

# Rota principal para servir a interface web
@app.route('/')
def index():
    return render_template('index.html')

# Rota para o ESP32 e o sistema web obterem as informacoes dos produtos
@app.route('/api/produtos')
def get_produtos():
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('SELECT * FROM produtos ORDER BY display_id;')
    produtos = cur.fetchall()
    cur.close()
    conn.close()

    produtos_json = []
    for produto in produtos:
        produtos_json.append({
            'id': produto[0],
            'nome': produto[1],
            'preco': str(produto[2]),
            'display_id': produto[3]
        })

    return jsonify(produtos_json)

# Rota para o sistema web obter os dados de feedback
@app.route('/api/feedback')
def get_feedback():
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('''
        SELECT 
            p.nome, 
            fc.feedback, 
            fc.momento 
        FROM 
            feedback_cliente fc
        JOIN 
            produtos p ON fc.produto_id = p.id
        ORDER BY fc.momento DESC;
    ''')
    feedback_data = cur.fetchall()
    cur.close()
    conn.close()

    feedback_json = []
    for data in feedback_data:
        feedback_json.append({
            'produto': data[0],
            'feedback': data[1],
            'momento': data[2].strftime("%Y-%m-%d %H:%M:%S")
        })

    return jsonify(feedback_json)

# Rota para o sistema web adicionar um novo produto
@app.route('/api/produtos', methods=['POST'])
def add_produto():
    data = request.get_json()
    nome = data.get('nome')
    preco = data.get('preco')
    display_id = data.get('display_id')

    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('INSERT INTO produtos (nome, preco, display_id) VALUES (%s, %s, %s) RETURNING id;', (nome, preco, display_id))
    new_id = cur.fetchone()[0]
    conn.commit()
    cur.close()
    conn.close()
    
    return jsonify({'id': new_id, 'status': 'Produto adicionado com sucesso!'}), 201

# Rota para o sistema web atualizar um produto existente
@app.route('/api/produtos/<int:produto_id>', methods=['PUT'])
def update_produto(produto_id):
    data = request.get_json()
    conn = get_db_connection()
    cur = conn.cursor()

    if 'nome' in data:
        cur.execute('UPDATE produtos SET nome = %s WHERE id = %s;', (data['nome'], produto_id))
    if 'preco' in data:
        cur.execute('UPDATE produtos SET preco = %s WHERE id = %s;', (data['preco'], produto_id))
    if 'display_id' in data:
        cur.execute('UPDATE produtos SET display_id = %s WHERE id = %s;', (data['display_id'], produto_id))

    conn.commit()
    cur.close()
    conn.close()

    return jsonify({'id': produto_id, 'status': 'Produto atualizado com sucesso!'})

# Rota para o sistema web deletar um produto
@app.route('/api/produtos/<int:produto_id>', methods=['DELETE'])
def delete_produto(produto_id):
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('DELETE FROM produtos WHERE id = %s;', (produto_id,))
    conn.commit()
    cur.close()
    conn.close()

    return jsonify({'id': produto_id, 'status': 'Produto deletado com sucesso!'})


# Rota para o ESP32 enviar o feedback dos botoes
@app.route('/api/feedback', methods=['POST'])
def post_feedback():
    data = request.get_json()
    produto_id = data.get('produto_id')
    feedback = data.get('feedback')

    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('INSERT INTO feedback_cliente (produto_id, feedback) VALUES (%s, %s);', (produto_id, feedback))
    conn.commit()
    cur.close()
    conn.close()

    return jsonify({'status': 'success'}), 201

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
