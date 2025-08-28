---
title: Changelog
tags: [protoetch, changelog, versioning]
updated: 2025-08-28
---

# 📓 ProtoEtch Changelog

[0.1.0] – 2025-08-28
commit 

Added
	•	DS18B20 driver module (sensor_ds18b20.h/.cpp)
	•	Non-blocking state machine (geen delay()).
	•	Event-driven interface met subscribe() voor callbacks.
	•	Caching via latest() voor snelle, non-blocking uitlezing.
	•	Exponential moving average (EMA) smoothing instelbaar.
	•	Error & stats reporting:
	•	Timeout, CRC fail, out-of-range detectie.
	•	lastError() en stats() API toegevoegd.
	•	CRC-check op scratchpad toegevoegd voor betrouwbare metingen.
	•	healthy() helper voor fail-safe gebruik in andere modules.
	•	Force sample (forceSample()) om direct nieuwe conversie te starten.
	•	Exponential backoff bij herhaalde fouten.
	•	Kalibratie-API:
	•	setCalibration(gain, offset)
	•	Persistentie via NVS: loadCalibration() en saveCalibration()

Changed
	•	config.h: logging macros toegevoegd (LOGI/LOGW/LOGE) voor lichtgewicht debug output.
	•	Verbeterde foutafhandeling bij afwezige of niet-bereikbare sensor.

Removed
	•	Blocking wachttijd voor conversies.
	•	Ruwe, onbeschermde uitlezing zonder CRC-checks.