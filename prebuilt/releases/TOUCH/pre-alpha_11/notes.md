# Meshcomod TOUCH — pre-alpha_11

Twelfth pre-alpha drop of the Meshcomod *touch* UI. On top of
[pre-alpha_10](../pre-alpha_10/notes.md): a **first-boot setup wizard** and a
round of **Wi-Fi reliability fixes** (a crash, a stuck-connecting bug, and the
scan picker not sticking).

Targets in this drop:
- `heltec_v4_tft_companion_radio_usb_tcp_touch` — Heltec WiFi LoRa 32 V4 (TFT + cap touch)
- `LilyGo_TDeck_companion_radio_touch` — LilyGo T-Deck / T-Deck Plus (TFT + cap touch + trackball + QWERTY keyboard, optional GPS)

Flashing the app-only `.bin` at offset `0x10000` keeps contacts, channels,
profile, chat history and Wi-Fi credentials intact.

## What's new since pre-alpha_10

**First-boot setup wizard**
- A one-time guided flow on a fresh flash: **Welcome → Name → Region → Wi-Fi**.
  Pick how you appear to other nodes, choose your LoRa region from the full
  preset list, and (optionally) join Wi-Fi — then it reboots into the OS.
- **Skip** on the welcome screen for anyone who'd rather configure later; the
  Name and Region steps note they can be changed later in Settings.
- Re-runnable any time from **Settings → Device → Run setup again**.

**Wi-Fi reliability**
- **Crash fix:** scanning for networks on a fresh (BLE-active) device could
  panic — bringing the Wi-Fi driver up while Bluetooth holds the internal heap
  OOMs `esp_wifi_init`. Every scan path is now guarded (and the wizard hides
  *Scan* until Wi-Fi is the live transport), so it can't happen.
- **"Stuck connecting" fix:** the wizard now trims stray spaces from the SSID /
  password before saving, so a trailing space no longer means "AP not found"
  and an endless *connecting…*.
- **Scan picker fix:** picking a network from the scan list now *sticks* — it
  no longer silently reverts to the previously-entered SSID (which also caused
  the wrong credentials to be saved). Affects both the Network tab and the
  wizard.

**Settings polish**
- **Swiping** between the settings sub-tabs now loads each page (previously only
  tapping the tab did).

**Under the hood**
- Fixed a chat-history regression where the shared background worker could
  overflow its stack (after the update-checker + Wi-Fi-scan additions) and
  zero the saved message count. The worker stack is enlarged and the update
  check reuses the worker's HTTP client. (Note: messages already lost to the
  earlier builds can't be recovered, but new messages persist correctly.)

## Flash

Short version (see [pre-alpha_0 notes](../pre-alpha_0/notes.md#flash) for the
full walkthrough). Pick the file for your board:

| Board | App-only (NVS-safe, `0x10000`) | Merged / recovery (`0x0`) |
|-------|--------------------------------|---------------------------|
| Heltec V4 TFT | `heltec_v4_tft_companion_radio_usb_tcp_touch.bin` | `heltec_v4_tft_companion_radio_usb_tcp_touch-merged.bin` |
| LilyGo T-Deck | `LilyGo_TDeck_companion_radio_touch.bin` | `LilyGo_TDeck_companion_radio_touch-merged.bin` |

1. **Recommended (NVS-safe):** flash the app-only `.bin` at offset `0x10000`.
   Keeps Wi-Fi creds, contacts, channels, profile and chat history. (The setup
   wizard only auto-appears on a device that hasn't completed it — use
   *Settings → Device → Run setup again* to see it.)
2. **Merged / recovery / first-time:** flash the `-merged.bin` at offset `0x0`.
   Resets NVS (Wi-Fi creds + touch prefs), so the **setup wizard appears on
   first boot**. SPIFFS data (contacts / channels / profile / chat) is preserved.

T-Deck note: if auto-reset during flashing is flaky, hold the trackball (BOOT)
while tapping reset to enter download mode. A charge-only USB cable shows up as
"no serial data" — use a data cable.

## Known issues

- DMs from a peer whose advert hasn't been received yet cannot decrypt
  (MeshCore protocol-level). Auto-add for chat-type adverts is on.
- Camera-side QR *scanner* still not shipped — Share-my-contact is one-way.
- Map tile / elevation fetching depends on the meshcomod proxy.
- Multi-byte routing needs MeshCore ≥ v1.14 on every repeater in the path.
- Live Wi-Fi scanning requires Wi-Fi already on (Bluetooth and Wi-Fi can't share
  the radio heap simultaneously); on a fresh device, type the SSID in the wizard
  and scan later from Settings → Network.
