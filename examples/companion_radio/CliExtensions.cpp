// =============================================================================
// CliExtensions.cpp
// Дополнительные команды CLI для Meshcomod (Heltec V3 / ESP32-S3 + SX1262)
// Команды: netscan, portscan <IP>, wol <MAC>, blescan, lscan,
//          wifi list, wifi connect <N>, wifi rm <N>, wifi save
// =============================================================================

#ifdef ESP32

#include "CliExtensions.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <Preferences.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Radio access для lscan (определена в variants/heltec_v3/target.h)
#if defined(USE_SX1262) && defined(HELTEC_LORA_V3)
#include "target.h"
#define LSCAN_AVAILABLE 1
#endif

// ======================================================================
// WiFi multi-network manager (NVS namespace "wifimgr", до 10 сетей)
// ======================================================================

static const char* kNvsNs   = "wifimgr";
static const int   kMaxNets = 10;

static int wifiMgrCount() {
  Preferences p; p.begin(kNvsNs, true);
  int n = p.getInt("count", 0); p.end(); return n;
}

static bool wifiMgrGet(int idx, char* ssid, size_t ssid_sz,
                       char* pwd, size_t pwd_sz) {
  if (idx < 0 || idx >= kMaxNets) return false;
  Preferences p; p.begin(kNvsNs, true);
  char ks[12], kp[12];
  snprintf(ks, sizeof(ks), "ssid%d", idx);
  snprintf(kp, sizeof(kp), "pwd%d",  idx);
  size_t sg = p.getString(ks, ssid, ssid_sz);
  p.getString(kp, pwd, pwd_sz);
  p.end(); return sg > 0;
}

static bool wifiMgrAdd(const char* ssid, const char* pwd) {
  if (!ssid || !ssid[0]) return false;
  Preferences p; p.begin(kNvsNs, false);
  int n = p.getInt("count", 0);
  if (n >= kMaxNets) { p.end(); return false; }
  char ks[12], kp[12];
  snprintf(ks, sizeof(ks), "ssid%d", n);
  snprintf(kp, sizeof(kp), "pwd%d",  n);
  p.putString(ks, ssid);
  p.putString(kp, pwd ? pwd : "");
  p.putInt("count", n + 1);
  p.end(); return true;
}

static bool wifiMgrRemove(int idx) {
  Preferences p; p.begin(kNvsNs, false);
  int n = p.getInt("count", 0);
  if (idx < 0 || idx >= n) { p.end(); return false; }
  for (int i = idx; i < n - 1; i++) {
    char ks1[12], kp1[12], ks2[12], kp2[12];
    snprintf(ks1, sizeof(ks1), "ssid%d", i);
    snprintf(kp1, sizeof(kp1), "pwd%d",  i);
    snprintf(ks2, sizeof(ks2), "ssid%d", i + 1);
    snprintf(kp2, sizeof(kp2), "pwd%d",  i + 1);
    char ss[65] = {}, sp[65] = {};
    p.getString(ks2, ss, sizeof(ss));
    p.getString(kp2, sp, sizeof(sp));
    p.putString(ks1, ss); p.putString(kp1, sp);
  }
  char ks[12], kp[12];
  snprintf(ks, sizeof(ks), "ssid%d", n - 1);
  snprintf(kp, sizeof(kp), "pwd%d",  n - 1);
  p.remove(ks); p.remove(kp);
  p.putInt("count", n - 1);
  p.end(); return true;
}

// ======================================================================
// Сетевой сканер
// ======================================================================

static bool     s_netscan_active = false;
static uint32_t s_netscan_base   = 0;
static uint32_t s_netscan_gw     = 0;
static int      s_netscan_idx    = 0;
static int      s_netscan_total  = 254;

static bool tcpProbe(IPAddress ip, uint16_t port, uint32_t ms = 80) {
  WiFiClient c; c.setTimeout(ms);
  bool ok = c.connect(ip, port, (int)ms);
  if (ok) c.stop();
  return ok;
}

