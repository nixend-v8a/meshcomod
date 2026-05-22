# Meshcomod TOUCH — pre-alpha_2

Third pre-alpha drop of the Heltec V4 Wireless LoRa V4 TFT *touch* UI variant.
Target board: `heltec_v4_tft_companion_radio_usb_tcp_touch`.

Builds on top of [pre-alpha_1](../pre-alpha_1/notes.md). Flashing the
app-only `.bin` at offset `0x10000` keeps your saved contacts, channels,
profile and Wi-Fi credentials intact.

## What's new since pre-alpha_1

- **Chat back button is now reliable.** It used to occasionally do nothing —
  the cap-touch driver's horizontal-swipe detector was calling
  `lv_indev_wait_release` mid-tap, which aborted the LVGL click on a
  freshly-pressed button. The back button now also fires on
  `LV_EVENT_PRESSED` so the action triggers on touch-down (before the
  abort path runs), and an internal guard ignores the duplicate CLICKED
  event so back doesn't double-close anything.
- **Long-press → delete a chat / remove a channel.** Hold a row in the Chats
  list to open a confirmation popup. Confirming a DM drops the UI thread
  + its cached messages (the contact itself stays in the Contacts tab and
  can be removed there separately). Confirming a channel also frees the
  channel slot in the underlying mesh and persists, so the channel doesn't
  come back on the next boot. `lv_indev_wait_release` is called inside the
  long-press handler so the trailing CLICKED no longer opens the chat
  behind the popup.
- **Settings → Logs no longer spams `chan refresh: named=… added=…`.** That
  was leftover debug from the thread-table starvation investigation; it
  hid the genuinely interesting log lines.

## Flash

See [pre-alpha_0 notes](../pre-alpha_0/notes.md#flash) for the full
explanation. Short version:

1. Use `heltec_v4_tft_companion_radio_usb_tcp_touch.bin` at offset
   `0x10000` (NVS preserved — recommended when updating from any earlier
   touch build).
2. Or run `scripts/flash-touch.sh` from a source checkout — also NVS-safe.

The `-merged.bin` image overwrites NVS (0x9000–0xe000) when written to
offset `0x0` via `esptool write_flash`; use the app-only variant unless
you specifically want a clean wipe.

## Known issues

Carried over from earlier pre-alphas:

- Touch-UI repeater ping reply sometimes doesn't reach `onContactResponse`
  on the touch side; the request fires correctly but the response is
  occasionally swallowed. 30 s "No reply" timeout gives the user visible
  feedback.
- DMs from a peer whose advert has not yet been received cannot decrypt
  (Meshcore protocol-level: TXT_MSG carries only a hash prefix of the
  sender's pub key). The sender's app should flood an advert before the
  first DM. Auto-add for chat-type adverts always engages on this build,
  so once the advert lands future DMs work.

## Status

Pre-alpha. Daily-driver capable for chat (DMs + public + hashtag channels +
private channels), settings, advert send/receive, Wi-Fi/BLE companion link.
Some features (Aux button action, manual contact ordering, OTA over Wi-Fi)
are not wired yet.
