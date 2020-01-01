nvrBase=0x4000000
nvrSize=0xA0000
ls "fd0:/"
fd=open("fd0:/dump.bin",0x0202,0777)
bytesWritten=write(fd,nvrBase,nvrSize)
close(fd)