static void netScanOneHost(int host_num) {
  uint32_t raw = s_netscan_base | (uint32_t)host_num;
  IPAddress ip((raw>>24)&0xFF,(raw>>16)&0xFF,(raw>>8)&0xFF,raw&0xFF);

  bool is_gw  = (raw == s_netscan_gw);
  bool has_web = tcpProbe(ip, 80, 70) || tcpProbe(ip, 443, 70);
  bool alive   = has_web || is_gw;

  if (!alive) {
    alive = tcpProbe(ip, 22, 60) || tcpProbe(ip, 8080, 60) ||
            tcpProbe(ip, 1883, 50) || tcpProbe(ip, 5000, 50);
  }
  if (!alive) return;

  char flags[16] = "[";
  if (is_gw)   strcat(flags, "G");
  if (has_web) strcat(flags, "W");
  strcat(flags, "P]");

  char line[80];
  snprintf(line, sizeof(line), "%d.%d.%d.%d  %s",
           (int)(raw>>24),(int)((raw>>16)&0xFF),
           (int)((raw>>8)&0xFF),(int)(raw&0xFF), flags);
  meshcomodReply(line);
}

// ======================================================================
// Сканер портов
// ======================================================================

static bool      s_portscan_active = false;
static IPAddress s_portscan_ip;
static int       s_portscan_idx = 0;

static const uint16_t kPorts[] = {
  21,22,23,25,53,80,443,554,8080,8443,8291,9100,1883,5000,5001,0
};
static const char* kPortNames[] = {
  "FTP","SSH","Telnet","SMTP","DNS","HTTP","HTTPS","RTSP/Camera",
  "HTTP-alt","HTTPS-alt","MikroTik Winbox","Printer JetDirect",
  "MQTT","TCP companion","TCP API-alt",nullptr
};

// ======================================================================
// Wake-on-LAN
// ======================================================================

