# Meshcomod TOUCH — pre-alpha_4

Fifth pre-alpha drop of the Heltec V4 Wireless LoRa V4 TFT *touch* UI variant.
Target board: `heltec_v4_tft_companion_radio_usb_tcp_touch`.

Builds on top of [pre-alpha_3](../pre-alpha_3/notes.md). Flashing the
app-only `.bin` at offset `0x10000` keeps your saved contacts, channels,
profile and Wi-Fi credentials intact — but **chat history (per-message
bubbles, unread counts, last_ts) is reset** on first boot because the
on-disk UI history blob bumped to v4 to make room for per-message RX
metadata (hops / SNR / RSSI). Contacts themselves are untouched.

## What's new since pre-alpha_3

**Tactical UI overhaul**
- Pure-black background everywhere, low-opacity neutral-gray buttons,
  COLOR_STATUS_OK / WARN / DANGER for state — modelled after Harris /
  Codan / Nett-Warrior-style equipment UIs, not Hollywood "tacticool".
- Splash screen reduced to a plain white "MESHCOMOD" wordmark fading up
  on black.

**Global status bar**
- Always-visible 22 px strip on `lv_layer_sys` (above every modal, even
  the keyboard) showing time, battery %, battery icon, and a Wi-Fi /
  Bluetooth indicator that follows whichever transport is up.
- Home tab shows the "MESHCOMOD" wordmark in the left zone; every other
  tab shows an envelope + unread count when there's unread mail, and
  blanks out when the inbox is empty.

**Chat bubbles**
- Each bubble now carries an HH:MM timestamp anchored bottom-right (uses
  the RTC when synced — NTP over Wi-Fi or manual Set Date/Time on the
  Settings tab — and falls back to "RTC unset" before that).
- Long-press a bubble opens a Copy / Info menu. **Info** shows full
  date+time, sender, direction (incoming/outgoing, DM/channel), flood
  vs direct path with hop count, SNR (dB), RSSI (dBm), and delivery
  status for outgoing DMs.
- Outgoing DM delivery indicator (✓ / ✓✓ / ✗) now sits inline with the
  timestamp instead of in isolation.

**Favorites**
- Per-contact favorite toggle in the contact action sheet; favorites
  filter chip pins the row marker.
- Magnifier / search overlay for filtering by name fragment.
- Pinned-to-top sort + snapshot-once NVS read for fast list refresh
  even with dozens of contacts.

**Repeater admin**
- Built-in admin console with login + remembered passwords (NVS),
  command picker with curated commands, scrolling CLI log with
  command echo.
- Skip eager-keyboard pop when password is already prefilled — typical
  flow is just "tap Login".

**Telemetry**
- Telemetry replies now properly decode CayenneLPP payloads (was
  showing raw hex). Tag-matching in `onContactResponse` so chained
  guest-login + telemetry-request flows don't get misrouted.

**Share my contact (QR)**
- Chats tab gains a small QR button on the left of the header. Tap →
  popup with an `lv_qrcode` encoding `meshcore://add?p=<pubkey>&n=<name>`,
  plus a short pubkey prefix for voice-verification.
- Same popup is reachable from Settings → Profile.

**Misc polish**
- Top-right `×` close badge on every popup modal (consistent dismiss
  affordance, 32×32 tap area).
- Chat composer row re-tuned to symmetric 8-px gaps around the
  textarea — was visibly crowded.
- Filter chips on Contacts tab resized to match the Chats tab `+`
  button so the headers visually line up.
- Quick-reply macro defaults swapped to operator-vocabulary set:
  "copy" / "wilco" / "stand by" / "moving to RP" / "ETA 5 min" / "RTB".
- Real ★ glyph (U+2605) shipped as a tiny font subset — Montserrat
  doesn't include it.

**Bug fixes**
- Settings textareas no longer pop the keyboard when you start a
  vertical scroll on them (dropped `LV_EVENT_PRESSED` from the focus
  handler, kept the scroll-guard on CLICKED).
- Settings modal **Close** button works reliably while keyboard is
  open — the keyboard mirror strip was sitting on top of it. Mirror
  now positions below the modal header when one is open.
- Keyboard rotation arrows position above the chat **composer** when
  the chat panel is active, instead of overlapping the QR / Send
  buttons.
- Per-message Info popup uses true screen-space coords for layout
  rather than parent-relative.
- Pre-alpha_4 bootloop on contact list (sibling label inside
  `lv_list_add_btn` flex) — fixed by switching to embedded `\n` in
  the single label.

## Flash

See [pre-alpha_0 notes](../pre-alpha_0/notes.md#flash). Short version:

1. Use `heltec_v4_tft_companion_radio_usb_tcp_touch.bin` at offset
   `0x10000` (NVS preserved — recommended when updating from any earlier
   touch build). Chat-history blob will reset on first boot, contacts
   stay.
2. Or run `scripts/flash-touch.sh` from a source checkout — also NVS-safe.

## Known issues

- DMs from a peer whose advert hasn't been received yet cannot decrypt
  (MeshCore protocol-level — TXT_MSG only carries a hash prefix of the
  sender's pub key). The sender's app should flood an advert before the
  first DM; auto-add for chat-type adverts always engages on this build.
- A camera-side QR *scanner* isn't shipped yet — the Share-my-contact
  QR popup is one-way for now (others scan it with their phone or
  another firmware that grows a camera later).

## Status

Pre-alpha. Daily-driver capable for chat (DMs + public + hashtag channels +
private channels + repeater admin), settings, advert send/receive,
Wi-Fi/BLE companion link, contact sharing.
