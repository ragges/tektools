nulldev = open("/null",3,0666)
taskSpawn "Redirect",1,0,0x4000,ioGlobalStdSet,1,nulldev
taskDelay (600)
GpibInput("WAITICON OPEN")

GpibInput("mess:box 80,80,450,200")
GpibInput("mess:show \"\n\n\n\n NVRAM dump to floppy started, please wait.\"")

nvrBase=0x4000000
nvrSize=0xA0000

tempBuf=malloc(nvrSize)

memcpy(tempBuf,nvrBase,nvrSize)

ls "fd0:/"
fd=open("fd0:/dump.bin",0x0202,0777)
bytesWritten=write(fd,tempBuf,nvrSize)
close(fd)

confmsg=malloc(500)
sprintf(confmsg, "mess:show \"\n\n\n\n Dump completed, power off instrument.\n\n")
sprintf(confmsg+strlen(confmsg), " Bytes requested: %d \n Bytes written to disk: %d \n\"", nvrSize, bytesWritten)

GpibInput(confmsg)
GpibInput("WAITICON CLOSE")
GpibInput("bel")

