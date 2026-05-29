# Meshcomod TOUCH — pre-alpha_6

Seventh pre-alpha drop of the Heltec V4 Wireless LoRa V4 TFT *touch* UI variant.
Target board: `heltec_v4_tft_companion_radio_usb_tcp_touch`.

Builds on top of [pre-alpha_5](../pre-alpha_5/notes.md). This is a
**stability and reliability** pass — no new tabs, but it fixes the
device-wide slowdown that could make typing or opening a chat take several
seconds, makes chat history survive reboots cleanly, and makes the
line-of-sight analyzer work consistently. Flashing the app-only `.bin` at
offset `0x10000` keeps contacts, channels, profile, chat history and Wi-Fi
credentials intact.

## What's new since pre-alpha_5

**Performance — the big fix**
- Fixed a device-wide slowdown where opening a DM, receiving a message, or
  even typing could freeze the UI for several seconds at a time. Root cause:
  stale map tiles left on the SPIFFS *data* partition by an early build had
  filled it to ~78%, so every small chat-history write triggered a
  multi-second flash garbage-collection storm — and a flash write stalls
  **both** CPU cores, freezing the whole UI.
- The firmware now purges those stale tiles on boot (map tiles live on their
  own dedicated partition now, so nothing of value is lost). On the first
  boot after flashing you may see it reclaim space in the Settings diagnostic
  panel (`Purged N SPIFFS images` / `SPIFFS X/Y KB used`); it drops from
  ~2.4 MB used back to well under 100 KB. Typing, opening chats and receiving
  messages are snappy again.

**Chat history survives a reboot**
- Chat history (per-message bubbles, unread counts, last-heard) is now saved
  synchronously on reboot, shutdown, and Wi-Fi/BLE mode switches, and written
  promptly after each message from internal RAM. A normal reboot keeps your
  conversation; only an unclean reset (e.g. an `esptool` flash with no
  shutdown hook) can lose the last few seconds of unsaved messages.

**Line-of-sight "Sightline" — reliable now**
- The analyzer retries the terrain-elevation fetch automatically (up to 3×)
  instead of giving up on the first hiccup. The proxy caches elevation
  profiles for 30 days, so the first attempt warms the cache and the retry is
  near-instant; the spacing between attempts also rides out the elevation
  backend's ~1 req/s rate limit on a brand-new path.
- Tolerates missing / no-data terrain samples by interpolating across them,
  so a single gap in the profile no longer fails the whole analysis.
- Shows progress while it works ("Analyzing terrain… retry 2 of 3") so a slow
  cold path reads as working rather than frozen.

## Flash

See [pre-alpha_0 notes](../pre-alpha_0/notes.md#flash). Short version:

1. **Recommended (NVS-safe):** flash
   `heltec_v4_tft_companion_radio_usb_tcp_touch.bin` at offset `0x10000`, or
   run `scripts/flash-touch.sh` from a source checkout. Keeps Wi-Fi creds,
   contacts, channels, profile and chat history. (First boot reclaims the
   stale-tile space described above — this is one-time.)
2. **Merged / recovery / first-time:** flash
   `heltec_v4_tft_companion_radio_usb_tcp_touch-merged.bin` at offset `0x0`.
   Use this if the Map tab shows a storage error (older partition table).
   Resets NVS (Wi-Fi creds + touch prefs like favorites / units / quick
   replies); SPIFFS data (contacts / channels / profile / chat) is preserved.

## Known issues

- DMs from a peer whose advert hasn't been received yet cannot decrypt
  (MeshCore protocol-level). Auto-add for chat-type adverts is on.
- Camera-side QR *scanner* still not shipped — Share-my-contact is one-way.
- Wi-Fi tile/elevation fetching depends on the meshcomod proxy being
  reachable. The line-of-sight retry now hides most transient backend
  rate-limits, but a brand-new path with **no** SRTM coverage (e.g. far out
  over water) will still report "couldn't fetch terrain data".

## Status

Pre-alpha. Daily-driver capable for chat, settings, advert send/receive,
Wi-Fi/BLE companion link, contact sharing, map (offline + Wi-Fi tiles) and
line-of-sight planning. This drop focuses on making all of the above feel
fast and dependable.
