
From:
https://www.eevblog.com/forum/testgear/tektronix-tds500600700-nvram-floppy-dump-tool/

Tektronix TDS500/600/700 NVRAM floppy backup and restore tool

#####

Tektronix TDS500/600/700 NVRAM floppy backup and restore tool
« on: January 17, 2019, 05:48:31 pm »

Hi all,

As many of these great Tektronix TDS scopes are ageing, they will have their NVRAMs sooner or later wiped due to battery failure and because AFAIK the only way to safely dump the NVRAMs contents is via a GPIB interface, which not every scope owner has, I've decided to write a small script which simply dumps the NVRAM contents to a floppy disk. As most TDS scopes, and certainly the higher spec'd ones, have a floppy disk option, taking a backup this way would be a quick win for anyone looking to do so or worrying about imminent battery doom.

The script is based on the JRE installer script and reads 0xA0000 bytes from the base address 0x4000000 (contiguous 128K DS1486 and 512K DS1250Y), so it should work with the TDS700C/D and TDS600C series. It has been tested on TDS754D, TDS784D and TDS694C. Adapting it to TDS series with different NVRAM addresses should be easy.

Simply put the attachment's contents into an (old) FAT formatted floppy and start up the scope with the disk mounted. About 10s after boot, it will dump the NVRAMs to dump.bin on the floppy. Use an error-free floppy and preferably first format it on the scope itself using the normal file utilities for best compatibility.

Make sure you rename the dump.bin right away to include the scope model and serial number, as there is no way to derive that from the binary dump afterwards.

Have fun saving your equipment!   :)

flyte

-- EDIT/UPDATE:

If we can read this way, we can also write!   ;)

So I've made a second script which will load an NVRAM dump back into the scope via floppy. Works the same way as the dumper, but expects a file writedmp.bin on the disk. Tested on a TDS754D scope, works perfectly. Note that loading a dump from a different scope will mess up all calibration values, so please be honest when offering scopes "repaired" this way and mention it to your buyer.

BTW, if you are looking for a factory new genuine Dallas DS1486, I have some left from known distributor origins. It's likely to be one of the very last batches, as the production stopped years ago and worldwide stocks are now depleted, and what now remains are Ebay floods of Asian counterfeits with all sorts of issues.

-- EDIT/UPDATE 2:

Made a new set of scripts v3, the previous download's nvram restore/write script did not write correctly in some cases which lead to a corrupt nvram, even if the file was fine, as it appeared. Also, the backup/dump script seemed to hang on A-series scopes, a TDS524A in the case tested. Please delete your old downloads, replace them with the new scripts and check info.txt in the archive.

-- EDIT/UPDATE 3:

Added a checksum verification tool so everyone can verify the dump taken has valid checksums on critical calibration data. Even though it contains checksum locations for a range of firmware versions and models, you may encounter an unrecognized NVRAM firmware prototype. Also, there may be locations in the NVRAM with other checksums than what is supported by the tool, but those verified by it are the critical ones regarding calibration and proper startup. It's written in Java, you need to install a Java JRE on your computer in order to use it. Run command example:

Code: [Select]
java -cp TDSNvrCV_1_0.zip TDSNvramChecksumVerifier DUMP.BIN

-- EDIT/UPDATE 4:

Added scripts to backup the factory calibration constants in the EEPROMs on acquisition boards of -B, -C and -D series scopes, starting with firmware v4.x. Check info.txt inside the archive. The checksum verifier has been updated to allow verification of the acquisition EEPROM dumps, and the tool will attempt to detect based on file size whether the dump is an NVRAM or acquisition EEPROM.

Code: [Select]
java -cp TDSNvrCV_2_0.zip TDSNvramChecksumVerifier DUMP.BIN

-- EDIT/UPDATE 5:

New version of the scripts which allow you to also write the EEPROMs on acquisition boards of -B, -C and -D series scopes, starting with firmware v4.x. Be very careful when using this function, you may permanently destroy your scope. Check info.txt inside the archive. The checksum verifier is now part of the archive.
* tdsNvramFloppyTools_v5.zip (18.41 kB - downloaded 11 times.)
