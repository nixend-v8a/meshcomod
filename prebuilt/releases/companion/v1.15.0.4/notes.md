## v1.15.0.4 — 2026-05-18

**Firmware version:** v1.15.0.4 (meshcomod on upstream **MeshCore 1.15.0**).

**Highlights**

- **Wi-Fi WebSocket fix:** Resolves the bug where the webclient (`connect.meshcomod.com`) failed to connect over Wi-Fi for multiple users — TCP handshake completed but the device immediately closed the connection (no data exchanged). Caused by stuck client slots after a half-closed previous session; `WiFiClient::connected()` keeps reporting `true` on a half-closed socket so the slot was never freed, and once both slots were occupied every new TCP accept was immediately `stop()`-ed.
- **Fix details:** `WebSocketCompanionServer` now timestamps each accept, evicts the oldest slot when the client table is full (rather than rejecting the new connection), and prunes half-open clients whose WebSocket upgrade never completed within 5 seconds.
- **Companion matrix refreshed:** V4 OLED, V4 TFT+touch, V3, Wireless Paper, and Xiao S3 WIO USB+TCP builds (merged + app-only).

**Primary prebuilts (flasher / OTA)**

| Device | Merged (0x0) | App-only |
|--------|----------------|----------|
| Heltec V4 (OLED) | [heltec_v4_companion_radio_usb_tcp-merged.bin](heltec_v4_companion_radio_usb_tcp-merged.bin) | [heltec_v4_companion_radio_usb_tcp.bin](heltec_v4_companion_radio_usb_tcp.bin) |
| Heltec V4 TFT + touch | [heltec_v4_tft_companion_radio_usb_tcp_touch-merged.bin](heltec_v4_tft_companion_radio_usb_tcp_touch-merged.bin) | [heltec_v4_tft_companion_radio_usb_tcp_touch.bin](heltec_v4_tft_companion_radio_usb_tcp_touch.bin) |
| Heltec V3 | [Heltec_v3_companion_radio_usb_tcp-merged.bin](Heltec_v3_companion_radio_usb_tcp-merged.bin) | [Heltec_v3_companion_radio_usb_tcp.bin](Heltec_v3_companion_radio_usb_tcp.bin) |
| Heltec Wireless Paper (E213) | [Heltec_Wireless_Paper_companion_radio_usb_tcp-merged.bin](Heltec_Wireless_Paper_companion_radio_usb_tcp-merged.bin) | [Heltec_Wireless_Paper_companion_radio_usb_tcp.bin](Heltec_Wireless_Paper_companion_radio_usb_tcp.bin) |
| Seeed Xiao S3 WIO SX1262 | [Xiao_S3_WIO_companion_radio_usb_tcp-merged.bin](Xiao_S3_WIO_companion_radio_usb_tcp-merged.bin) | [Xiao_S3_WIO_companion_radio_usb_tcp.bin](Xiao_S3_WIO_companion_radio_usb_tcp.bin) |

**Build:** `DISABLE_DEBUG=1`, `FIRMWARE_VERSION=v1.15.0.4`, git **`ec6b3897`** (parent of this promotion; short SHA embedded in `out/` filenames).

**Procedure:** [`docs/RELEASE_PROCEDURE.md`](../../../docs/RELEASE_PROCEDURE.md), [`scripts/copy-release-bins.sh`](../../../scripts/copy-release-bins.sh).
