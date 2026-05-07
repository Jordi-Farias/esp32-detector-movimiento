#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

// --- ¡CAMBIA ESTO POR TUS DATOS! ---
const char* ssid = "TU_NOMBRE_DE_WIFI";
const char* password = "TU_CONTRASEÑA_DE_WIFI";
// -----------------------------------

WebServer server(80);

const int pirPin = 13;
const int ledPin = 2;

// --- NUEVAS VARIABLES PARA EL FILTRO INTELIGENTE ---
bool alarmaActiva = false;           // ¿La alarma está sonando?
unsigned long ultimaDeteccion = 0;   // Guarda en qué milisegundo fue la última detección
const unsigned long tiempoGracia = 5000; // 5000 milisegundos (5 segundos) de espera antes de apagarse
// ---------------------------------------------------

String generarPaginaWeb(bool hayMovimiento) {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<meta http-equiv='refresh' content='2'><title>Alarma ESP32</title>";
  html += "<style>body { font-family: Arial; text-align: center; margin-top: 100px; background-color: #f4f4f9;}";
  html += ".estado { font-size: 30px; padding: 30px; border-radius: 15px; display: inline-block; color: white; font-weight: bold;}";
  if (hayMovimiento) html += ".alerta { background-color: #e74c3c; }"; 
  else html += ".alerta { background-color: #2ecc71; }"; 
  html += "</style></head><body><h1>Monitor de Movimiento PIR</h1>";
  if (hayMovimiento) html += "<div class='estado alerta'>¡ALERTA! Movimiento Detectado 🏃‍♂️</div>";
  else html += "<div class='estado alerta'>Todo tranquilo... 😌</div>";
  html += "</body></html>";
  return html;
}

void manejarPaginaPrincipal() {
  server.send(200, "text/html", generarPaginaWeb(alarmaActiva));
}

void setup() {
  Serial.begin(115200);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.print("Conectando a: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  Serial.println("\n¡Wi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", manejarPaginaPrincipal);
  server.begin();
  
  Serial.println("Calibrando sensor... (espera 2 segundos)");
  delay(2000);
  Serial.println("¡Sistema Activo!\n");
}

void loop() {
  server.handleClient();

  int lecturaSensor = digitalRead(pirPin);

  // 1. Si el sensor detecta movimiento en ESTE preciso momento
  if (lecturaSensor == HIGH) {
    ultimaDeteccion = millis(); // Reiniciamos el cronómetro interno
    
    // Si la alarma estaba apagada, la encendemos y avisamos (UNA SOLA VEZ)
    if (!alarmaActiva) {
      alarmaActiva = true;
      digitalWrite(ledPin, HIGH);
      Serial.println("-----------------------------------------");
      Serial.println(" 🚨 ¡ALERTA! SE HA DETECTADO MOVIMIENTO 🚨 ");
      Serial.println("-----------------------------------------\n");
    }
  }

  // 2. Si la alarma está activa, pero ha pasado mucho tiempo sin detectar nada nuevo
  if (alarmaActiva && (millis() - ultimaDeteccion > tiempoGracia)) {
    alarmaActiva = false; // Apagamos la alarma
    digitalWrite(ledPin, LOW);
    Serial.println(" ✅ El movimiento ha cesado de forma definitiva.\n");
  }

  delay(50);
}