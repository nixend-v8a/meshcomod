## r1.15.0.4 — 2026-05-18

**Repeater TCP version string (on device):** `meshcomod-r1.15.0.4-repeater-tcp-<gitsha>`.

**Highlights**

- **Wi-Fi WebSocket fix:** Same fix as companion v1.15.0.4. Resolves immediate disconnect on the `ws://device:8765` endpoint after a previous session left a slot stuck (TCP handshake completes, then immediate FIN-ACK with zero data). `WebSocketCompanionServer` now timestamps accepts, evicts the oldest slot when full, and prunes half-open clients after a 5s handshake timeout.
- **Full repeater TCP matrix:** V4 OLED, V4 TFT+touch, V3, Wireless Paper, and Xiao S3 WIO `*_repeater_tcp` app and merged images.

**Prebuilts**

| Device | Merged (0x0) | App-only |
|--------|----------------|----------|
| Heltec V4 (OLED) | [heltec_v4_repeater_tcp-merged.bin](heltec_v4_repeater_tcp-merged.bin) | [heltec_v4_repeater_tcp.bin](heltec_v4_repeater_tcp.bin) |
| Heltec V4 TFT + touch | [heltec_v4_tft_repeater_tcp-merged.bin](heltec_v4_tft_repeater_tcp-merged.bin) | [heltec_v4_tft_repeater_tcp.bin](heltec_v4_tft_repeater_tcp.bin) |
| Heltec V3 | [Heltec_v3_repeater_tcp-merged.bin](Heltec_v3_repeater_tcp-merged.bin) | [Heltec_v3_repeater_tcp.bin](Heltec_v3_repeater_tcp.bin) |
| Heltec Wireless Paper (E213) | [Heltec_Wireless_Paper_repeater_tcp-merged.bin](Heltec_Wireless_Paper_repeater_tcp-merged.bin) | [Heltec_Wireless_Paper_repeater_tcp.bin](Heltec_Wireless_Paper_repeater_tcp.bin) |
| Seeed Xiao S3 WIO SX1262 | [Xiao_S3_WIO_repeater_tcp-merged.bin](Xiao_S3_WIO_repeater_tcp-merged.bin) | [Xiao_S3_WIO_repeater_tcp.bin](Xiao_S3_WIO_repeater_tcp.bin) |

**Build:** `DISABLE_DEBUG=1`, `REPEATER_FIRMWARE_VERSION=r1.15.0.4-repeater-tcp`, git **`ec6b3897`**.

**Procedure:** [`docs/REPEATER_RELEASE_PROCEDURE.md`](../../../docs/REPEATER_RELEASE_PROCEDURE.md), [`scripts/copy-repeater-release-bins.sh`](../../../scripts/copy-repeater-release-bins.sh).
