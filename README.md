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

## How It Works

AliceLamp is C++17 firmware built on the Raspberry Pi Pico SDK. A PIO state machine
drives the WS2812B data stream without blocking the application loop. The main loop
debounces both touch inputs, advances the selected animation, and renders the logical
LED ring at a fixed frame interval. Color output is gamma corrected and capped at a
conservative global brightness before it reaches the strip.

The code is split into focused modules:

- `src/main.cpp` coordinates input, animation, and output.
- `src/ring_animation.cpp` contains hardware-independent animation math.
- `src/debounced_button.cpp` handles active-high touch input.
- `src/ws2812_strip.cpp` owns PIO setup and WS2812B output.
- `include/alice_lamp/config.hpp` contains hardware and timing defaults.
- `pio/ws2812.pio` implements the 800 kHz LED protocol.

## Tested Hardware

This project was developed with these parts:

- [Pimoroni PGA2350](https://shop.pimoroni.com/products/pga2350?variant=42092629229651):
    RP2350 microcontroller board used for the tested local build
- [Adafruit USB Type-C Vertical Breakout - Downstream Connection](https://www.adafruit.com/product/5993):
    vertical USB-C power input breakout
- [BTF-LIGHTING FCOB WS2812B RGB LED strip](https://www.amazon.com/dp/B0CNXKSWD7):
    5 V, 1 m (3.2 ft), 160 LEDs, 5 mm wide, IP30
- [D-FLIFE TTP223 capacitive touch sensor modules](https://www.amazon.com/dp/B0C6XKXPVX):
    two modules are required, one for power and one for animation cycling

The LED listing does not include a power adapter or controller. Use a regulated
5 V USB-C supply sized for the installed LED count. The Adafruit breakout carries
the power input; connect its ground to the PGA2350 ground and connect only the LED
strip's data input to the configured GPIO pin.

## Downloads

Each published GitHub release includes ready-to-flash UF2 files for:

- Raspberry Pi Pico 2: `AliceLamp-<version>-pico2.uf2`
- Pimoroni Pico Plus 2: `AliceLamp-<version>-pimoroni-pico-plus-2.uf2`
- Pimoroni PGA2350: `AliceLamp-<version>-pimoroni-pga2350.uf2`

Choose the file matching your board, hold its BOOTSEL button while connecting it,
and copy the UF2 onto the mounted `RPI-RP2` drive.

## Building from Source

The project currently targets:

- Raspberry Pi Pico 2 (`pico2`)
- Pimoroni Pico Plus 2 (`pimoroni_pico_plus2_rp2350`)
- Pimoroni PGA2350 (`pimoroni_pga2350`)

Local development uses Raspberry Pi Pico SDK **2.3.1**, CMake 3.13 or newer,
Ninja, and an Arm GNU embedded toolchain. The checked-in VS Code configuration is
compatible with the Raspberry Pi Pico extension and defaults to the PGA2350.

With `PICO_SDK_PATH` configured, build from a terminal with:

```powershell
cmake -S . -B build -G Ninja -DPICO_BOARD=pimoroni_pga2350
cmake --build build
```

The resulting flash image is `build/AliceLamp.uf2`. To target another supported
board, configure a separate build directory with its board identifier.

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

## Contributing and Feedback

Pull requests are welcome, especially for new animations, additional RP2350 boards,
hardware validation, documentation, and focused reliability improvements. Please
keep animation math independent of hardware, preserve smooth wraparound between the
first and final LEDs, and build the firmware without warnings before submitting.

Ideas, build photos, wiring corrections, bug reports, and feedback about brightness
or animation behavior are all useful. Start a
[GitHub issue](https://github.com/Fortinbra/AliceLamp/issues) to share what worked,
what did not, or what the lamp should do next. Proposed larger features can begin as
a design under `docs/features` before implementation.
