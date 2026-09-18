# Wi-Fi Time Synchronization

**Status:** Proposed

## Goal

Make the pocket-watch animation display the correct local time instead of deriving
its hand positions from the animation phase. The lamp should synchronize UTC from
an NTP server after connecting to Wi-Fi, apply the configured time zone and daylight
saving rules, then keep time locally between updates.

## Board Support

The initial release targets do not include onboard Wi-Fi:

| Current target | Proposed Wi-Fi path |
| --- | --- |
| Raspberry Pi Pico 2 | Add a Raspberry Pi Pico 2 W build |
| Pimoroni Pico Plus 2 | Add a Pimoroni Pico Plus 2 W build |
| Pimoroni PGA2350 | Use an external supported Wi-Fi module or remain offline |

The existing non-Wi-Fi UF2 files must remain available. Wi-Fi-specific code should
only be linked for compatible targets so the base firmware remains portable.

## Proposed Design

1. Introduce a `ClockTime` value containing hour, minute, second, and validity.
2. Change the clock animation to consume `ClockTime`; keep animation phase only
   for decorative pulses.
3. Add a time service that maintains UTC from the monotonic Pico clock and
   periodically corrects it from NTP.
4. Add a CYW43-backed network adapter for Pico 2 W and Pico Plus 2 W builds.
5. Add one configuration service for Wi-Fi credentials, time zone, daylight saving,
   NTP servers, and the sync interval.
6. Store the selected time-zone rule separately from UTC and apply it only when
   producing local display time.
7. Keep the configuration service available over HTTP on the local network after
   Wi-Fi connects.
8. Show a recognizable searching animation until the first successful sync, then
   transition to the clock without blocking LED updates or touch handling.

Network and timekeeping code should remain independent of `RingAnimation`. The
animation receives time data and should not perform network operations itself.

## Provisioning and Security

Pre-built UF2 files cannot contain a user's network name or password. On first boot,
or after a failed connection, the lamp should expose a temporary setup access point.
Its local configuration page collects Wi-Fi credentials and time settings together:

1. Network name and password
2. Time-zone region
3. Automatic daylight-saving behavior
4. Optional custom NTP servers and synchronization interval

Prefer recognizable region names such as `America/Chicago` in the interface. The
firmware can map supported regions to compact POSIX time-zone rules rather than
embedding a complete time-zone database. A custom POSIX rule should remain available
for regions not included in the presets.

Credentials and time settings should be stored in a versioned flash record with an
integrity check. Passwords must be masked in the interface, omitted from responses,
and never logged over USB serial.

A compile-time credentials file may be useful during development, but it must be
gitignored and must not be used by release builds.

## Local Configuration Portal

After joining Wi-Fi and receiving a DHCP lease, write a concise message to the USB
serial log, for example:

```text
Wi-Fi connected; configuration: http://192.168.1.42/
```

The HTTP configuration portal should continue listening at that address so a user
on the same local network can change Wi-Fi and time settings later. Advertising an
`alice-lamp.local` mDNS name is desirable, but the numeric IP address must still be
logged because mDNS is not available on every client or network.

Saving time-zone or daylight-saving changes should update the displayed time without
rebooting. When Wi-Fi credentials change, retain the previous settings until the new
network connects successfully. If validation fails, restore the previous connection;
if neither configuration works, return to setup-access-point mode.

Configuration changes should require either a setup PIN or recent physical approval
using a touch control. The portal should only be reachable from the local network and
must not expose the stored Wi-Fi password.

## Reliability

- Request time from multiple configurable NTP servers over UDP port 123.
- Use UTC internally and support an explicit POSIX time-zone rule for daylight
  saving changes.
- Recalculate the local UTC offset when a daylight-saving transition occurs.
- Retry with bounded exponential backoff while animations continue running.
- Resynchronize periodically to correct oscillator drift.
- Keep displaying locally maintained time during temporary network outages.
- Mark time invalid after boot until NTP succeeds; do not present fabricated time
  as accurate.
- Avoid frequent flash writes solely to preserve the last synchronized timestamp.

## Configuration

Default to `America/Chicago` with automatic daylight-saving transitions, representing
US Central Time. Users can change both values during initial Wi-Fi setup or later
through the local configuration portal.

The implementation should eventually expose settings equivalent to:

| Setting | Example | Purpose |
| --- | --- | --- |
| Network name | `Wonderland` | Select the local Wi-Fi network |
| Network password | Hidden | Authenticate without exposing the value later |
| NTP servers | `pool.ntp.org` | Sources of UTC time |
| Time-zone region | `America/Chicago` | Select local offset and DST behavior |
| Automatic DST | `true` | Apply seasonal transitions for the selected region |
| Custom time-zone rule | `CST6CDT,M3.2.0,M11.1.0` | Override region presets |
| Sync interval | 6 hours | Limit traffic while correcting drift |
| Setup PIN | User selected | Protect later configuration changes |

Runtime provisioning is preferred for SSID, password, and time zone so one release
image can be used by different households.

## Release Changes

Add these targets without removing the current three:

- `pico2_w`
- `pimoroni_pico_plus2_w_rp2350`

Wi-Fi UF2 filenames should follow the existing versioned release convention. The
PGA2350 should continue to use the current decorative clock unless an external
network adapter is explicitly configured.

## Acceptance Criteria

- A newly provisioned Wi-Fi board obtains valid time without firmware recompilation.
- Initial setup collects Wi-Fi, time-zone, and daylight-saving settings together.
- Clock hand positions represent local hour, minute, and second values.
- The clock changes offset at configured daylight-saving boundaries.
- Animation and touch input remain responsive during connection and NTP retries.
- Loss of Wi-Fi does not stop the clock after an initial synchronization.
- The USB serial log reports the assigned IP address without revealing credentials.
- Visiting the logged address allows authorized changes to Wi-Fi and time settings.
- Invalid replacement credentials preserve the prior working configuration or return
   the lamp to setup-access-point mode.
- Credentials are absent from source control, logs, and release binaries by default.
- Non-Wi-Fi board builds continue to compile and retain all current animations.
