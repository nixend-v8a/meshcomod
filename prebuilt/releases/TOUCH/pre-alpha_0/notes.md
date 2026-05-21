# Meshcomod TOUCH — pre-alpha_0

First **pre-alpha** drop of the Heltec V4 Wireless LoRa V4 TFT *touch* UI variant.
Target board: `heltec_v4_tft_companion_radio_usb_tcp_touch`.

## What's in this build

UI / UX
- LVGL home page: connection status (Wi-Fi 📶 + IP, or BLE 🔵), heartbeat dot,
  battery icon + %, TX/RX dispatcher chart, Send-advert button.
- Bottom-tab icons (Home / Chats / Contacts / Settings); combined Chats inbox
  with Channels + DMs; People (contacts) with filters All / RPT / Peer / Found.
- Tap-a-contact action sheet:
  - Repeater → **Ping (status)** + Delete
  - Peer → **Send message** + Delete
- Discovered store (24-slot ring) of adverts not yet in `contacts[]`, openable
  via the **Found** filter; one-tap Add to promote into contacts.
- Settings: Profile / Radio / Auto-add / Transports / Wi-Fi / Device / Experimental
  modals, with the Wi-Fi page showing live status, SSID/password entry, radio
  toggle. Save auto-reboots to apply.
- Screen timeout (default 20 s, configurable in Settings → Device). BOOT button
  toggles the screen: **press to lock**, **press to unlock**; when locked
  manually, touch cannot unlock it (only BOOT can) — when locked from idle
  timeout, any touch wakes it.

Networking
- **BLE vs Wi-Fi mutex at boot.** ESP32-S3 internal heap can't fit Bluedroid +
  LVGL/TFT + Wi-Fi together (esp_wifi_init silently OOMs), so the firmware
  picks one at `setup()` based on whether Wi-Fi credentials are saved with
  the radio enabled. Save Wi-Fi creds in the UI → auto-reboot → Wi-Fi mode
  (TCP/WS). Clear creds → reboot → BLE mode. BLE-mode boots leave the lwIP
  stack uninitialized and never try to bind the TCP server.
- **NTP time sync** via `configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", ...)` once
  Wi-Fi associates. Device-info time display uses `localtime_r` so it shows
  CET/CEST. RTC stays UTC internally (protocol-facing).
- Public-channel messages route into the channel thread regardless of TCP/BLE
  connection state (previously stale `g_last_event` mis-routed them into a
  DM thread named after the channel).

Diagnostics / device
- Self-contained inline AES-128 ECB (commit `05ba1d95`); replaces the
  arduinolibs AES that froze the device on the V4 hardware. AES self-test
  result is printed at boot and pushed to the on-screen Diag.

## Flash

The merged image preserves the partition table + bootloader at their canonical
offsets, but writing it with `esptool write_flash 0x0 *-merged.bin` will
overwrite the NVS region (0x9000–0xe000) with `0xFF` and **wipe saved Wi-Fi
credentials** every flash. Two safer options:

1. Use the `prebuilt/releases/TOUCH/pre-alpha_0/heltec_v4_tft_companion_radio_usb_tcp_touch.bin`
   image at offset `0x10000` together with the official bootloader / partition
   table from the Arduino-ESP32 default 16 MB partition layout — NVS untouched.
2. From a source checkout, run `scripts/flash-touch.sh` which writes the four
   components at their exact offsets and leaves the NVS region intact.

## Known issues

- Touch-UI repeater ping reply: request fires correctly (`sendRequest` returns
  SENT_FLOOD / SENT_DIRECT), but the response sometimes doesn't reach
  `onContactResponse` on the touch side even though the same repeater replies
  fine to a meshcore TCP client connected to the same device. A 30 s "No
  reply from <name>" timeout is shown so the user has visible feedback.
- After a manual flash that holds BOOT during reset, GPIO0 can read LOW for
  long enough to be mistaken as a press. The polling code samples the real
  state on first iter to avoid a spurious lock at boot.

## Status

Pre-alpha. UI is reactive and core flows (DMs, channels, advert RX/TX, Wi-Fi
or BLE companion to the meshcore client) work. Some features (Aux button
action, manual contact ordering, OTA over Wi-Fi) are not wired yet.
