---
title: ProtoEtch – BOM
version: v2.0
status: draft
updated: 2025-08-27
tags: [protoetch, bom, hardware, electronics, mechatronics]
---

# ProtoEtch — Bill of Materials (v2.0)

> Scope: full build (electronics, mechatronics, enclosure wiring).  
> License: hardware docs under **CERN-OHL-S v2**.

## Quick Purchase Checklist
- [ ] ESP32-WROOM-32E (8MB)
- [ ] ST7789V SPI TFT 2.4–2.8"
- [ ] DS18B20 waterproof probe
- [ ] 24 V BLDC pump (≈30 W, ≈800 L/h)
- [ ] IRLZ44N MOSFET + 100 Ω + 100 kΩ
- [ ] P6KE33A TVS + 3300 µF/35 V + 0.1 µF
- [ ] 5×20 mm fuses (T3.15A AC, T2A DC) + holders
- [ ] 24 V PSU (LRS-75-24)
- [ ] 5 V & 3.3 V regulators (LM2596 or equiv.)
- [ ] Relay module 5 V (heater)
- [ ] Silicone tube ID10/OD14 + barbs + valve + clamps
- [ ] GX16 panel connectors (heater/pump/sensor)
- [ ] Wiring: AWG20 (power), AWG24 (signal)

---

## Electronics & UI
| Qty | Item                      | Example / Part            | Notes                |
| --: | ------------------------- | ------------------------- | -------------------- |
|   1 | ESP32-WROOM-32E (8MB)     | ESP32-WROOM-32E-N8        | Main MCU             |
|   1 | TFT ST7789V SPI (240×320) | 2.4–2.8" ST7789V          | No touch             |
|   1 | Rotary encoder w/ push    | Bourns PEC11R-4215F-S0024 | 24 detents           |
|   1 | Rotary knob               | Ø28×16 mm (3D/alu)        | D-bore 6 mm          |
|   2 | Momentary push 12 mm      | non-lit, red              | Start/Func           |
|   1 | Main DC toggle            | SPST                      | 24 V master          |
|   1 | RESET tactile             | 6×6 mm                    | ESP32 EN             |
|   1 | BOOT tactile              | 6×6 mm                    | ESP32 IO0            |
|   1 | Prog header               | 1×6, 2.54 mm RA           | GND/3V3/TX/RX/EN/IO0 |

## Sensors
| Qty | Item | Example / Part | Notes |
|---:|---|---|---|
| 1 | DS18B20 probe | 1 m waterproof | Stainless tip |
| 1 | Pull-up | 4.7 kΩ ¼ W | DS18B20 data→3V3 |

## Pump (24 V DC) & Drive
| Qty | Item | Example / Part | Notes |
|---:|---|---|---|
| 1 | BLDC centrifugal pump 24 V | P45042D05 (~30 W, ~800 L/h) | Quiet, wet-rotor |
| 1 | MOSFET (THT) | **IRLZ44N** TO-220 | Low-side PWM |
| 1 | Gate R | **100 Ω** ¼ W | ESP32→gate |
| 1 | Gate pulldown | **100 kΩ** ¼ W | Gate→GND |
| 1 | TVS 24 V (THT) | **P6KE33A** (bi-dir) | At pump conn. |
| 1 | Bulk elco | **3300 µF/35 V**, 105 °C, low-ESR | Near pump conn. |
| 1 | Decoupling | **0.1 µF** (100 nF ≥50 V) | Parallel to elco |
| 1 | PCB fuse holder | 5×20 mm (THT) | For pump fuse |
| 2–5 | Fuse slow-blow | **T2.0 A** (keep **T2.5 A** spare) | +24 V series |

## Heater (230 VAC)
| Qty | Item | Example / Part | Notes |
|---:|---|---|---|
| 1 | Titanium heater | 300–500 W | No thermostat |
| 1 | Relay module 5 V | 1-ch, opto/transistor | Active-LOW IN |
| 1 | IEC-C14 inlet w/ switch+fuse | Fused inlet | AC entry |
| 1 | Panel fuse holder | 5×20 mm | Heater line (opt.) |
| 5–10 | Fuse slow-blow | **T3.15 A** | AC main & heater |
| 1 | Earth lug + ring | M3 | PE bonding |

## Power
| Qty | Item | Example / Part | Notes |
|---:|---|---|---|
| 1 | 24 V PSU | **Mean Well LRS-75-24** (3.2 A) | Compact |
| 1 | 5 V buck | LM2596 module | Relay + misc |
| 1 | 3.3 V reg/buck | LM2596 @3.3 V *or* LDO | ESP32/TFT |
| 1 | Polyfuse 5 V | **MF-R110** | 1.10 A hold |
| 1 | Polyfuse 3.3 V | **MF-R050** | 0.50 A hold |

## Connectors & Cabling
| Qty | Item | Example / Part | Notes |
|---:|---|---|---|
| 1 | GX16-3 (panel+cable) | Heater L/N/PE | AC out |
| 2 | GX16-4 (panel+cable) | Pump / Sensor | Or GX16-3 for DS18B20 |
| – | JST-VH 3.96 mm 2-pin + crimps | SVH-21T-P1.1 | Pump/DC power |
| – | JST-XH 2.54 mm (2–6p) + crimps | – | UI/sensors |
| – | Dupont 2.54 mm (M/F) + crimps | – | Dev leads |
| 2 | Bulkhead hose barb | PP/PVDF, **ID 10 mm** | In/return |
| 1 set | T-piece + ball valve | 10 mm | Bypass trim |
| 3–4 m | Silicone tube | **ID10 / OD14 mm** | −60…+200 °C |
| 6–10 | Hose clamps | Plastic / SS (dry) | Secure tubing |
| 4 | Rubber vibration mounts | M3 | Pump isolation |
| – | Cable glands | PG7/PG9 | Through-panel |

## Passives / Misc (board-level)
| Qty | Item | Example / Part | Notes |
|---:|---|---|---|
| 10–20 | 0.1 µF ceramic (X7R) | 50 V | Decoupling |
| Assort. | 10–220 µF electrolytics | 50 V | 5 V/3V3 rails |
| 5 | 10 kΩ ¼ W | – | Pull-ups (EN/IO0) |
| 2 | Ferrite bead (opt.) | – | 5 V/3.3 V filters |
| – | Heat-inserts M3 + screws | – | Enclosure |
| 4 | Rubber feet | – | Base |

## Wiring (recommendations)
| Qty | Item | Spec | Notes |
|---:|---|---|---|
| 5 m | Silicone wire | **AWG20** red/black | 24 V pump/DC distro |
| 5 m | Silicone wire | **AWG24** 6-color set | Signals/UI |
| 2 m | Mains wire | 3×0.75 mm² | AC inlet → relay |

---