static bool parseMac(const char* s, uint8_t mac[6]) {
  int n = sscanf(s,"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                 &mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
  if (n != 6) n = sscanf(s,"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                         &mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
  return n == 6;
}

static void sendWol(const uint8_t mac[6]) {
  uint8_t pkt[102];
  memset(pkt, 0xFF, 6);
  for (int i = 0; i < 16; i++) memcpy(pkt + 6 + i*6, mac, 6);
  WiFiUDP udp; udp.begin(0);
  IPAddress bc(255,255,255,255);
  udp.beginPacket(bc, 9);
  udp.write(pkt, sizeof(pkt));
  udp.endPacket(); udp.stop();
}

// ======================================================================
// BLE scan
// ======================================================================

static BLEScan*  s_bleScan      = nullptr;
static bool      s_bleStarted   = false;
static uint32_t  s_bleStartMs   = 0;
static const uint32_t kBleDur   = 5500;

class CliBleCallback : public BLEAdvertisedDeviceCallbacks {
public:
  void onResult(BLEAdvertisedDevice d) override {
    char line[128];
    snprintf(line, sizeof(line), "BLE %s  RSSI=%d  %s",
             d.getAddress().toString().c_str(),
             d.getRSSI(),
             d.getName().length() ? d.getName().c_str() : "(noname)");
    meshcomodReply(line);
  }
};
static CliBleCallback s_bleCb;

// ======================================================================
// LoRa scan
// ======================================================================

static bool  s_lscan_active = false;
static int   s_lscan_idx    = 0;
static const float kLscanStart  = 863.0f;
static const float kLscanStop   = 870.0f;
static const float kLscanStep   = 0.2f;
static const int   kLscanTotal  = 36;
static const float kLscanNoise  = -120.0f;

#ifndef LORA_FREQ
#define LORA_FREQ 869.618f
#endif
#ifndef LORA_BW
#define LORA_BW 62.5f
#endif
#ifndef LORA_SF
#define LORA_SF 8
#endif

// ======================================================================
// cliExtHandle — парсер команд
// ======================================================================

bool cliExtHandle(const char* p) {
  if (!p || !p[0]) return false;

  // ── wifi save ──────────────────────────────────────────────────────
  if (strncasecmp(p,"wifi save",9)==0 && (p[9]=='\0'||p[9]==' '||p[9]=='\t')) {
#if defined(WIFI_SSID)||defined(MULTI_TRANSPORT_COMPANION)
    if (WiFi.status()!=WL_CONNECTED){meshcomodReply("ERR: not connected");return true;}
    String ssid=WiFi.SSID();
    if (wifiMgrAdd(ssid.c_str(),nullptr)){
      char b[80]; snprintf(b,sizeof(b),"saved: %s (#%d)",ssid.c_str(),wifiMgrCount()-1);
      meshcomodReply(b);
    } else meshcomodReply("ERR: list full or already saved");
#else
    meshcomodReply("ERR: WiFi not in this build");
#endif
    return true;
  }

  // ── wifi list ──────────────────────────────────────────────────────
  if (strncasecmp(p,"wifi list",9)==0 && (p[9]=='\0'||p[9]==' '||p[9]=='\t')) {
    int n=wifiMgrCount();
    if (n==0){meshcomodReply("no saved networks");return true;}
    for(int i=0;i<n;i++){
      char ssid[65]={},pwd[65]={};
      wifiMgrGet(i,ssid,sizeof(ssid),pwd,sizeof(pwd));
      char b[80]; snprintf(b,sizeof(b),"%d: %s",i+1,ssid);
      meshcomodReply(b);
    }
    return true;
  }

  // ── wifi connect <N> ───────────────────────────────────────────────
  if (strncasecmp(p,"wifi connect ",13)==0) {
#if defined(WIFI_SSID)||defined(MULTI_TRANSPORT_COMPANION)
    int n=atoi(p+13);
    char ssid[65]={},pwd[65]={};
    if(n<1||!wifiMgrGet(n-1,ssid,sizeof(ssid),pwd,sizeof(pwd))){
      meshcomodReply("ERR: invalid index"); return true;}
    WiFi.begin(ssid,pwd[0]?pwd:nullptr);
    char b[80]; snprintf(b,sizeof(b),"connecting to: %s...",ssid);
    meshcomodReply(b);
#else
    meshcomodReply("ERR: WiFi not in this build");
#endif
    return true;
  }

  // ── wifi rm <N> ────────────────────────────────────────────────────
  if (strncasecmp(p,"wifi rm ",8)==0) {
    int n=atoi(p+8);
    if(n<1||!wifiMgrRemove(n-1)) meshcomodReply("ERR: invalid index");
    else { char b[40]; snprintf(b,sizeof(b),"removed #%d",n); meshcomodReply(b); }
    return true;
  }

  // ── wol <MAC> ──────────────────────────────────────────────────────
  if (strncasecmp(p,"wol ",4)==0) {
#if defined(WIFI_SSID)||defined(MULTI_TRANSPORT_COMPANION)
    uint8_t mac[6];
    if(!parseMac(p+4,mac)){meshcomodReply("ERR: bad MAC (AA:BB:CC:DD:EE:FF)");return true;}
    if(WiFi.status()!=WL_CONNECTED){meshcomodReply("ERR: WiFi not connected");return true;}
    sendWol(mac);
    char b[56]; snprintf(b,sizeof(b),"WoL => %02X:%02X:%02X:%02X:%02X:%02X",
                          mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    meshcomodReply(b);
#else
    meshcomodReply("ERR: WiFi not in this build");
#endif
    return true;
  }

  // ── netscan ────────────────────────────────────────────────────────
  if (strncasecmp(p,"netscan",7)==0 && (p[7]=='\0'||p[7]==' '||p[7]=='\t')) {
#if defined(WIFI_SSID)||defined(MULTI_TRANSPORT_COMPANION)
    if(WiFi.status()!=WL_CONNECTED){meshcomodReply("ERR: WiFi not connected");return true;}
    if(s_netscan_active||s_portscan_active){meshcomodReply("ERR: scan running");return true;}
    uint32_t lip=(uint32_t)WiFi.localIP(), mask=(uint32_t)WiFi.subnetMask();
    s_netscan_base=lip&mask; s_netscan_gw=(uint32_t)WiFi.gatewayIP();
    s_netscan_idx=1;
    uint32_t inv=~mask; s_netscan_total=(int)inv-1;
    if(s_netscan_total<1||s_netscan_total>254) s_netscan_total=254;
    s_netscan_active=true;
    char b[64]; snprintf(b,sizeof(b),"scanning %d.%d.%d.0/24...",
      (int)(s_netscan_base>>24),(int)((s_netscan_base>>16)&0xFF),
      (int)((s_netscan_base>>8)&0xFF));
    meshcomodReply(b);
#else
    meshcomodReply("ERR: WiFi not in this build");
#endif
    return true;
  }

  // ── portscan <IP> ──────────────────────────────────────────────────
  if (strncasecmp(p,"portscan ",9)==0) {
#if defined(WIFI_SSID)||defined(MULTI_TRANSPORT_COMPANION)
    if(WiFi.status()!=WL_CONNECTED){meshcomodReply("ERR: WiFi not connected");return true;}
    if(s_netscan_active||s_portscan_active){meshcomodReply("ERR: scan running");return true;}
    IPAddress ip;
    if(!ip.fromString(p+9)){meshcomodReply("ERR: bad IP");return true;}
    s_portscan_ip=ip; s_portscan_idx=0; s_portscan_active=true;
    char b[48]; snprintf(b,sizeof(b),"portscan %s...",p+9);
    meshcomodReply(b);
#else
    meshcomodReply("ERR: WiFi not in this build");
#endif
    return true;
  }

  // ── blescan ────────────────────────────────────────────────────────
  if (strncasecmp(p,"blescan",7)==0 && (p[7]=='\0'||p[7]==' '||p[7]=='\t')) {
    if(s_bleStarted){meshcomodReply("ERR: BLE scan running");return true;}
    if(!BLEDevice::getInitialized()){meshcomodReply("ERR: BLE not ready");return true;}
    if(!s_bleScan) {
      s_bleScan=BLEDevice::getScan();
      s_bleScan->setAdvertisedDeviceCallbacks(&s_bleCb,true);
      s_bleScan->setActiveScan(true);
    }
    s_bleScan->clearResults();
    s_bleScan->start(5,nullptr,false);
    s_bleStarted=true; s_bleStartMs=millis();
    meshcomodReply("BLE scan 5s...");
    return true;
  }

  // ── lscan ──────────────────────────────────────────────────────────
  if (strncasecmp(p,"lscan",5)==0 && (p[5]=='\0'||p[5]==' '||p[5]=='\t')) {
#ifdef LSCAN_AVAILABLE
    if(s_lscan_active){meshcomodReply("ERR: lscan running");return true;}
    if(s_netscan_active||s_portscan_active){meshcomodReply("ERR: other scan running");return true;}
    s_lscan_idx=0; s_lscan_active=true;
    meshcomodReply("lscan 863-870 MHz step 200kHz...");
#else
    meshcomodReply("ERR: lscan needs SX1262 + Heltec V3 build");
#endif
    return true;
  }

  return false;
}

// ======================================================================
// cliExtLoop — вызывается из loop()
// ======================================================================

void cliExtLoop() {

  // Netscan: один хост за вызов
  if (s_netscan_active) {
    if (s_netscan_idx > s_netscan_total) {
      meshcomodReply("netscan done");
      s_netscan_active=false;
    } else {
      netScanOneHost(s_netscan_idx++);
    }
    return;
  }

  // Portscan: один порт за вызов
  if (s_portscan_active) {
    uint16_t port=kPorts[s_portscan_idx];
    if (port==0) {
      meshcomodReply("portscan done");
      s_portscan_active=false;
    } else {
      if (tcpProbe(s_portscan_ip,port,80)) {
        char line[64];
        snprintf(line,sizeof(line),"%5u OPEN  %s",
                 port, kPortNames[s_portscan_idx]?kPortNames[s_portscan_idx]:"");
        meshcomodReply(line);
      }
      s_portscan_idx++;
    }
    return;
  }

  // BLE scan: ждём завершения 5с
  if (s_bleStarted && (millis()-s_bleStartMs >= kBleDur)) {
    int found=s_bleScan->getResults().getCount();
    char b[40]; snprintf(b,sizeof(b),"blescan done, found %d",found);
    meshcomodReply(b);
    s_bleStarted=false;
  }

  // LoRa scan: один канал за вызов
#ifdef LSCAN_AVAILABLE
  if (s_lscan_active) {
    float freq=kLscanStart+s_lscan_idx*kLscanStep;

    // Жёсткая проверка диапазона 863–870 МГц
    if (freq < 863.0f || freq > 870.0f) {
      meshcomodReply("ERR: lscan freq out of 863-870 range");
      radio_set_params(LORA_FREQ, LORA_BW, LORA_SF, 5);
      s_lscan_active=false; return;
    }

    if (s_lscan_idx >= kLscanTotal) {
      radio_set_params(LORA_FREQ, LORA_BW, LORA_SF, 5);
      meshcomodReply("lscan done");
      s_lscan_active=false; return;
    }

    float rssi=radio_get_instant_rssi(freq);
    if (rssi > kLscanNoise) {
      char line[48];
      snprintf(line,sizeof(line),"%.1f MHz  RSSI=%.0f dBm ***",freq,rssi);
      meshcomodReply(line);
    }
    s_lscan_idx++;
  }
#endif
}

#endif // ESP32
