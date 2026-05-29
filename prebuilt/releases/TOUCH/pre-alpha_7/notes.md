# Meshcomod TOUCH — pre-alpha_7

Eighth pre-alpha drop of the Heltec V4 Wireless LoRa V4 TFT *touch* UI variant.
Target board: `heltec_v4_tft_companion_radio_usb_tcp_touch`.

Builds on top of [pre-alpha_6](../pre-alpha_6/notes.md). The headline is a
**landscape mode** (with smooth hardware rotation) and a **status-bar control
center**, plus a fix for settings buttons occasionally going unresponsive.
Flashing the app-only `.bin` at offset `0x10000` keeps contacts, channels,
profile, chat history and Wi-Fi credentials intact.

## What's new since pre-alpha_6

**Landscape mode**
- New **Settings → Device → "Orientation"** button toggles Portrait ⇄
  Landscape and reboots to apply.
- Landscape rotates the panel in **hardware**, so LVGL renders natively in
  320×240 — scrolling and animation are as smooth as portrait (no per-pixel
  software-rotation cost). Touch and swipe direction are remapped to match.
- The whole UI reflows to the wider, shorter screen: status bar, tab bar,
  Home (taller TX/RX chart with the Send-advert button parked top-right), the
  Map, the chat conversation view, all popups, and every Settings page (fields
  and buttons fill the width). The pre-LVGL boot wordmark rotates too.
- Chat/contact list rows are tighter: chat rows lose the empty padding (both
  orientations), and contact rows collapse name + type + age + distance onto a
  single line in landscape so far more fit on screen.

**Status-bar control center**
- **Tap the top status bar** for an iPhone-style drop-down: date + time,
  battery %/voltage, Wi-Fi IP, and quick **Wi-Fi / Bluetooth / GPS** toggles.
- Wi-Fi and Bluetooth are the mutually-exclusive companion transports they
  really are: turning the active one off/on is instant (no reboot); switching
  to the *other* one reboots to re-init cleanly (Bluetooth and Wi-Fi can't both
  hold the radio's internal heap). GPS toggles instantly.

**Bug fixes**
- **Settings buttons could become unresponsive** — the press animation showed
  but nothing happened, especially on the long Device page. The settings
  pages packed 20+ controls directly into the scroll view, which wedged
  LVGL's scroll machinery so taps never resolved into a click. Each page now
  scrolls a single content container, and the tap-vs-scroll threshold was
  raised so taps aren't mistaken for scrolls on the cap-touch panel.

## Flash

See [pre-alpha_0 notes](../pre-alpha_0/notes.md#flash). Short version:

1. **Recommended (NVS-safe):** flash
   `heltec_v4_tft_companion_radio_usb_tcp_touch.bin` at offset `0x10000`, or
   run `scripts/flash-touch.sh` from a source checkout. Keeps Wi-Fi creds,
   contacts, channels, profile and chat history.
2. **Merged / recovery / first-time:** flash
   `heltec_v4_tft_companion_radio_usb_tcp_touch-merged.bin` at offset `0x0`.
   Use this if the Map tab shows a storage error (older partition table).
   Resets NVS (Wi-Fi creds + touch prefs like favorites / units / orientation);
   SPIFFS data (contacts / channels / profile / chat) is preserved.

## Known issues

- DMs from a peer whose advert hasn't been received yet cannot decrypt
  (MeshCore protocol-level). Auto-add for chat-type adverts is on.
- Camera-side QR *scanner* still not shipped — Share-my-contact is one-way.
- Landscape is a first full pass; a stray screen may still have a portrait-era
  margin. Map tile/elevation fetching still depends on the meshcomod proxy.

## Status

Pre-alpha. Daily-driver capable for chat, settings, advert send/receive,
Wi-Fi/BLE companion link, contact sharing, map (offline + Wi-Fi tiles) and
line-of-sight planning — now in portrait **or** landscape, with a quick-access
control center.
