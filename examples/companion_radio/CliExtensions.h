#pragma once
// =============================================================================
// CliExtensions.h — дополнительные CLI-команды для Meshcomod (Heltec V3 ESP32-S3)
//
// Добавляемые команды (в чате с контактом «Meshcomod»):
//   netscan            — сканирование локальной подсети
//   portscan <IP>      — сканирование портов конкретного IP
//   wol <MAC>          — Wake-on-LAN (MAC вида AA:BB:CC:DD:EE:FF)
//   blescan            — сканирование BLE-устройств (5 сек)
//   lscan              — пассивное сканирование LoRa-частот 863–870 МГц
//   wifi list          — список сохранённых WiFi-сетей
//   wifi connect <N>   — подключиться к сети №N
//   wifi rm <N>        — удалить сеть №N
//   wifi save          — сохранить текущую SSID как новую запись
//
// Интеграция в MyMesh.cpp (4 строки):
//   1. После #include "WiFiConfig.h":  #include "CliExtensions.h"
//   2. После pushMeshcomodReply (~строка 350-412):
//        void meshcomodReply(const char* m) { pushMeshcomodReply(m); }
//   3. В конце handleMeshcomodCommand (~строка 860):
//        if (cliExtHandle(p)) return true;
//   4. В loop(), после checkSerialInterface():  cliExtLoop();
// =============================================================================

#ifdef ESP32
#include <Arduino.h>

// Враппер вокруг pushMeshcomodReply, определяется в MyMesh.cpp
extern void meshcomodReply(const char* msg);

// Обработка команды.  p — строка команды без ведущих пробелов.
// Возвращает true если команда распознана.
bool cliExtHandle(const char* p);

// Вызывать из loop() — продвигает фоновые сканирования.
void cliExtLoop();

#endif // ESP32
