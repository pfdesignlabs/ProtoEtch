---
title: ProtoEtch
tags: [protoetch, pcb, etching, hardware, esp32, open-hardware]
version: v0.2
updated: 2025-08-28
license: CERN-OHL-S v2
---

> **License Notice**  
> This work is licensed under the **CERN-OHL-S v2**.  
> You may redistribute and modify this documentation under the terms of the CERN-OHL-S v2, as published by CERN.  
> This documentation is distributed WITHOUT ANY EXPRESS OR IMPLIED WARRANTY,  
> including of MERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A PARTICULAR PURPOSE.  
> Please see the CERN-OHL-S v2 for applicable conditions.

# 🧪 ProtoEtch

ProtoEtch is an open-hardware **PCB etching station** that makes etching **simple, repeatable, and safe**.  
It automatically agitates and heats the etchant, uses a DS18B20 sensor for feedback, and shows live data on a local display. Remote monitoring and control (MQTT, OTA updates) are planned.  
Designed for makers who want **consistent results** without babysitting their etching tank.

---

## ✨ Features
- Automatic **fluid agitation** with a quiet 24 V BLDC pump (PWM controlled). *(planned)*
- Controlled **heating** of the etchant with a titanium immersion heater. ✅  
- **DS18B20** sensor feedback for temperature regulation. ✅  
- Local **UI with TFT display + rotary encoder + buttons**. *(planned)*  
- **Safety features**: fuses, grounding, TVS, bulk caps, watchdog cut-offs. *(partially implemented)*  
- **OTA updates** and **MQTT integration** for remote control/monitoring. *(planned)*  
- **Open hardware**: CERN-OHL-S v2 for electronics & mechanics, software under permissive license.  

---

## 🛠 Hardware Overview

### Electronics
- **ESP32-WROOM-32E (8 MB)** main controller.  
- **ST7789V SPI TFT** (240×320).  
- Rotary encoder + push, 2 momentary buttons, 1 toggle switch.  
- **Relay module (5 V)** to switch 230 V heater.  
- **IRLZ44N MOSFET** low-side driver for 24 V pump.  

### Mechatronics
- **Pump**: 24 V DC BLDC centrifugal, ~30 W, ~800 L/h, mag-drive.  
- **Heater**: Titanium immersion heater (300–500 W).  
- **Sensor**: DS18B20 waterproof probe.  
- **Tubing**: Silicone ID10/OD14 with barbs + clamps, vibration mounts.  

### Power & Protection
- **Mean Well LRS-75-24** PSU (24 V / 3.2 A).  
- Buck converters: LM2596 (5 V & 3.3 V).  
- Fuses:  
  - AC: T3.15 A slow-blow (main & heater).  
  - DC pump: T2.0 A slow-blow.  
  - 5 V polyfuse (MF-R110), 3.3 V polyfuse (MF-R050).  
- **TVS P6KE33A** + **3300 µF bulk elco** + **0.1 µF ceramic** near pump.  

---

## 📜 Bill of Materials (BOM)

### Quick Purchase Checklist
- [ ] ESP32-WROOM-32E (8 MB)  
- [ ] TFT ST7789V SPI (240×320)  
- [ ] DS18B20 probe (waterproof)  
- [ ] 24 V BLDC centrifugal pump (~30 W, ~800 L/h)  
- [ ] IRLZ44N MOSFET + resistors + TVS + elco  
- [ ] Titanium heater (300–500 W)  
- [ ] Relay module 5 V (heater)  
- [ ] Mean Well LRS-75-24 PSU  
- [ ] LM2596 buck modules (5 V & 3.3 V)  
- [ ] GX16 connectors (pump, heater, sensor)  
- [ ] Silicone tubing ID10/OD14 + barbs + clamps  
- [ ] Assorted fuses + fuse holders  

### Detailed BOM
👉 See [`hardware/electronics/bom/ProtoEtch_BOM_v2.0.md`](./hardware/electronics/bom/ProtoEtch_BOM_v2.0.md)

---

## 📡 ESP32 Pin Mapping

| Function        | GPIO | Notes |
|-----------------|------|-------|
| TFT SCK         | 18   | SPI |
| TFT MOSI        | 23   | SPI |
| TFT CS          | 5    | SPI |
| TFT DC          | 16   | SPI |
| TFT RST         | 17   | SPI |
| Rotary A        | 32   | Input |
| Rotary B        | 33   | Input |
| Rotary SW       | 22   | Input_pullup |
| Button1         | 14   | Input_pullup |
| Button2         | 27   | Input_pullup |
| DS18B20         | 21   | +4.7 kΩ pull-up |
| Pump MOSFET     | 25   | 100 Ω gate + 100 kΩ pull-down |
| Heater Relay IN | 26   | Active-LOW |

---

## ⚠️ Safety

- ⚡ **Mains (230 V AC)**: always fuse the heater line and earth bond the enclosure.  
- ☣ **Chemicals**: Sodium persulfate etchant is corrosive. Use only silicone tubing & titanium heater.  
- 🔥 **Overheating**: DS18B20 regulates heater; consider thermal cutoff.  
- 🔌 **Fuses**: see Power & Protection section.  

---

## 🚀 Roadmap

- [x] Repo structure defined  
- [x] BOM finalized (v2.0)  
- [x] DS18B20 driver (non-blocking, CRC, calibration, stats)  
- [x] Heater HAL + Controller:
  - Relay switching
  - Bang-bang control with hysteresis
  - Min on/off hold
  - NVS persistence
  - Serial CLI for tuning
- [ ] Pump control module + MOSFET HAL  
- [ ] Wi-Fi provisioning (captive portal)  
- [ ] MQTT integration  
- [ ] OTA updates  
- [ ] UI firmware (TFT + encoder + buttons)  
- [ ] PCB layout → prototype board  
- [ ] Enclosure CAD  
- [ ] Full system assembly & test  

---

## 🤝 Contributing

Contributions welcome!  
- Fork → branch → pull request.  
- Follow [CONTRIBUTING.md](./CONTRIBUTING.md).  
- Please respect CERN-OHL-S v2 for hardware & MIT/Apache for firmware (to be dual-licensed later).

---

## 📜 License

- **Hardware**: CERN Open Hardware License v2 – Strongly Reciprocal (CERN-OHL-S v2)  
- **Firmware**: MIT (planned; TBD)  
- See [LICENSE](./LICENSE).