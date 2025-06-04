
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic* pCharacteristic;
bool deviceConnected = false;
bool habvoid = false;
unsigned long tempoIntervalo = 0;
unsigned long proximoAlvo = 0;
int ledIndex = 0;

const int ledPins[] = {1, 39, 0, 21, 4, 17, 10};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
  String value = String(pCharacteristic->getValue().c_str());
  value.trim();
  Serial.printf("%s\\n", value);

  if (value.startsWith("TEMPO:")) {
      digitalWrite(1, LOW);
      digitalWrite(39, LOW);
      digitalWrite(0, LOW);
      digitalWrite(21, LOW);
      digitalWrite(4, LOW);
      digitalWrite(17, LOW);
      digitalWrite(10, LOW);
    tempoIntervalo = value.substring(6).toInt() * 1000;
    proximoAlvo = millis() + tempoIntervalo;
    ledIndex = 0;
    Serial.printf("Tempo recebido: %lu ms\n", tempoIntervalo);
    habvoid = true;
  } else if (value == "CONFIRMAR") {
    digitalWrite(ledPins[ledIndex], LOW);
    ledIndex++;
    Serial.printf("ledIndex: %d\n", ledIndex);
    if (ledIndex < numLeds) {
      proximoAlvo = millis() + tempoIntervalo;
      Serial.printf("Proximo alvo: %lu ms\n", proximoAlvo);
      Serial.printf("Milis: %lu ms\n", millis());
    } else {
      Serial.println("Todos os LEDs foram acionados.");
    }
    habvoid = true;
  }
}
};

unsigned long lastDisconnectTime = 0;  // nova variável

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Dispositivo conectado.");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    lastDisconnectTime = millis();  // salva o momento da desconexão
    Serial.println("Dispositivo desconectado.");
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(1, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(10, OUTPUT);
  
  digitalWrite(1, LOW);
  digitalWrite(39, LOW);
  digitalWrite(0, LOW);
  digitalWrite(21, LOW);
  digitalWrite(4, LOW);
  digitalWrite(17, LOW);
  digitalWrite(10, LOW);

  BLEDevice::init("ESP32_MedAlert");
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
  Serial.println("Anunciando BLE...");
}

void loop() {
  if (deviceConnected && tempoIntervalo > 0 && ledIndex < numLeds && habvoid == true) {
    if (millis() >= proximoAlvo) {
      digitalWrite(ledPins[ledIndex], HIGH);
      Serial.printf("LED %d aceso (pino %d)\n", ledIndex + 1, ledPins[ledIndex]);
      habvoid = false; // aguarda confirmação
    }
  }

   // Verifica se está desconectado há mais de 10 segundos
  if (!deviceConnected && (millis() - lastDisconnectTime >= 10000)) {
    BLEDevice::startAdvertising();
    Serial.println("Reiniciando anúncio BLE após 10s de desconexão...");
    lastDisconnectTime = millis(); // reinicia o contador para evitar múltiplas chamadas
  }
}
