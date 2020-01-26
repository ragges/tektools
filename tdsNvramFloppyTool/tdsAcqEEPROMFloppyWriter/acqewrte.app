nulldev = open("/null",3,0666)
taskSpawn "Redirect",1,0,0x4000,ioGlobalStdSet,1,nulldev
taskDelay (60)
GpibInput("WAITICON OPEN")

confmsg=malloc(1000)
GpibInput("mess:box 80,80,575,300")
sprintf(confmsg, "mess:show \"\n\n\n\n ACQ EEPROM WRITING WILL START IN 30 SEC,\n")
sprintf(confmsg+strlen(confmsg), " POWER OFF INSTRUMENT NOW TO CANCEL.\n\n\n")
sprintf(confmsg+strlen(confmsg), " +++ SET THE WRITE PROTECTION SWITCH TO OFF NOW +++\"")

GpibInput(confmsg)

GpibInput("bel")
taskDelay (20)
GpibInput("bel")

taskDelay (1200)

confmsg[49]='1'

GpibInput(confmsg)

GpibInput("bel")
taskDelay (20)
GpibInput("bel")

taskDelay (600)

GpibInput("mess:show \"\n\n\n\n ACQ EEPROM writing started, please wait.\"")

fileName="fd0:/eewrdmp.bin"
acqEEPROMSize=0x200
tempBuf=malloc(acqEEPROMSize)
bfill(tempBuf,acqEEPROMSize,0)
ls "fd0:/"
fd=open(fileName,0,0777)
bytesRead=read(fd,tempBuf,acqEEPROMSize)
close(fd)

taskDelay (120)

_gtlX24c02Bcopy(tempBuf,0x10A00000,acqEEPROMSize)

taskDelay (60)

GpibInput("mess:show \"\n\n\n\n Verifying EEPROM contents, please wait.\"")

cmpBuf=malloc(acqEEPROMSize)

bfill(tempBuf,acqEEPROMSize,0)

ls "fd0:/"
fd2=open(fileName,0,0777)
bytesRead=read(fd2,tempBuf,acqEEPROMSize)
close(fd2)

taskDelay (120)

_gtlX24c02Bcopy(0x10A00000,cmpBuf,acqEEPROMSize)

taskDelay (60)

cmpResult=memcmp(cmpBuf,tempBuf,acqEEPROMSize)

sprintf(confmsg, "mess:show \"\n\n\n\n Write completed, power off instrument.\n\n")
sprintf(confmsg+strlen(confmsg), " Bytes read: %d \n Verification result value: %d ", bytesRead, cmpResult)
sprintf(confmsg+strlen(confmsg), "   (0 = OK,  other value = FAIL)\n")
sprintf(confmsg+strlen(confmsg), "\n\n\n SET THE WRITE PROTECTION SWITCH BACK TO ON NOW\"")

GpibInput(confmsg)
GpibInput("bel;WAITICON CLOSE")
