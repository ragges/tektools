# Changelog

## HEAD - *Changes still not in a named release*

#### Enhancements:
- Enabled as many compiler warnings as possible
- General cleanup of code
- Added printout of version string from git and build time in tektool, tekfwtool and getcaldata
- Support for building on MacOS with NI 488.2 driver
- tektool - added better support for compiling on windows

#### Bug Fixes:
- tektool - flash is now reset to read mode after identify
- tektool - does not send password before writing - not needed in bootloader mode, and may be a problem with some firmware versions
- tekfwtool - erasing 28F016SA flash now works (target.c/target.bin)
- tekfwtool - flashing used bad indexing, fixed now (target.c/target.bin)

---

## v0.0.0 (2020-01-01)

Initial release
- Support for building on Linux with linux-gpib, ARM (raspbian)
