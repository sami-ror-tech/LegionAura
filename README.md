# LegionAura

An open-source RGB keyboard lighting controller for Lenovo LOQ, Legion, and IdeaPad Gaming laptops on Linux.

> LegionAura provides full control over the built-in 4-zone RGB ITE keyboard without requiring Lenovo Vantage or Windows. It is lightweight, fast, and designed to work entirely through USB HID control transfers, replicating the behavior of Lenovo’s firmware-level lighting commands.

---

## ✨ Features

* ✅ 4-zone RGB lighting control
* ✅ Static, Breath, Wave, and Hue effects
* ✅ Per-zone custom colors (HEX RRGGBB)
* ✅ Animation speed control (1–4)
* ✅ Brightness control (1–2)
* ✅ Wave direction (LTR / RTL)
* ✅ Brightness-only mode
* ✅ Safe color auto-fill
    * 1 color → applies to all 4 zones
    * 2 colors → Z1, Z2, Z2, Z2
    * 3 colors → Z1, Z2, Z3, Z3
* ✅ Simple CLI with human-friendly commands
* ✅ C++14/libusb backend
* ✅ No Python needed
* GUI (Qt6) support is planned.

---

## 🎯 Why LegionAura Exists

Lenovo does not officially provide Linux support for multi-zone RGB keyboard lighting on LOQ/Legion/IdeaPad gaming laptops.

Most devices expose only raw HID/USB interfaces with undocumented control packets. LegionAura implements a clean, fully-open library and CLI based on reverse-engineering and community research.

The goal is to provide:
* a stable command-line controller
* a reusable C++ library
* a future GUI that mirrors Lenovo Vantage’s lighting controls
* support for multiple Lenovo gaming models

---

## 🖥️ Supported Devices

All laptops using the **ITE 8295 RGB controller** over USB HID are supported.

Default device (yours):
* Vendor: `0x048D`
* Product: `0xC993`

More devices will be added to `devices/devices.json`.

If your laptop uses a different VID/PID, you can edit the C++ source or open an issue.

---

## 🛠️ How It Works

LegionAura communicates with the keyboard’s ITE controller using a single USB `SET_REPORT` control transfer: