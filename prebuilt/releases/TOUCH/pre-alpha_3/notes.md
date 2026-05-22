# Meshcomod TOUCH — pre-alpha_3

Fourth pre-alpha drop of the Heltec V4 Wireless LoRa V4 TFT *touch* UI variant.
Target board: `heltec_v4_tft_companion_radio_usb_tcp_touch`.

Builds on top of [pre-alpha_2](../pre-alpha_2/notes.md). Flashing the
app-only `.bin` at offset `0x10000` keeps your saved contacts, channels,
profile and Wi-Fi credentials intact.

## What's new since pre-alpha_2

- **Long-press → delete now survives a reboot.** Deleting a chat or
  removing a channel used to schedule the UI thread-list write lazily
  (with a 2 s deferral while the user was on the chat inbox), so if you
  rebooted in that window the on-disk thread list still contained the
  deleted thread and it came back on next boot — even though the channel
  slot in the underlying mesh was already wiped. `removeThread` now
  flushes the UI history synchronously, so the deletion is durable the
  moment the popup is confirmed.

## Flash

See [pre-alpha_0 notes](../pre-alpha_0/notes.md#flash). Short version:

1. Use `heltec_v4_tft_companion_radio_usb_tcp_touch.bin` at offset
   `0x10000` (NVS preserved — recommended when updating from any earlier
   touch build).
2. Or run `scripts/flash-touch.sh` from a source checkout — also NVS-safe.

## Known issues

Same set carried over from pre-alpha_2:

- Touch-UI repeater ping reply sometimes doesn't reach `onContactResponse`
  on the touch side even though the same repeater replies fine to a
  meshcore TCP client. 30 s "No reply" timeout shows the user something.
- DMs from a peer whose advert hasn't been received yet cannot decrypt
  (Meshcore protocol-level — TXT_MSG only carries a hash prefix of the
  sender's pub key). The sender's app should flood an advert before the
  first DM; auto-add for chat-type adverts always engages on this build.

## Status

Pre-alpha. Daily-driver capable for chat (DMs + public + hashtag channels +
private channels), settings, advert send/receive, Wi-Fi/BLE companion link.
