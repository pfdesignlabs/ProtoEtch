---
title: ProtoEtch
tags: [protoetch, pcb, etching, hardware, esp32, open-hardware]
version: v0.2
updated: 2025-08-27
license: CERN-OHL-S v2
---

> **License Notice**  
> This work is licensed under the **CERN-OHL-S v2**.  
> You may redistribute and modify this documentation under the terms of the CERN-OHL-S v2, as published by CERN.  
> This documentation is distributed WITHOUT ANY EXPRESS OR IMPLIED WARRANTY,  
> including of MERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A PARTICULAR PURPOSE.  
> Please see the CERN-OHL-S v2 for applicable conditions.

# 🧪 ProtoEtch

ProtoEtch is built to make PCB etching **simple and consistent**.  
It automatically stirs and heats the etching solution, shows real-time status on a display, and lets you monitor or adjust remotely.  
Designed for makers who want repeatable results without constant supervision.

---

## ✨ Features
- Automatic **fluid agitation** with a quiet 24 V BLDC pump (PWM controlled).
- Controlled **heating** of the etchant with a titanium immersion heater.
- **DS18B20** sensor feedback for temperature regulation.
- Local **UI with TFT display + rotary encoder + buttons**.
- **Safety features**: fuses, polyfuses, grounding, TVS & bulk capacitors.
- **OTA updates** and **MQTT integration** for remote control/monitoring.

---

## 🛠 Hardware Overview

### Electronics
- **ESP32-WROOM-32E** (8 MB) as main controller.
- **ST7789V SPI TFT** display (240×320).
- Rotary encoder + push, 2 momentary buttons, 1 toggle switch.
- **Relay module (5 V)** to control 230 V heater.
- **MOSFET (IRLZ44N)** low-side driver for 24 V pump.

### Mechatronics
- **Pump:** 24 V DC BLDC centrifugal, ~30 W, ~800 L/h, mag-drive (quiet).  
- **Heater:** Titanium immersion heater (300–500 W).  
- **Sensor:** DS18B20 waterproof probe.  
- **Tubing:** Silicone (ID 10 mm, OD 14 mm), barbs, clamps, vibration mounts.

### Power & Protection
- **Mean Well LRS-75-24** PSU (24 V / 3.2 A).  
- Buck converters: LM2596 (5 V + 3.3 V).  
- Fusing:  
  - AC: T3.15 A slow-blow (main & heater).  
  - DC pump: T2.0 A slow-blow.  
  - 5 V polyfuse (MF-R110), 3.3 V polyfuse (MF-R050).  
- **TVS P6KE33A** + **3300 µF bulk elco** + **0.1 µF ceramic** near pump.  

---

## 📂 Repository Structure


---

## 📜 Bill of Materials (BOM)

### Quick Purchase Checklist
- [ ] ESP32-WROOM-32E (8MB)  
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
| DS18B20         | 21   | +4.7kΩ pull-up |
| Pump MOSFET     | 25   | 100 Ω + 100 kΩ |
| Heater Relay IN | 26   | Active-LOW |

---

## ⚠️ Safety

- ⚡ **Mains (230 V AC)** present: always fuse the heater line and earth bond the enclosure.  
- ☣ **Chemicals:** Sodium persulfate etchant; corrosive. Use only silicone tubing & titanium heater.  
- 🔥 **Overheating:** DS18B20 regulates heater; consider thermal cutoff.  
- 🔌 **Fuses:** see section above for AC/DC protection.  

---

## 🚀 Roadmap

- [x] BOM finalized (v2.0)  
- [x] Repo structure defined  
- [ ] Schematic → Fusion360 design  
- [ ] PCB layout → prototype board  
- [ ] Firmware base: UI + pump + heater control  
- [ ] OTA + MQTT integration  
- [ ] Enclosure CAD
- [ ] First full assembly & test  

---

## 🤝 Contributing

Contributions welcome!  
- Fork → branch → pull request.  
- Follow [CONTRIBUTING.md](./CONTRIBUTING.md).  
- Please respect CERN-OHL-S v2 for hardware & MIT/Apache for firmware (if dual-licensed later).

---

## 📜 License

- **Hardware**: CERN Open Hardware License v2 – Strongly Reciprocal (CERN-OHL-S v2)  
- **Software** (firmware): MIT (planned; TBD)  
- See [LICENSE](./LICENSE).

---