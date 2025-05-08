#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// UUIDs para o serviço e a característica
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    Serial.println("Dispositivo conectado!");
  }

  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println("Dispositivo desconectado.");
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando anúncio BLE...");

  // Inicializa BLE
  BLEDevice::init("ESP32S3_Hello");

  // Cria servidor e define callback
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Cria serviço
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Cria característica
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // Adiciona descritor obrigatório para notificações funcionarem
  pCharacteristic->addDescriptor(new BLE2902());

  // Define valor inicial
  pCharacteristic->setValue("Hello World");

  // Inicia serviço
  pService->start();

  // Começa a anunciar
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);  // Compatível com Android
  pAdvertising->setMinPreferred(0x12);  // Compatível com iOS
  BLEDevice::startAdvertising();

  Serial.println("Anunciando BLE como 'ESP32S3_Hello'");
}

void loop() {
  // Envia notificações periodicamente após conexão
  if (deviceConnected) {
    pCharacteristic->setValue("Hello World");
    pCharacteristic->notify();
    delay(2000);
  }
}