nvrBase=0x4000000
nvrSize=0xA0000
ls "fd0:/"
fd=open("fd0:/writedmp.bin",0,0777)
bytesRead=read(fd,nvrBase,nvrSize)
close(fd)
reboot