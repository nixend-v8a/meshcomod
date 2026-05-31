# Meshcomod TOUCH — pre-alpha_10

Eleventh pre-alpha drop of the Meshcomod *touch* UI. A small settings-focused
drop on top of [pre-alpha_9](../pre-alpha_9/notes.md): a manual **clock offset**
and a clear **multi-byte routing** control.

Targets in this drop:
- `heltec_v4_tft_companion_radio_usb_tcp_touch` — Heltec WiFi LoRa 32 V4 (TFT + cap touch)
- `LilyGo_TDeck_companion_radio_touch` — LilyGo T-Deck / T-Deck Plus (TFT + cap touch + trackball + QWERTY keyboard, optional GPS)

Flashing the app-only `.bin` at offset `0x10000` keeps contacts, channels,
profile, chat history and Wi-Fi credentials intact.

## What's new since pre-alpha_9

**Clock offset (Settings → Device)**
- A "Time offset (vs automatic)" stepper nudges the displayed clock by **±whole
  hours** on top of the automatic (NTP / companion / mesh) time — handy when
  you're offline or in a different time zone. Applied live, persists across
  reboot, and the Wi-Fi NTP sync honours it. Display-only: the RTC and outgoing
  mesh timestamps stay UTC. The time zone is now also set at boot so local time
  is correct without a network.

**Multi-byte routing (Settings → Radio params)**
- A clear "Multi-byte routing (path hash)" control — **1 byte (legacy) / 2 bytes
  / 3 bytes** — sets how many bytes of each repeater's hash this node stamps into
  the path when it adverts / sends. 1-byte hashes collide in dense regions;
  2–3 bytes disambiguate. (This was already implemented and applied to sends;
  the control was previously buried in the Advert/path-policy modal as the
  cryptic "Mode 0/1/2" — now relabelled and surfaced in Radio params.)
  ⚠️ Every repeater on a path must run MeshCore ≥ v1.14 or it silently drops
  2/3-byte packets — only raise this once your region's repeaters support it.

## Flash

Short version (see [pre-alpha_0 notes](../pre-alpha_0/notes.md#flash) for the
full walkthrough). Pick the file for your board:

| Board | App-only (NVS-safe, `0x10000`) | Merged / recovery (`0x0`) |
|-------|--------------------------------|---------------------------|
| Heltec V4 TFT | `heltec_v4_tft_companion_radio_usb_tcp_touch.bin` | `heltec_v4_tft_companion_radio_usb_tcp_touch-merged.bin` |
| LilyGo T-Deck | `LilyGo_TDeck_companion_radio_touch.bin` | `LilyGo_TDeck_companion_radio_touch-merged.bin` |

1. **Recommended (NVS-safe):** flash the app-only `.bin` at offset `0x10000`.
   Keeps Wi-Fi creds, contacts, channels, profile and chat history.
2. **Merged / recovery / first-time:** flash the `-merged.bin` at offset `0x0`.
   Resets NVS (Wi-Fi creds + touch prefs); SPIFFS data (contacts / channels /
   profile / chat) is preserved.

T-Deck note: if auto-reset during flashing is flaky, hold the trackball (BOOT)
while tapping reset to enter download mode. A charge-only USB cable shows up as
"no serial data" — use a data cable.

## Known issues

- DMs from a peer whose advert hasn't been received yet cannot decrypt
  (MeshCore protocol-level). Auto-add for chat-type adverts is on.
- Camera-side QR *scanner* still not shipped — Share-my-contact is one-way.
- Map tile / elevation fetching depends on the meshcomod proxy.
- Multi-byte routing needs MeshCore ≥ v1.14 on every repeater in the path.
- Lock-screen custom wallpapers must be JPEG (PNG isn't decodable here); the
  built-in wallpaper is embedded pre-dithered and stays clean.
- T-Deck: the left-side button is a hardware reset (not remappable); use the
  spacebar / control-center Lock tile to lock the screen.

## Status

Pre-alpha, on **two boards**. Daily-driver capable for chat, settings, advert
send/receive, Wi-Fi/BLE companion link, contact sharing, file manager (with the
image viewer), map (offline + Wi-Fi tiles) and line-of-sight planning, in
portrait or landscape, with a T-Deck lock screen — now with a manual clock
offset and an exposed multi-byte routing control.
