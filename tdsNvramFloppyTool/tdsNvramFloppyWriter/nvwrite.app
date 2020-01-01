nulldev = open("/null",3,0666)
taskSpawn "Redirect",1,0,0x4000,ioGlobalStdSet,1,nulldev
taskDelay (60)
GpibInput("WAITICON OPEN")

GpibInput("mess:box 80,80,550,200")
GpibInput("mess:show \"\n\n\n\n NVRAM WRITING WILL START IN 30 SEC,\n POWER OFF INSTRUMENT NOW TO CANCEL.\"")

GpibInput("bel")
taskDelay (20)
GpibInput("bel")

taskDelay (1200)

GpibInput("mess:show \"\n\n\n\n NVRAM WRITING WILL START IN 10 SEC,\n POWER OFF INSTRUMENT NOW TO CANCEL.\"")

GpibInput("bel")
taskDelay (20)
GpibInput("bel")

taskDelay (600)

GpibInput("mess:show \"\n\n\n\n NVRAM writing started, please wait.\"")

nvrBase=0x4000000
nvrSize=0xA0000

tempBuf=malloc(nvrSize)

ls "fd0:/"
fd=open("fd0:/writedmp.bin",0,0777)
bytesRead=read(fd,tempBuf,nvrSize)
close(fd)

confmsg=malloc(500)
sprintf(confmsg, "mess:show \"\n\n\n\n NVRAM write will be executed in 10s.\n")
sprintf(confmsg+strlen(confmsg), " DO NOT power off, wait for automatic reboot.\n")
sprintf(confmsg+strlen(confmsg), " Bytes requested: %d \n Bytes written to temporary buffer: %d \n\"", nvrSize, bytesRead)

GpibInput(confmsg)
GpibInput("bel;WAITICON CLOSE")

taskDelay (600)

memcpy(nvrBase,tempBuf,nvrSize); reboot