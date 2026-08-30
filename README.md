# USDOT system

## System overview, protocol, and method

This repository contains the current prototype for an ultrasound-guided diffuse optical tomography (USDOT) acquisition system. It keeps the hardware capture path, bridge firmware, and operator interface together so that detector data can be traced from the ADC to the display.

The data path is:

```text
LTC2326 ADC -> Artix-7 FPGA -> SPI -> ESP32-S3 -> UDP/Wi-Fi -> Qt application
```

The Vivado design targets the Artix-7 XC7A35T on a Cmod A7-35T. It drives the LTC2326 conversion and serial clocks, collects signed 16-bit samples, buffers data in 15 channel FIFOs, and exposes the FIFOs through an SPI slave. The ESP32-S3 runs as an open Wi-Fi access point named `USDOT`, listens for UDP packets on `192.168.4.1:12345`, forwards each packet over 2 MHz SPI, and returns the full-duplex SPI response to the sender. The desktop application checks the connection, polls any selected detector channels, and plots the returned samples.

The implemented wire protocol is small and fixed:

| Request | Meaning |
| --- | --- |
| `R` padded to 4 bytes | Connection check. The FPGA echoes the request through SPI. |
| `0x01` through `0x0F` | Read detector FIFO 1 through 15. |
| `0x11` through `0x1F` | Reset detector FIFO 1 through 15. |
| 1,024-byte monitor packet | Reset the selected FIFO with the first byte, clock out its data with the remaining channel bytes, and return 1,024 bytes over UDP. |

The Qt monitor accepts only 1,024-byte replies. It skips the first 10 framing bytes, decodes the rest as big-endian signed 16-bit samples, and converts each value to volts with `raw * 10.24 / 32768`. The native desktop build uses the real UDP link. The WebAssembly build cannot open the Qt UDP socket, so it simulates the connection and generates repeatable waveforms for all 15 channels. Data-acquisition, source-control, gain-control, and native ultrasound-capture functions are still disabled in the interface.

The checked-in `demo/` directory is the deployable WebAssembly website, available as the [interactive USDOT demo](https://yiyangruan2004.github.io/USDOT-demo/). Hosts that support custom headers can use `demo/_headers`. On GitHub Pages, `demo/coi-serviceworker.js` supplies the same cross-origin isolation policy in the browser so the multithreaded Qt runtime can start.

## Repository structure

```text
USDOT/
|-- ESP32/                     ESP-IDF firmware configured for PlatformIO
|   |-- include/               UDP and SPI interfaces
|   |-- src/                   Wi-Fi, UDP-to-SPI bridge, and application entry point
|   |-- CMakeLists.txt         ESP-IDF project definition
|   |-- platformio.ini         Freenove ESP32-S3 WROOM build environment
|   `-- sdkconfig_*            Board-specific ESP-IDF configuration
|-- FPGA/                      Vivado 2025.2 project for the Cmod A7-35T
|   |-- FPGA.xpr               Vivado project entry point
|   `-- FPGA.srcs/
|       |-- constrs_1/         Cmod A7 pin and clock constraints
|       |-- sim_1/             Verilog testbench
|       |-- sources_1/new/     ADC, FIFO-control, SPI, and top-level RTL
|       `-- sources_1/bd/      Block design and generated IP configuration
|-- UI/                        Shared Qt Widgets desktop and WebAssembly application
|   |-- main.cpp               Application entry point
|   |-- mainwindow.*           Dashboard, channel selection, and chart rendering
|   |-- ctrl.*                 Native connection checks or browser simulation
|   |-- monitor.*              Native channel polling or generated demo waveforms
|   |-- logger.*               Qt message logging
|   `-- CMakeLists.txt         Qt build definition
|-- docs/                      Local research and project references; ignored by Git
|-- .gitignore                 Generated-file and local-document exclusions
`-- README.md                  System method, protocol, and repository map
```
