#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_task_wdt.h>

//---
// Configuracoes globais
//---

// Substitua pelas suas credenciais de Wi-Fi
const char* ssid = "seu_ssid";
const char* password = "sua_senha";

// Use o endereco IP que o servidor Flask exibiu no terminal.
const char* api_endpoint = "http://192.168.0.11:5000/api/produtos";
const char* feedback_endpoint = "http://192.168.0.11:5000/api/feedback";

// Definicao dos enderecos e pinos
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define BUTTON_YES_1_PIN 13
#define BUTTON_NO_1_PIN 14
#define BUTTON_YES_2_PIN 15
#define BUTTON_NO_2_PIN 12

// Objetos para os displays.
TwoWire I2C_one = TwoWire(0);
TwoWire I2C_two = TwoWire(1);

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_one, -1);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_two, -1);

// Variaveis de tempo para a atualizacao (metodo nao-bloqueante)
unsigned long lastUpdateTime = 0;
const long updateInterval = 10000; // 10 segundos

//---
// Funcoes auxiliares
//---

// Funcao para exibir as informacoes do produto
void showProductInfo(Adafruit_SSD1306 &display, const char* name, float price) {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(name);
  
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.printf("R$ %.2f", price);
  display.display();
}

// Funcao para exibir a resposta do botao
void showButtonResponse(Adafruit_SSD1306 &display, const char* response) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 8);
  display.println(response);
  display.display();
}

// Funcao para enviar o feedback para a API
void sendFeedback(int productId, const char* feedback) {
  HTTPClient http;
  http.begin(feedback_endpoint);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<100> doc;
  doc["produto_id"] = productId;
  doc["feedback"] = feedback;

  String json_payload;
  serializeJson(doc, json_payload);

  int httpCode = http.POST(json_payload);

  if (httpCode > 0) {
    Serial.printf("[HTTP] Feedback enviado, codigo: %d\n", httpCode);
  } else {
    Serial.printf("[HTTP] Falha ao enviar feedback: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

// Funcao para obter e exibir os dados da API
void getAndDisplayData() {
  HTTPClient http;
  http.begin(api_endpoint);

  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    
    StaticJsonDocument<500> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print(F("Erro ao analisar JSON: "));
      Serial.println(error.f_str());
      return;
    }

    display1.clearDisplay();
    display2.clearDisplay();

    for (JsonObject produto : doc.as<JsonArray>()) {
      int id = produto["id"];
      int display_id = produto["display_id"];
      const char* nome = produto["nome"];
      float preco = produto["preco"];

      if (display_id == 1) {
        showProductInfo(display1, nome, preco);
      } else if (display_id == 2) {
        showProductInfo(display2, nome, preco);
      }
    }
  } else {
    Serial.printf("[HTTP] Falha na requisicao: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

//---
// Funcoes principais
//---

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado!");

  // Configura a Watchdog com a nova sintaxe
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 5000,
    .trigger_panic = true
  };
  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL);

  I2C_one.begin(21, 22);
  if (!display1.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Falha ao alocar SSD1306_1"));
    for (;;);
  }

  I2C_two.begin(25, 26);
  if (!display2.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Falha ao alocar SSD1306_2"));
    for (;;);
  }
  
  pinMode(BUTTON_YES_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_NO_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_YES_2_PIN, INPUT_PULLUP);
  pinMode(BUTTON_NO_2_PIN, INPUT_PULLUP);
}

void loop() {
  // Reseta o Watchdog para evitar que o ESP32 reinicie
  esp_task_wdt_reset();

  // Checa se ja passou o tempo para atualizar os dados (metodo nao-bloqueante)
  if (millis() - lastUpdateTime >= updateInterval) {
    getAndDisplayData();
    lastUpdateTime = millis();
  }

  // Leitura dos botoes do Display 1
  if (digitalRead(BUTTON_YES_1_PIN) == LOW) {
    sendFeedback(1, "Sim");
    showButtonResponse(display1, "Sim");
    delay(2000);
  }

  if (digitalRead(BUTTON_NO_1_PIN) == LOW) {
    sendFeedback(1, "Nao");
    showButtonResponse(display1, "Nao");
    delay(2000);
  }

  // Leitura dos botoes do Display 2
  if (digitalRead(BUTTON_YES_2_PIN) == LOW) {
    sendFeedback(2, "Sim");
    showButtonResponse(display2, "Sim");
    delay(2000);
  }

  if (digitalRead(BUTTON_NO_2_PIN) == LOW) {
    sendFeedback(2, "Nao");
    showButtonResponse(display2, "Nao");
    delay(2000);
  }
}
