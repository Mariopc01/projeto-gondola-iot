# projeto-gondola-iot
Projeto que visa a exibição dos preços online com a interação dos clientes para coleta de dados referente a pesquisa de preços


# Projeto Gôndola Inteligente

## Visão Geral do Projeto

O "Projeto Gôndola Inteligente" é uma solução completa de Internet das Coisas (IoT) que integra hardware (ESP32 com displays OLED e botões), uma API em Python com o framework Flask e um banco de dados PostgreSQL para gerenciamento e coleta de dados em tempo real.

O objetivo principal do projeto é modernizar a gestão de produtos em gôndolas de varejo. Ele permite a atualização dinâmica de informações (como nome e preço de produtos) e a coleta de feedback do cliente por meio de botões físicos, criando uma poderosa ferramenta de análise de dados para o varejista.

## Arquitetura do Sistema

A arquitetura do projeto é dividida em três camadas principais:

1.  **Hardware (ESP32):**
    * Um microcontrolador **ESP32** com conectividade Wi-Fi.
    * Displays **OLED de 0.91 polegadas (128x32)** e **OLED de 0.96 polegadas (128x64)** conectados via interface I2C.
    * Botões físicos para coletar feedback do cliente (`Sim` e `Não`).
    * O ESP32 se conecta à API em Flask para buscar as informações dos produtos e para enviar os dados de feedback coletados.

2.  **Backend (Flask API com PostgreSQL):**
    * Uma API desenvolvida em **Python** utilizando o framework **Flask**.
    * Gerencia as informações dos produtos e os dados de feedback.
    * Se comunica com um banco de dados **PostgreSQL** para persistência dos dados.
    * As rotas da API permitem a criação, leitura, atualização e exclusão de produtos (CRUD), além da coleta e exibição do feedback dos clientes.

3.  **Frontend (Interface Web):**
    * Uma interface web simples e funcional construída com **HTML, CSS e JavaScript**.
    * Conecta-se à API para exibir a lista de produtos, permitindo que o usuário gerencie as informações de forma intuitiva.
    * Exibe o histórico de feedback dos clientes em tempo real, permitindo a análise dos dados coletados pelos displays.

## Funcionalidades Principais

* **Atualização Dinâmica de Preços:** O usuário pode alterar o nome e o preço de um produto via interface web, e a informação é atualizada instantaneamente nos displays OLED.
* **Coleta de Feedback:** Os botões "Sim" e "Não" permitem que os clientes interajam com o produto, e o feedback é enviado para a API e salvo no banco de dados.
* **Visualização de Dados:** A interface web apresenta uma tabela com o histórico de feedback, permitindo a análise de quais produtos estão recebendo mais engajamento.
* **Gerenciamento Completo:** A interface web permite adicionar, atualizar e excluir produtos.

## Problemas Encontrados e Soluções

Durante o desenvolvimento, alguns desafios foram superados, que servem como aprendizado:

* **Módulo `dotenv` não encontrado:** A primeira tentativa de rodar a API falhou porque a biblioteca `python-dotenv` não estava instalada no ambiente virtual. O problema foi resolvido com o comando `pip install python-dotenv`.

* **Erro de `KeyError`:** O servidor Flask retornava um erro de `KeyError` ao tentar se conectar ao banco de dados. A análise do erro mostrou que o nome da variável de ambiente no arquivo `.env` não correspondia ao nome usado no código (`DB_USER` em vez de `postgres`). A solução foi corrigir a variável para `DB_USER` no código ou no arquivo `.env`, garantindo que ambos tivessem o mesmo nome.

* **Exibição de JSON no Navegador:** Inicialmente, a URL da API exibia apenas o código JSON bruto. A solução foi aprimorar a arquitetura para que a rota principal (`/`) servisse o arquivo `index.html`, enquanto a rota da API (`/api/produtos`) continuava a fornecer o JSON. Isso permitiu que o projeto funcionasse como uma aplicação web completa, e não apenas uma API.

* **Conexão do ESP32 com a API:** Foi necessário configurar o endereço IP da rede local no código do ESP32 (`const char* api_endpoint = "http://192.168.0.11:5000/api/produtos";`), já que a comunicação deve ser feita para o servidor local rodando no computador.

## Tempo de execução do projeto:

Dia 1: Conectar o hardware e programar o ESP32.

Dia 2: Planejar a arquitetura de software, incluindo o banco de dados, instalar o PostgreSQL, criar o banco de dados e as tabelas.

Dia 3: Debug e fezer a API em Flask se conectar com o banco de dados, conectar o ESP32 à API, fezer os botões funcionarem e criar a interface web completa.



## Custo do projeto

Custo total do projeto: R$ 200,00

Para testes foram comprados 5 displays 128x32 e 5 display 128x64



## Como Instalar e Rodar o Projeto

1.  **Configurar o Banco de Dados:**
    * Tenha o **PostgreSQL** instalado e crie um banco de dados chamado `projeto_gondola`.
    * Crie as tabelas `produtos` e `feedback_cliente` usando os scripts SQL fornecidos no projeto.

2.  **Configurar o Backend (API):**
    * Crie um ambiente virtual e instale as dependências: `pip install flask psycopg2-binary python-dotenv flask-cors`.
    * Crie um arquivo `.env` na pasta do projeto com suas credenciais do banco de dados: `DB_USER="seu_usuario"` e `DB_PASSWORD="sua_senha"`.
    * Rode o servidor Flask: `python app.py`.

3.  **Configurar o Hardware (ESP32):**
    * Abra o projeto no seu ambiente de desenvolvimento Arduino IDE.
    * Atualize as credenciais da sua rede Wi-Fi e o endereço IP da sua máquina no código do ESP32.
    * Faça o upload do código para o ESP32.

4.  **Acessar a Interface Web:**
    * Com o servidor Flask rodando, abra o seu navegador e acesse: `http://127.0.0.1:5000/`.

---

**Autor:** Mário de Paula Costa

**Data:** 03 de Setembro de 2025
