// Añadimos la cabecera de Arduino para compilar con toolchains estándar
#include <Arduino.h>

// Definimos el pin donde está conectado el cable de datos (verde) del sensor PIR
const int pinPIR = 34; 

// Variables para manejar el estado del sensor
int estadoPIR = LOW; // Asumimos que no hay movimiento al encender
int lectura = 0;     // Variable para guardar lo que lee el sensor

void setup() {
  // Iniciamos la comunicación serial a 115200 baudios (típico en ESP32)
  Serial.begin(115200);
  
  // Configuramos el pin 34 como entrada (INPUT)
  pinMode(pinPIR, INPUT);
  
  Serial.println("Iniciando sensor... esperando a que se estabilice.");
  delay(2000); // El sensor necesita un par de segundos para "acostumbrarse" al ambiente
  Serial.println("¡Sensor listo!");
}

void loop() {
  // Leemos si el sensor está enviando un 1 (HIGH) o un 0 (LOW)
  lectura = digitalRead(pinPIR);

  // Si la lectura es HIGH, significa que detectó algo
  if (lectura == HIGH) {
    if (estadoPIR == LOW) {
      // Solo imprimimos si es un evento "nuevo" para no saturar la pantalla
      Serial.println("¡Movimiento detectado! 👀");
      estadoPIR = HIGH; // Actualizamos el estado
    }
  } 
  // Si la lectura es LOW, no hay movimiento
  else {
    if (estadoPIR == HIGH) {
      Serial.println("Todo tranquilo, sin movimiento. 🛑");
      estadoPIR = LOW; // Actualizamos el estado
    }
  }
  
  delay(100); // Pequeña pausa para estabilizar las lecturas
}