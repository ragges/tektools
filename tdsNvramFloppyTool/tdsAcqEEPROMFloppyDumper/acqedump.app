nulldev = open("/null",3,0666)
taskSpawn "Redirect",1,0,0x4000,ioGlobalStdSet,1,nulldev
taskDelay (600)
GpibInput("WAITICON OPEN")

GpibInput("mess:box 80,80,450,200")
GpibInput("mess:show \"\n\n\n\n ACQ EEPROM dump to floppy started, please wait.\"")

acqEEPROMSize=0x200
tempBuf=malloc(acqEEPROMSize)
_gtlX24c02Bcopy(0x10A00000,tempBuf,acqEEPROMSize)
ls "fd0:/"
fd=open("fd0:/acqeeprm.bin",0x0202,0777)
bytesWritten=write(fd,tempBuf,acqEEPROMSize)
close(fd)

confmsg=malloc(500)
sprintf(confmsg, "mess:show \"\n\n\n\n Dump completed, power off instrument.\n\n")
sprintf(confmsg+strlen(confmsg), " Bytes requested: %d \n Bytes written to disk: %d \n\"", acqEEPROMSize, bytesWritten)

GpibInput(confmsg)
GpibInput("WAITICON CLOSE")
GpibInput("bel")

