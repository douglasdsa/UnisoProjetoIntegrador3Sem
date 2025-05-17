
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic* pCharacteristic;
bool deviceConnected = false;
unsigned long tempoIntervalo = 0;
unsigned long proximoAlvo = 0;
int ledIndex = 0;

const int ledPins[] = {1, 39, 0, 21, 4, 17, 10};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.find("TEMPO:") == 0) {
      tempoIntervalo = atol(value.substr(6).c_str()) * 1000;
      proximoAlvo = millis() + tempoIntervalo;
      ledIndex = 0;
      Serial.printf("Tempo recebido: %lu ms\n", tempoIntervalo);
    } else if (value == "CONFIRMAR") {
      digitalWrite(ledPins[ledIndex], LOW);
      ledIndex++;
      if (ledIndex < numLeds) {
        proximoAlvo = millis() + tempoIntervalo;
      } else {
        Serial.println("Todos os LEDs já foram acionados.");
      }
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Conectado");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Desconectado");
  }
};

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

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
  Serial.println("Aguardando conexão BLE...");
}

void loop() {
  if (deviceConnected && tempoIntervalo > 0 && ledIndex < numLeds) {
    if (millis() >= proximoAlvo) {
      digitalWrite(ledPins[ledIndex], HIGH);
      Serial.printf("LED %d aceso (pino %d)\n", ledIndex + 1, ledPins[ledIndex]);
      tempoIntervalo = 0; // pausa até confirmação
    }
  }
}
