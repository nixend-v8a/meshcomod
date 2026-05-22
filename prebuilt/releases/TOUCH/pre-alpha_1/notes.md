# Meshcomod TOUCH — pre-alpha_1

Second pre-alpha drop of the Heltec V4 Wireless LoRa V4 TFT *touch* UI variant.
Target board: `heltec_v4_tft_companion_radio_usb_tcp_touch`.

Builds on top of pre-alpha_0 — flashing this directly over a working
pre-alpha_0 device keeps your contacts, channels, profile and Wi-Fi
credentials intact (NVS is preserved when flashing the app-only `.bin` at
offset `0x10000`, see the "Flash" section below).

## What's new since pre-alpha_0

UI / UX
- **Animated MESHCOMOD boot splash**: the pre-LVGL "Loading…" screen is gone,
  replaced with a centered MESHCOMOD title; after LVGL comes up the splash
  animates (title fades in, an accent rule grows out from the center,
  "private mesh" tagline fades in below) and auto-removes after ~1.5 s.
  Tap to dismiss.
- **WhatsApp-style chat bubbles** in the chat detail screen. Each message is
  its own bubble: outgoing right-aligned in the sent-color, incoming
  left-aligned in the recv-color. The single-textarea "you: hi\nrx: hey\n"
  blob is gone. For channel/group chats the sender's name now appears as a
  small accent line inside the incoming bubble (parses "Name: body" from the
  wire) instead of a literal "rx" placeholder.
- **Chats tab "+" button** opens an action sheet mirroring the web client's
  Add-channel dropdown:
  - Create a private channel (name + optional 32-hex secret; empty = random)
  - Join a private channel (paste 32-hex secret)
  - Join the public channel (idempotent re-add of slot 0)
  - Join a hashtag channel (key derived as SHA-256("#" + lowercased name),
    first 16 bytes — matches the web client and meshcore-packet-knife)
- **Landscape keyboard typing**: two rotation arrows above the on-screen
  keyboard flip the display 90° clockwise or counter-clockwise while typing,
  so the keyboard takes the wider screen edge. Choice persists in NVS
  (`meshTouch/kbrot`) and is re-applied next time the keyboard opens.
  Arrows fade to ~20% opacity on first keystroke so they stay out of the way.
  Closing the keyboard always reverts the display to portrait and scrolls
  the modal/screen back to the top.
- **Live keyboard mirror**: while typing, the field under the keyboard now
  updates on every keystroke (used to only sync when the keyboard closed).
- **Wi-Fi page reboot confirmation popup**: pressing "Save / turn on" shows
  a popup explaining the radio will switch and reboot before applying.
- **Discovered (Found) page** now mentions whether auto-add is currently on.

Channels / contacts
- Channels added via the new "+" menu appear in the Chats list immediately
  (no 4 s backstop wait).
- Thread-table starvation fix: the UI's 48-slot thread table used to be
  pre-filled with "ghost" DM threads for every known contact, which would
  starve newly-added channels (`findOrCreateThread` returned -1 once the
  table was full). Empty-history DM threads are now reclaimed on every
  `refreshThreadsFromMesh`; DM threads are created lazily on first message
  or when the user explicitly opens a contact's DM.
- `MyMesh::shouldAutoAddContactType` now always returns true for
  `ADV_TYPE_CHAT` regardless of the manual-add preference, so a peer's
  advert always promotes to a contact and their subsequent DMs decode.
  Repeater / room / sensor types still honour the user's auto-add bitmask.

Performance
- LVGL allocator routed through PSRAM (`LV_MEM_CUSTOM`) so the ~60 KB of
  widget state during `buildUiTree` no longer steals internal DRAM that
  the WiFi DMA path needs.
- Touch driver moved onto a pinned core-0 FreeRTOS poll task, decoupling
  CHSC6X I²C reads from the LVGL frame timer. Tap min-duration lowered,
  release debounced over 2 polls.
- Swipes across Settings rows no longer double-fire as both a tab change
  *and* a row click (`lv_indev_wait_release` is called once a horizontal
  drag is detected).
- ST7789 SPI bus bumped to 80 MHz, draw buffer sized at 240×24 in PSRAM.

Settings layout
- "Transports" settings page replaced with a dedicated **Bluetooth** page.
  Toggle BLE on/off; the device reboots into the correct mode automatically
  (BLE/Wi-Fi are mutually exclusive on this board for the same heap reason
  documented in pre-alpha_0).

## Flash

The merged image preserves the partition table + bootloader at their canonical
offsets, but writing it with `esptool write_flash 0x0 *-merged.bin` will
overwrite the NVS region (0x9000–0xe000) with `0xFF` and **wipe saved Wi-Fi
credentials, contacts and channels** every flash. Two safer options:

1. Use the `prebuilt/releases/TOUCH/pre-alpha_1/heltec_v4_tft_companion_radio_usb_tcp_touch.bin`
   image at offset `0x10000` together with the official bootloader / partition
   table from the Arduino-ESP32 default 16 MB partition layout — NVS untouched.
   This is the recommended path when updating from pre-alpha_0.
2. From a source checkout, run `scripts/flash-touch.sh` which writes the four
   components at their exact offsets and leaves the NVS region intact.

## Known issues

(Carried over from pre-alpha_0 unless otherwise noted.)

- Touch-UI repeater ping reply: request fires correctly (`sendRequest` returns
  SENT_FLOOD / SENT_DIRECT), but the response sometimes doesn't reach
  `onContactResponse` on the touch side even though the same repeater replies
  fine to a meshcore TCP client connected to the same device. A 30 s "No
  reply from <name>" timeout is shown so the user has visible feedback.
- DMs from a peer whose advert has not yet been received cannot decrypt
  (Meshcore protocol-level: TXT_MSG carries only a hash prefix of the
  sender's pub key; the receiver needs the full key from a prior advert to
  derive the shared secret). The sender's app should flood an advert before
  the first DM. Auto-add for chat-type adverts now always engages on this
  build, so once the advert lands future DMs work.

## Status

Pre-alpha. Daily-driver capable for chat (DMs + public + hashtag channels +
private channels), settings, advert send/receive, Wi-Fi/BLE companion link.
Some features (Aux button action, manual contact ordering, OTA over Wi-Fi)
are not wired yet.
