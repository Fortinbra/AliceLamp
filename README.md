# AliceLamp

Alice in Wonderland themed RGB LED lamp for a Raspberry Pi Pico-compatible board.

The enclosure is based on
[Alice in wonderland led lamp](https://www.printables.com/model/147050-alice-in-wonderland-led-lamp)
by [simik191](https://www.printables.com/@simik191_3192), licensed under
[CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/). That model is
a remix of
[Alice in Wonderland Cheshire Cat Silhouette Lamp](https://www.thingiverse.com/thing:4686926)
by AgentPothead.

The firmware drives one WS2812B strip arranged as a closed ring and provides six
moving, smoothly wrapped animations:

1. Blues and purples
2. Reds and oranges
3. Yellows and greens
4. Reds and blues
5. A whimsical pocket-watch clock with twelve gold markers and animated hour,
   minute, and red second hands. LED zero is the six o'clock position, placing
   twelve o'clock halfway around the strip.
6. A full-spectrum rainbow chase with three moving brightness waves

One capacitive touch input toggles the lights and the other advances to the next
animation. The default wiring assumes active-high touch modules such as the TTP223.

## Downloads

Each published GitHub release includes ready-to-flash UF2 files for:

- Raspberry Pi Pico 2: `AliceLamp-<version>-pico2.uf2`
- Pimoroni Pico Plus 2: `AliceLamp-<version>-pimoroni-pico-plus-2.uf2`
- Pimoroni PGA2350: `AliceLamp-<version>-pimoroni-pga2350.uf2`

Choose the file matching your board, hold its BOOTSEL button while connecting it,
and copy the UF2 onto the mounted `RPI-RP2` drive.

## Configuration

The defaults are:

| Setting | Default |
| --- | ---: |
| Active LED count | 80 |
| Startup clear limit | 300 |
| WS2812B data | GPIO 2 |
| Power touch output | GPIO 3 |
| Cycle touch output | GPIO 4 |

Local builds target the Pimoroni PGA2350 by default. Passing `-DPICO_BOARD=...`
to CMake overrides that target for a separate build directory.

Change these values in the CMake extension settings or reconfigure from a shell:

```powershell
cmake -S . -B build -DALICE_LED_COUNT=72 -DALICE_LED_PIN=2 `
    -DALICE_LED_CLEAR_COUNT=300 -DALICE_POWER_TOUCH_PIN=3 `
    -DALICE_CYCLE_TOUCH_PIN=4
```

`ALICE_LED_CLEAR_COUNT` must be at least the active LED count. At startup and when
turning the lamp off, the firmware sends black through this many positions so LEDs
from a previously larger configuration do not retain their last latched color.

The maximum global brightness is `brightness` in
`include/alice_lamp/config.hpp`. A gamma-2.0 correction is applied before this
brightness cap for smoother perceived transitions and subdued midtones. Keep the
Pico and LED power-supply grounds connected; power the strip from a supply sized
for the chosen LED count rather than from a Pico GPIO pin.

## Versioning

The current version is stored in `VERSION` using a two-part `MAJOR.MINOR` format.
Increment MINOR when adding backward-compatible features or animations, and MAJOR
for incompatible hardware, configuration, or behavior changes. Release tags must
match the file with a `v` prefix, such as `v0.1`.

## Proposed Features

Future feature designs are tracked in [docs](docs/README.md), beginning with
[Wi-Fi time synchronization](docs/features/wifi-time-sync.md) for an accurate
pocket-watch display.
