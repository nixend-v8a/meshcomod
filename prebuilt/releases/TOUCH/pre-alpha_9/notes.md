# Meshcomod TOUCH — pre-alpha_9

Tenth pre-alpha drop of the Meshcomod *touch* UI. Headline: a proper **lock
screen** on the LilyGo T-Deck — a wallpaper with a live clock and lock-state
text, plus hold-to-unlock — and a new **image viewer** in the file manager.

Targets in this drop:
- `heltec_v4_tft_companion_radio_usb_tcp_touch` — Heltec WiFi LoRa 32 V4 (TFT + cap touch)
- `LilyGo_TDeck_companion_radio_touch` — LilyGo T-Deck / T-Deck Plus (TFT + cap touch + trackball + QWERTY keyboard, optional GPS)

Builds on top of [pre-alpha_8](../pre-alpha_8/notes.md). Flashing the app-only
`.bin` at offset `0x10000` keeps contacts, channels, profile, chat history and
Wi-Fi credentials intact.

## What's new since pre-alpha_8

**Lock screen (T-Deck)**
- A real lock screen: a full-screen **wallpaper** with a **live clock**,
  "Screen locked", and an unlock hint drawn over it. The status bar goes
  transparent so the wallpaper shows through behind the icons.
- **Lock** with **space** (a 1 s "Locking…" countdown — down from 3 s) or the
  control-center **Lock** tile; the screen then goes dark.
- While locked, **any key or trackball click reveals** the lock screen (lights
  it) without unlocking; **hold the trackball ~1 s to unlock**, with a matching
  "Unlocking…" countdown. Touch does nothing while locked.
- **Settings → Device → Lock screen**: choose the **wallpaper** (any JPEG in
  internal `/lock/` or on an SD card) and the **text colour** of the clock and
  lock text. The built-in wallpaper is embedded pre-dithered so its gradient
  stays smooth on the 16-bit panel instead of banding.
- Fix: the idle screen-dim no longer fires on **every trackball click** (a timer
  underflow in pre-alpha_8 that dimmed the screen on each click).

**File manager**
- New **image viewer**: tap a JPEG to view it, with a **full-screen** toggle.
  (This hardware can't decode PNG without producing noise — convert to JPEG; the
  viewer now says so instead of rendering static.)
- MeshCore's internal data files (the per-contact blobs, prefs, logs) are now
  **hidden by default**, with a "Show system files" toggle, so onboard storage
  isn't cluttered. A `/lock` folder holds lock-screen wallpapers.

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
- Lock-screen custom wallpapers must be **JPEG** (PNG isn't decodable here); a
  heavily-graded custom JPEG can still band a little — the built-in wallpaper is
  embedded pre-dithered and stays clean.
- T-Deck: the left-side button is a hardware reset (not remappable); use the
  spacebar / control-center Lock tile to lock the screen.

## Status

Pre-alpha, on **two boards**. Daily-driver capable for chat, settings, advert
send/receive, Wi-Fi/BLE companion link, contact sharing, file manager (with the
new image viewer), map (offline + Wi-Fi tiles) and line-of-sight planning, in
portrait or landscape — now with a lock screen on the T-Deck.
