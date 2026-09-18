# AliceLamp Project Guidelines

## C++ Style

- Use C++17, fixed-width integer types, brace initialization, `const`, and
  `[[nodiscard]]` where they make ownership and intent clearer.
- Put project code in the `alice_lamp` namespace and use `snake_case` for
  functions, variables, and constants. Use `PascalCase` for types.
- Keep headers self-contained with `#pragma once`; include what each file uses.
- Avoid dynamic allocation, exceptions, RTTI, blocking delays, and unbounded work
  because this firmware runs continuously on a microcontroller.
- Prefer small cohesive classes and functions. Keep hardware access in drivers,
  animation math independent of hardware, and `main()` focused on orchestration.
- Do not add comments that merely repeat the code. Document hardware constraints,
  timing assumptions, and decisions that are otherwise non-obvious.

## Project Conventions

- Store public headers under `include/alice_lamp`, implementations under `src`,
  and PIO programs under `pio`.
- Keep user-adjustable hardware and timing values in
  `include/alice_lamp/config.hpp` or expose them as CMake cache settings.
- Preserve the WS2812 logical-ring behavior: animations must transition smoothly
  between the final and first LED.
- Treat `VERSION` as the single version source and keep it in `MAJOR.MINOR`
  format. Increment MINOR for backward-compatible features and animations;
  increment MAJOR for incompatible hardware, configuration, or behavior changes.
- Tag releases as `vMAJOR.MINOR`, exactly matching `VERSION`.
- Keep changes focused and avoid modifying generated files under `build`.

## Validation

- Build every firmware change with the `Compile Project` task (Ninja in `build`).
- Treat compiler warnings as defects in changed code and report when hardware-only
  behavior cannot be validated locally.