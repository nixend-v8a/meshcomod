# Meshcomod TOUCH — pre-alpha_5

Sixth pre-alpha drop of the Heltec V4 Wireless LoRa V4 TFT *touch* UI variant.
Target board: `heltec_v4_tft_companion_radio_usb_tcp_touch`.

Builds on top of [pre-alpha_4](../pre-alpha_4/notes.md). This drop is mostly
about the new **Map tab** and a **terrain-aware line-of-sight analyzer**, plus
a Contacts-tab redesign. Flashing the app-only `.bin` at offset `0x10000`
keeps contacts, channels, profile, chat history and Wi-Fi credentials intact.
If the Map tab reports a storage error after an app-only flash, your partition
table predates the dedicated tile store — flash the **merged** image once (see
*Flash* below); contacts/channels/profile/chat on SPIFFS are preserved, only
NVS (Wi-Fi creds + touch prefs) resets.

## What's new since pre-alpha_4

**Map tab**
- Slippy-tile map centered on your GPS, with pan, zoom (±), and recenter.
- Contacts/repeaters/rooms plotted as colored markers over the map; your own
  position is a white GPS crosshair. Tap a marker for the action sheet.
- Offline tile packs (`/tiles/<z>/<x>/<y>.jpg` on a dedicated LittleFS
  partition) **and** on-demand Wi-Fi fetching — see below.

**Wi-Fi tile fetching (no PC needed)**
- When connected to Wi-Fi, missing tiles in view are downloaded automatically
  and cached for offline use. Pan into an un-tiled area on a hotspot to
  pre-load it.
- On-device HTTPS isn't viable (mbedTLS won't fit in the post-Wi-Fi heap), so
  the device fetches plain HTTP from a small meshcomod proxy
  (`tiles.meshcomod.com`) that transcodes OSM PNG → JPEG (the device's light
  SJPG decoder path). Tiles live on their own partition, so refreshing the map
  never wipes Profile / contacts / chat history.
- Contextual status: "Downloading map tiles…" when fetching, "Wi-Fi off —
  gaps" / "Connect to Wi-Fi to download this area" when offline, and a compact
  `↓ N downloading` counter in the bottom bar.

**Contact distance + units**
- Each contact row shows the great-circle distance from your position when
  both ends have a GPS fix (e.g. `1.4km`, `350m`, `37km`).
- New **Settings → Device → "Distance in miles"** toggle (km default; applies
  instantly, persists in NVS).

**Contacts tab redesign**
- Header is now a segmented filter — **All · ★ · RPT · Peer** — with big tap
  targets and a clear active state, plus a **⋯** overflow menu for Search /
  Found / Add contact.
- Opens on **All**, sorted by **most-recently-heard** (favorites still pinned
  to top). The ★ view falls back to showing every contact when you haven't
  starred anyone (no more empty list).

**Line-of-sight analyzer ("Sightline")**
- New action-sheet entry (shown when both you and the contact have GPS).
  Samples the great-circle path, pulls a ground-elevation profile from the
  proxy's `/elev` endpoint, applies the 4/3-earth-radius curvature model and
  0.6·F1 Fresnel clearance, and renders a **terrain cross-section** with a
  verdict: LINE OF SIGHT / MARGINAL / NO LINE OF SIGHT, with the obstruction
  distance and how far terrain rises over the sight line.
- **+/- buttons under each end of the graph** set your and the contact's
  antenna height (1 m steps); the analysis redraws instantly (no re-fetch),
  so you can watch a blocked path clear as you raise a mast.
- The fetch runs on a background worker — the UI stays responsive and the
  "Analyzing…" popup can be dismissed mid-fetch.

**Action sheet**
- Contact actions are now a **2-column grid** (Delete spans the full bottom
  row) so repeaters fit without clipping.

**Performance / stability**
- Map markers always render above the (now opaque) JPEG tiles.
- 60 Hz refresh / internal-DRAM draw buffer tuning; tile fetcher and LOS share
  one core-0 worker so the UI thread is never blocked by network I/O.

## Flash

See [pre-alpha_0 notes](../pre-alpha_0/notes.md#flash). Short version:

1. **Recommended (NVS-safe):** flash
   `heltec_v4_tft_companion_radio_usb_tcp_touch.bin` at offset `0x10000`, or
   run `scripts/flash-touch.sh` from a source checkout. Keeps Wi-Fi creds,
   contacts, channels, profile and chat history.
2. **Merged / recovery / first-time:** flash
   `heltec_v4_tft_companion_radio_usb_tcp_touch-merged.bin` at offset `0x0`.
   Use this if the Map tab shows a storage error (older partition table).
   Resets NVS (Wi-Fi creds + touch prefs like favorites / units / quick
   replies); SPIFFS data (contacts / channels / profile / chat) is preserved.

## Map setup

- Offline packs: `scripts/build/tile-pack.py` → `scripts/build/upload-tiles.py`
  (writes only the tiles partition; Profile / contacts / chats untouched).
- Online: just connect Wi-Fi (Settings → Wi-Fi) and open the Map tab; tiles
  stream in via `tiles.meshcomod.com` and cache to flash for offline use.

## Known issues

- DMs from a peer whose advert hasn't been received yet cannot decrypt
  (MeshCore protocol-level). Auto-add for chat-type adverts is on.
- Camera-side QR *scanner* still not shipped — Share-my-contact is one-way.
- Wi-Fi tile/elevation fetching depends on the meshcomod proxy being
  reachable; on a cold path the first tiles/LOS can take a few seconds (then
  they're cached). The elevation backend (SRTM via opentopodata) rate-limits,
  so a brand-new LOS path occasionally needs a second try.

## Status

Pre-alpha. Daily-driver capable for chat, settings, advert send/receive,
Wi-Fi/BLE companion link, contact sharing, **map (offline + Wi-Fi tiles)** and
**line-of-sight planning**.
