## v1.15.0.5 — 2026-05-22

**Firmware version:** v1.15.0.5 (meshcomod on upstream **MeshCore 1.15.0**).

**Highlights**

- **Heltec V4 OLED long-press hibernate fix:** On the SHUTDOWN/POWER tab in the on-device UI, long-pressing the user button now actually triggers hibernate. Previously the screen showed "HIBERNATE: long press" but the press was silently ignored, because the handler only accepted `KEY_ENTER` while single-button long-press emits `KEY_LONG_ENTER`. Handler now accepts both (same pattern already used for the ADVERT and GPS tabs). Benefits any single-button companion device that navigates to the SHUTDOWN tab.
- **V4 TFT builds dropped from this drop:** Only V4 OLED, V3, Wireless Paper, and Xiao S3 WIO are produced from this folder going forward. If you need TFT, build it from the upstream/separate tree.

**Primary prebuilts (flasher / OTA)**

| Device | Merged (0x0) | App-only |
|--------|----------------|----------|
| Heltec V4 (OLED) | [heltec_v4_companion_radio_usb_tcp-merged.bin](heltec_v4_companion_radio_usb_tcp-merged.bin) | [heltec_v4_companion_radio_usb_tcp.bin](heltec_v4_companion_radio_usb_tcp.bin) |
| Heltec V3 | [Heltec_v3_companion_radio_usb_tcp-merged.bin](Heltec_v3_companion_radio_usb_tcp-merged.bin) | [Heltec_v3_companion_radio_usb_tcp.bin](Heltec_v3_companion_radio_usb_tcp.bin) |
| Heltec Wireless Paper (E213) | [Heltec_Wireless_Paper_companion_radio_usb_tcp-merged.bin](Heltec_Wireless_Paper_companion_radio_usb_tcp-merged.bin) | [Heltec_Wireless_Paper_companion_radio_usb_tcp.bin](Heltec_Wireless_Paper_companion_radio_usb_tcp.bin) |
| Seeed Xiao S3 WIO SX1262 | [Xiao_S3_WIO_companion_radio_usb_tcp-merged.bin](Xiao_S3_WIO_companion_radio_usb_tcp-merged.bin) | [Xiao_S3_WIO_companion_radio_usb_tcp.bin](Xiao_S3_WIO_companion_radio_usb_tcp.bin) |

**Build:** `FIRMWARE_VERSION=v1.15.0.5`, git **`6f19e88c`** (parent of this promotion; short SHA embedded in `out/` filenames).

**Procedure:** [`docs/RELEASE_PROCEDURE.md`](../../../docs/RELEASE_PROCEDURE.md), [`scripts/copy-release-bins.sh`](../../../scripts/copy-release-bins.sh).
