# LegionAura

<p align="center">
  <img src="https://raw.githubusercontent.com/nivedck/LegionAura/main/assets/logo.png" alt="LegionAura Logo" width="200"/>
</p>

<p align="center">
  <strong>An open-source RGB keyboard lighting controller for Lenovo LOQ, Legion, and IdeaPad Gaming laptops on Linux.</strong>
</p>

<p align="center">
  <a href="https://github.com/nivedck/LegionAura/blob/main/LICENSE"><img src="https://img.shields.io/github/license/nivedck/LegionAura" alt="License"></a>
  <a href="https://github.com/nivedck/LegionAura/issues"><img src="https://img.shields.io/github/issues/nivedck/LegionAura" alt="Issues"></a>
  <a href="https://github.com/nivedck/LegionAura/stargazers"><img src="https://img.shields.io/github/stars/nivedck/LegionAura" alt="Stars"></a>
</p>

> LegionAura provides full control over the built-in 4-zone RGB ITE keyboard without requiring Lenovo Vantage or Windows. It is lightweight, fast, and designed to work entirely through USB HID control transfers, replicating the behavior of Lenovo’s firmware-level lighting commands.

---

## 📋 Table of Contents

- [✨ Features](#-features)
- [🎯 Why LegionAura Exists](#-why-legionaura-exists)
- [🖥️ Supported Devices](#️-supported-devices)
- [🛠️ How It Works](#️-how-it-works)
- [🚀 Getting Started](#-getting-started)
  - [Installation](#installation)
  - [Building from Source](#building-from-source)
- [💡 Usage](#-usage)
- [🤝 Contributing](#-contributing)
- [📜 License](#-license)

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
* ✅ C++17/libusb backend
* ✅ GUI (Qt6) support is planned.

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

Default device (Lenovo LOQ 2024):
* Vendor: `0x048D`
* Product: `0xC993`

More devices will be added to `devices/devices.json`.

If your laptop uses a different VID/PID, you can edit the C++ source or open an issue.

---

## 🛠️ How It Works

LegionAura communicates with the keyboard’s ITE controller using a single USB `SET_REPORT` control transfer.

---

## 🚀 Getting Started

### Installation

You can install LegionAura by cloning the repository and building it from source.

### Building from Source

**Prerequisites:**
* A C++17 compatible compiler (e.g., GCC, Clang)
* CMake (version 3.16 or later)
* libusb (version 1.0 or later)

**Build steps:**

```bash
git clone https://github.com/nivedck/LegionAura.git
cd LegionAura
mkdir build
cd build
cmake ..
make
```

This will create the `legionaura` executable in the `build/cli` directory.

---

## 💡 Usage

```
Usage:
  legionaura static <colors...> [--brightness 1|2]
  legionaura breath <colors...> [--speed 1..4] [--brightness 1|2]
  legionaura wave <ltr|rtl> [--speed 1..4] [--brightness 1|2]
  legionaura hue [--speed 1..4] [--brightness 1|2]
  legionaura off
  legionaura --brightness 1|2    (brightness only)
```

**Examples:**

* Set a static color for all zones:
  ```bash
  ./build/cli/legionaura static ff0000
  ```

* Set a breathing effect with custom colors:
  ```bash
  ./build/cli/legionaura breath ff0000 00ff00 0000ff
  ```

* Set a wave effect from left to right:
  ```bash
  ./build/cli/legionaura wave ltr --speed 2
  ```

---

## 🤝 Contributing

Contributions are welcome! If you have a feature request, bug report, or want to contribute to the code, please open an issue or a pull request.

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
