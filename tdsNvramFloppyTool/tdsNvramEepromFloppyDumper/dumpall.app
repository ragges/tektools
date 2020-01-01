nulldev = open("/null",3,0666)
taskSpawn "Redirect",1,0,0x4000,ioGlobalStdSet,1,nulldev
taskDelay (600)
GpibInput("WAITICON OPEN")

GpibInput("mess:box 40,80,490,400")
GpibInput("mess:show \"\n\n\n\n NVRAM dump to floppy started, please wait.\"")

nvrBase=0x4000000
nvrSize=0xA0000

tempBuf=malloc(nvrSize)

memcpy(tempBuf,nvrBase,nvrSize)

ls "fd0:/"
fd=open("fd0:/nvram.bin",0x0202,0777)
nvbytesWritten=write(fd,tempBuf,nvrSize)
close(fd)

confmsg=malloc(1000)
sprintf(confmsg, "mess:show \"\n\n\n\n NVRAM Dump completed.\n")
sprintf(confmsg+strlen(confmsg), " Bytes requested: %d \n Bytes written: %d\n", nvrSize, nvbytesWritten)
sprintf(confmsg+strlen(confmsg), " Starting EEPROM dump...\"")
GpibInput(confmsg)

acqEEPROMSize=0x200
tempBuf=malloc(acqEEPROMSize)
_gtlX24c02Bcopy(0x10A00000,tempBuf,acqEEPROMSize)
fdee=open("fd0:/acqeeprm.bin",0x0202,0777)
eebytesWritten=write(fdee,tempBuf,acqEEPROMSize)
close(fdee)

confmsg=malloc(1000)
sprintf(confmsg, "mess:show \"\n\n\n\n NVRAM Dump completed.\n")
sprintf(confmsg+strlen(confmsg), " Bytes requested: %d \n Bytes written: %d\n", nvrSize, nvbytesWritten)
sprintf(confmsg+strlen(confmsg), " EEPROM dump completed.\n")
sprintf(confmsg+strlen(confmsg), " Bytes requested: %d \n Bytes written: %d\n\n", acqEEPROMSize, eebytesWritten)
sprintf(confmsg+strlen(confmsg), " Dump completed, power off instrument.\"")
GpibInput(confmsg)

GpibInput("WAITICON CLOSE")
GpibInput("bel")
