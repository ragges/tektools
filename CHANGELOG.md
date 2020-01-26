# Changelog

## HEAD - *Changes still not in a named release*
- none yet

---

## v0.1.0 (2020-01-26)

#### Enhancements:
- Enabled as many compiler warnings as possible
- General cleanup of code
- Added printout of version string from git and build time in tektool, tekfwtool and getcaldata
- Support for building on MacOS with NI 488.2 driver
- tektool - added better support for compiling on windows
- updated tdsNvramFloppyTool to v5, with the possibility to write to the EEPROMs.
- moved the extra tdsNvramFloppyTool script to a new directory called tdsNvramFloppyTool-extra

#### Bug Fixes:
- tektool - flash is now reset to read mode after identify
- tektool - does not send password before writing - not needed in bootloader mode, and may be a problem with some firmware versions
- tekfwtool - erasing 28F016SA flash now works (target.c/target.bin)
- tekfwtool - flashing used bad indexing, fixed now (target.c/target.bin)

---

## v0.0.0 (2020-01-01)

Initial release
- Support for building on Linux with linux-gpib, ARM (raspbian)
