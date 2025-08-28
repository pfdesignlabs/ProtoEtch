---
title: Contributing Guide
tags: [protoetch, contributing, open-source]
updated: 2025-08-27
---

# 🤝 Contributing to ProtoEtch

Thank you for your interest in improving **ProtoEtch**!  
This project is open hardware + software. We welcome issues, ideas, and pull requests.

---

## 🛠 How to contribute

- [ ] Fork the repo and create a feature branch (`git checkout -b feature/my-idea`)  
- [ ] Make your changes (docs, hardware, firmware, enclosure)  
- [ ] Follow coding style (Arduino/PlatformIO, C++17; 2 spaces indentation)  
- [ ] Update docs if you change hardware or firmware  
- [ ] Commit using [conventional commits](https://www.conventionalcommits.org/)  
- [ ] Open a Pull Request with a clear description  

---

## 📂 Scope of contributions

- **Firmware**: ESP32 (UI, MQTT, OTA, control loops)  
- **Electronics**: schematics, BOM, KiCad layout, wiring docs  
- **Mechatronics**: pump, heater, sensor mounts, tubing  
- **Enclosure**: CAD, STL, design language  
- **Docs**: assembly guides, safety notes, UI menus  
- **Issues**: bug reports, feature requests, UX suggestions  

---

## 🧪 Testing before PR

- Compile firmware with PlatformIO (ESP32-WROOM-32E).  
- Check that wiring/BOM changes are reflected in `hardware/electronics/bom/`.  
- For mechanical changes: add STL/STEP preview image in `assets/renders/`.  

---

## 📜 License note

By contributing, you agree that your work is released under:  
- **Hardware** → CERN-OHL-S v2  
- **Firmware** → MIT (planned)  