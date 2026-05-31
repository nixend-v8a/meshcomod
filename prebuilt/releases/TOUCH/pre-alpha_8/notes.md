# Meshcomod TOUCH — pre-alpha_8

Ninth pre-alpha drop of the Meshcomod *touch* UI. The headline is a **second
supported board: the LilyGo T-Deck / T-Deck Plus** — the same touch firmware now
ships for both the Heltec V4 TFT and the T-Deck.

Targets in this drop:
- `heltec_v4_tft_companion_radio_usb_tcp_touch` — Heltec WiFi LoRa 32 V4 (TFT + cap touch)
- `LilyGo_TDeck_companion_radio_touch` — **new** — LilyGo T-Deck / T-Deck Plus (TFT + cap touch + trackball + QWERTY keyboard, optional GPS)

Builds on top of [pre-alpha_7](../pre-alpha_7/notes.md). Flashing the app-only
`.bin` at offset `0x10000` keeps contacts, channels, profile, chat history and
Wi-Fi credentials intact.

## What's new since pre-alpha_7

**LilyGo T-Deck support (new board)**
- Full on-device touch UI on the T-Deck: GT911 capacitive touch, the trackball
  as an on-screen cursor + click, and the physical QWERTY keyboard wired into
  every text field (chat composer, terminal, file editor, search).
- **Screen lock**: press **space** to lock (a 3 s "Locking…" countdown you can
  cancel by tapping or pressing another key); **click the trackball to unlock**.
  Touch and trackball-scroll won't wake a locked screen. The panel also auto-
  dims after idle and wakes on touch. The keyboard backlight stays dark while
  locked.
- Keyboard backlight modes (off / on / auto) in the status-bar control center;
  auto lights on a keypress or when a text field is focused.

**Chat**
- Conversations and channels render **full-width**; sent bubbles align cleanly
  to the right.
- **@mentions**: long-press a message to @mention its sender; a channel/message
  where you're mentioned shows a blue **@** next to the unread counter and the
  mentioned bubble is highlighted.
- Unread counts moved to the right of each row; a **mark-all-as-read** button
  (with confirmation) plus per-chat/channel mark-as-read on long-press.
- Read state now **persists across a reboot** (previously read threads could
  come back unread after a power cycle).

**File manager**
- Browse onboard storage and microSD: create / rename / move / copy / delete
  files and folders, a built-in text editor, and a "+" / long-press menu for
  new folders. Detects a microSD and offers a **FAT32 format** if it's
  unreadable (e.g. exFAT). microSD mounting is now far more reliable (multi-try
  init with backoff). On the Storage page, **Back** returns Home.

**Other**
- The **Send advert** button now asks **Flood vs Zero-hop**.
- Status-bar control center: **swipe up to dismiss**; a **Lock** tile (T-Deck).
- Admin-login popup no longer clips off the top of the screen.
- **Memory / stability**: the contacts table and chat buffers moved to PSRAM,
  freeing ~80 KB of scarce internal DRAM — this fixes the out-of-memory reboots
  that could hit when opening a channel while Wi-Fi (re)connected. System info
  now shows total DRAM and a task-manager-style **Memory detail** view.

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
- T-Deck: the left-side button is a hardware reset (not remappable); use the
  spacebar / control-center Lock tile to lock the screen.

## Status

Pre-alpha, now on **two boards**. Daily-driver capable for chat, settings,
advert send/receive, Wi-Fi/BLE companion link, contact sharing, file manager,
map (offline + Wi-Fi tiles) and line-of-sight planning, in portrait or
landscape.
