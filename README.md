# Tektronix TDS 5xx/6xx/7xx/8xx tools on Linux, ARM, and MacOS

These are tools for backing up and restoring NVRAM, Flash and EEPROMs
in certain Tektronix oscilloscopes such as the TDS 5xx/6xx/7xx series,
with minor modifications so that they can be used with Linux, and
optionally ARM CPUs e.g. a Raspberry Pi, and MacOS with NI drivers.

(What is special with ARM is that C compilers for efficiency often
defaults to char being unsigned, which some programs do not expect.)

### Changelog

Release notes / change history is in [CHANGELOG.md](CHANGELOG.md)

### Prerequisites

You need some system to run it on, e.g.:
* Linux based:
  * A Linux system
  * linux-gpib - [https://linux-gpib.sourceforge.io](https://linux-gpib.sourceforge.io)
  * A GPIB adapter
* MacOS based:
  * A MacOS system
  * A 488.2 library, e.g. National Instruments 488.2 drivers
  * A GPIB adapter

The programs have been tested for reading (making backups) with these setups:
* Linux based:
  * Raspberry Pi (3B+)
  * Raspbian (version 9.11)
  * linux-gpib (version 4.3.0)
  * USB to GPIB adapter, Agilent 82357B (from Ebay)
* MacOS based: 
  * Mac
  * National Instruments 488.2 drivers
  * USB to GPIB adapter, National Instruments

These programs will likely work with just minor modifications on many
other POSIX compliant systems with an NI-488.2 compliant GPIB API, but
this has not been tested.

#### linux-gpib

linux-gpib can be a little tricky to install and get working. If the
linker has problems finding the gpib libraries when you run the
programs, try running `sudo ldconfig`. The GPIB adapter may need
firmware for booting, check the linux-gpib documentation. The
/dev/gpibN device files may be accessible only by root - if so, try
e.g. `sudo chgrp dialout /dev/gpib*`.

To test the linux-gpib installaton and GPIB connectivity, use ibterm,
for example:
```
/usr/local/bin/ibterm -d N
```
where N is the GPIB address of the instrument.

At the ibterm prompt, type `*IDN?` and check that you get a reasonable
identification response from the instrument:
```
ibterm>*IDN?
TEKTRONIX,TDS 694C,0,CF:91.1CT FV:v6.4e
```

### Installing

To get and compile the programs, clone the git repository, go to the tektools directory and run `make`:
```
git clone https://github.com/ragges/tektools.git
cd tektools
make
```

## Running the programs

### tektool, tekfwtool

These programs read and write the NVRAMs containing user settings,
stored waveforms, and on older instruments calibration data, and the
flash that contains the firmware.

tekfwtool downloads a piece of 68k code to be able to write firmware
to the flash faster, tektool does not.

* tektool supports flash type 28F016SA (there is experimental support
  for 28F160S5 that can be enabled with a `#define` in the program)
* tektfwool supports flash types 28F016SA and 28F160S5

The scope must be started with the NVRAM protection switch set to
unprotected mode (the rocker switch behind the small holes on the
right side of the scope). The scope starts in bootloader mode and
appears almost dead, it does not show anything on the screen and all
LEDs on the front stays lit, but it responds on GPIB, typically on
address 29.

tekfwtool looks for the 68k code in the file "target.bin" in the
current directory. It must either be run when standing in the
directory of the program, or there must be a copy of that file, or a
link to it, in the current working directory.

You could for example dump NVRAM and firmware from the scope using:
```
# NOTE - Addresses and lengths may have to be adjusted depending
# on model
./tektool -r NVRAM_all.bin -b 0x04000000 -l 0x100000
./tektool -r firmware.bin -b 0x01000000 -l 0x400000
```

### getcaldata

getcaldata reads and writes the calibration data in the EEPROMs on the
acquisition board on newer models, typically models ending with B or
higher.

The EEPROM chips may be called e.g. U1052 and U1055, or U1055 and
U1056. This program calls them U1052 and U1055 and ignores what is
printed on the board.

The scope should be booted normally.

The program assumes the GPIB address of the scope is 1, this can
be changed in the program.

Just run it and it will dump the EEPROMs.

You may want to double check that the addresses and sizes of the
NVRAMs are correct for your model.

### tdsNvramFloppyTool and TDSNvrCV_2_0

**tdsNvramFloppyTool** is a set of scripts that are to be put on a floppy
disk that will let the scope itself read NVRAM and EEPROM
data, or write NVRAM data, to/from the floppy disks - no GPIB is needed.

Addresses and sizes may have to be adjusted depending on model.

In this kit there is also an extra version,
tdsNvramEepromFloppyDumper, that dumps both the NVRAM and the EEPROMs
to the floppy in one sweep.

To use the tdsNvramFloppyTool, format a floppy (preferably in the
scope), copy the file(s) that do what you want to the floppy, and boot
the scope with the floppy inserted.

**TDSNvrCV_2_0** is a tool for checksumming NVRAM and EEPROM dumps,
written in Java.

Note that for checking EEPROM dumps taken with the getcaldata tool,
you need to concatenate the two 256 byte files into one 512 byte
file, and run the check on the new combined file:
```
cat U1052.bin U1055.bin > EEPROM_combined.bin
java -cp TDSNvrCV_2_0.zip TDSNvramChecksumVerifier EEPROM_combined.bin
```

For more information about using these scripts and the checksumming
tool, see the
[thread on eevblog](https://www.eevblog.com/forum/testgear/tektronix-tds500600700-nvram-floppy-dump-tool/)
(or the file README.txt), and the info.txt and info-2.txt files in the
directory.

There is nothing OS specific about these, but they are very nice
tools, so they are included in this kit anyway.

## Hint

You can use tektool, tekfwtool and getcaldata to get the data using
GPIB, and tdsNvramFloppyTool to get it using a floppy, and compare the
results to check that you have likely got correct and error free
data. Note that the first two bytes of the NVRAM changes between
almost every read, and much more of it as soon as the scope is being
used.

You can also use the NVRAM and EEPROM checksumming tool TDSNvrCV_2_0
to check your NVRAM dumps.

If you use the floppy method first, and then immediately flip the
NVRAM protection switch and reboot it for GPIB dumping using
tektool/tekfwtool, only the first few bytes of the NVRAM, the date and
time, should differ.

## Built With

* Linux
  * [linux-gpib](https://linux-gpib.sourceforge.io) - The GPIB driver and libraries
* MacOS
  * [National Instruments 488.2 drivers](http://www.ni.com/sv-se/support/downloads/drivers.html)

## Links to sources

* [tektool](https://forum.tek.com/download/file.php?id=24983&sid=de2267bdadfd0a11ce92f2d5648d656e) - tektool in a zip file on forum.tek.com
* [tekfwtool](https://github.com/fenugrec/tekfwtool) - tekfwtool on github
* [tekfwtool](https://stackframe.org/tekfwtool/) - tekfwtool on stackframe.org
* [tdsNvramFloppyTool](https://www.eevblog.com/forum/testgear/tektronix-tds500600700-nvram-floppy-dump-tool/) - information about tdsNvramFloppyTool
* [getcaldata.c](https://drive.google.com/file/d/0Bz230ThydfRGYWFZbE5kWWhnVkk/view) - getcaldata.c

## DIST directories

In the program directories there are subdirectories called DIST that
contain the original programs and in some cases other stuff that came
with it.

## Acknowledgments

* All the helpful people in the community that has made this possible
* flyte at eevblog.com forum
* Sven Schnelle (svens@stackframe.org>)
* Dr. Albert Roseiro at Tantratron
