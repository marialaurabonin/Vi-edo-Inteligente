#include "arduino_secrets.h"
/*****************************************
Definir librerías, instancias y constantes
para la conexión de los sensores
*****************************************/
#define DHTPIN 9// pin DATA de DHT11
#define BombaAgua 8 // pin DATA de Bomba de Agua 
#define PIN_SENSORlluvia 1 // pin DATA de sensor Lluvia
#define sensor_humedad_suelo 0 // pin DATA de sensor Humedad de suelo

#include <DHT.h> //Librería DHT
#include <DHT_U.h>
#include <WiFiNINA.h> // Librería de Wifi para módulo NINA
#include "thingProperties.h" //Añade contenido del archivo .h anexado como pestaña


DHT dht(DHTPIN, DHT11); // Crea el objeto DHT

const char THING_ID[] = "0a8bab5f-8748-4c8a-9f70-2f3ba3a32d9f"; // ID de la placa
const char *device_id = "Prototipo_IoT_Viñedo"; 

const int TemperaturaMax = 35;
const int HumedadSueloMax = 44;// Rango => 400: Húmedo 100% - 1023: Seco.
const int HumedadSueloMin = 25;// Rango => 400: Húmedo 100% - 1023: Seco.
const int NivelLluvia = 45;//450;      // Rango => 0: No hay lluvia - 1023: lluvia 100%

unsigned long tiempoAnterior = 0; // Almacena la última vez que se lanzó el evento
const unsigned long tiempoConsulta = 15 * 60 * 1000;  // Intervalo de tiempo en milisegundos (15 minutos)


void setup() {
  Serial.begin(115200); 
  delay(1000); 

  Serial.println("¡Bienvenido!");
  Serial.println(">>Configure el dispositivo para conectarlo a su red<<");
  Serial.println("\r");
  initProperties();
  
/*****************************************
  Connect to Arduino IoT Cloud
*****************************************/
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);//Indica inicio de la conexión con el servicio Arduino IoT Cloud 

  setDebugMessageLevel(2); //Establece el nivel de mensajes de depuración en 2
  ArduinoCloud.printDebugInfo(); //Imprime información de depuración específica
/****************************************/

  pinMode(BombaAgua, OUTPUT);
  digitalWrite(BombaAgua, LOW); // Inicializa la bomba de agua apagada.
  dht.begin();
  
//Pin RGB como salida en sus tres colores
  pinMode(LEDR, OUTPUT); 
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDR, HIGH); //inicializa led rojo en alto
  digitalWrite(LEDG, LOW); //inicializa led verde en bajo
  digitalWrite(LEDB, LOW); //inicializa led azul en bajo
}

void loop() {
  unsigned long tiempoActual = millis();
  ArduinoCloud.update();  


  if ((tiempoActual - tiempoAnterior) >= tiempoConsulta) {
    Lectura_Sensores();
      
    tiempoAnterior = tiempoActual;
    //led_azul = false;
    //led_rojo = true;
    //led_verde = false;
    
    if (sensorLluvia > NivelLluvia) {
      led_azul = true;
      led_rojo = false;
      led_verde = false;
      onLedAzulChange();
    } else {
      if (HumedadSuelo <= HumedadSueloMin) {
        led_verde = true;
        led_azul = false;
        led_rojo = false;
        onLedVerdeChange();
        }
        if (dht_temperatura >= TemperaturaMax) {
          Serial.println("¡Alerta de altas temperaturas!");
          Serial.println("Se recomienda postergar el riego.");
          Serial.println("\r");
        }else {
          if (HumedadSuelo >= HumedadSueloMax) {
            led_rojo = true;
            led_azul = false;
            led_verde = false;
            onLedRojoChange();
          }
        }
     }
    Serial.println("Muestra N°");
    numero_muestra++;
  }
}

void Lectura_Sensores() {
  int HumedadAnalog_Suelo = analogRead(sensor_humedad_suelo);
  HumedadSuelo = map(HumedadAnalog_Suelo, 0, 1023, 100, 0);
  dht_temperatura = dht.readTemperature();
  dht_humedad = dht.readHumidity();
  int sensorAnalog_Lluvia = analogRead(PIN_SENSORlluvia);
  sensorLluvia = map(sensorAnalog_Lluvia, 0, 1023, 0, 100);
}
/***Función para Encendido Led Azul***/
void onLedAzulChange() {
  //led_rojo = false;
  //led_verde = false;
  if (led_azul=true) {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, HIGH);
    Serial.println("Presencia de Lluvia");
    offSistemaRiego();
  } else {
    digitalWrite(LEDB, LOW);
  }
}
/***Función para Encendido Led Rojo***/
void onLedRojoChange() {
  //led_verde = false;
  //led_azul = false;
  if (led_rojo=true) {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDB, LOW);
    digitalWrite(LEDG, LOW);
    offSistemaRiego();
  } else {
    digitalWrite(LEDR, LOW);
  }
}
/***Función para Encendido Led Verde***/
void onLedVerdeChange() {
 // led_rojo = false;
  //led_azul = false;
  if (led_verde=true) {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDB, LOW);
    digitalWrite(LEDG, HIGH);
    onSistemaRiego();
  } else {
    digitalWrite(LEDG, LOW);
    offSistemaRiego();
  }
}
/***Función para Encender Bomba***/
void onSistemaRiego() {
  digitalWrite(BombaAgua, HIGH);
  Serial.println("Sistema de Riego Encendido");
  Serial.println("\r");
}
/***Función para Apagar Bomba***/
void offSistemaRiego() {
  digitalWrite(BombaAgua, LOW);
  Serial.println("Sistema de Riego Apagado");
  Serial.println("\r");
}

