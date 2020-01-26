acqEEPROMSize=0x200
tempBuf=malloc(acqEEPROMSize)
bfill(tempBuf,acqEEPROMSize,0)
ls "fd0:/"
fd=open("fd0:/eewrdmp.bin",0,0777)
bytesRead=read(fd,tempBuf,acqEEPROMSize)
close(fd)
_gtlX24c02Bcopy(tempBuf,0x10A00000,acqEEPROMSize)
reboot
