acqEEPROMSize=0x200
tempBuf=malloc(acqEEPROMSize)
_gtlX24c02Bcopy(0x10A00000,tempBuf,acqEEPROMSize)
ls "fd0:/"
fd=open("fd0:/acqeeprm.bin",0x0202,0777)
bytesWritten=write(fd,tempBuf,acqEEPROMSize)
close(fd)