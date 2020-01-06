/*
 *
 *	CAL EEPROM backup - for TDS520B,TDS540B,TDS724A,TDS744A,TDS754A,TDS782A,TDS784A 
 *	
 *	compiled with MinGW gcc on Windows Vista / Win7 
 *  tested with Agilent I/O 16.x and S82357 from http://bmjd.biz/index.php 
 *  
 *	
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  #include <conio.h>
  #include <windows.h>
  /* for NI adapters uncomment following line */

  /* for Agilent adapters uncomment following line */
  #include "ni488.h"
#elif defined(__linux__)
  /* linux with linux-gpib */
  #include <gpib/ib.h>
#elif defined(__APPLE__)
  /* MacOS with NI GPIB drivers */
  #include <ni4882.h>
#else
        #error "Unknown compiler environment!"
#endif


#define ARRAYSIZE 100            // Size of read buffer

int  Dev;                        // Device handle
char ReadBuffer[ARRAYSIZE + 1];  // Read data buffer
char ErrorMnemonic[21][5] = {"EDVR", "ECIC", "ENOL", "EADR", "EARG",
                             "ESAC", "EABO", "ENEB", "EDMA", "",
                             "EOIP", "ECAP", "EFSO", "", "EBUS",
                             "ESTB", "ESRQ", "", "", "", "ETAB"};


void GPIBCleanup(int Dev, char* ErrorMsg);
static int debug;


int main(void)  {

/*
 * ========================================================================
 *
 * INITIALIZATION SECTION
 *
 * ========================================================================
 */

/*
 *  Assign a unique identifier to the device and store in the variable
 *  Dev.  If the ERR bit is set in ibsta, call GPIBCleanup with an
 *  error message. Otherwise, the device handle, Dev, is returned and
 *  is used in all subsequent calls to the device.
 */

#define BDINDEX               0     // Board Index
#define PRIMARY_ADDR_OF_DMM   1     // Primary address of device
#define NO_SECONDARY_ADDR     0     // Secondary address of device
#define TIMEOUT               T10s  // Timeout value = 10 seconds
#define EOTMODE               1     // Enable the END message
#define EOSMODE               0     // Disable the EOS mode

FILE *outfile;
int f;
#if 0
unsigned long addr;
// addr = 0x00040000L; /* CAL data base address on TDS5xxB,6xxA,7xxA*/
addr = 262144; /* CAL data base address on TDS5xxB,6xxA,7xxA*/
#endif


    Dev = ibdev (BDINDEX, PRIMARY_ADDR_OF_DMM, NO_SECONDARY_ADDR,
                TIMEOUT, EOTMODE, EOSMODE);
    if (ibsta & ERR)
    {
       GPIBCleanup(Dev, "Unable to open device");
       return 1;
    }

/*
 *  Clear the internal or device functions of the device.  If the error
 *  bit ERR is set in ibsta, call GPIBCleanup with an error message.
 */

    ibclr (Dev);
    if (ibsta & ERR)
    {
       GPIBCleanup(Dev, "Unable to clear device");
       return 1;
    }

/*
 * ========================================================================
 *
 *  MAIN BODY SECTION
 *
 *  In this application, the Main Body communicates with the instrument
 *  by writing a command to it and reading its response. 
 *
 * ========================================================================
 */


        ibwrt (Dev, "*IDN?", 5L);
    if (ibsta & ERR)
    {
       GPIBCleanup(Dev, "Unable to write to device");
       return 1;
    }

    ibrd (Dev, ReadBuffer, ARRAYSIZE);
    if (ibsta & ERR)
    {
       GPIBCleanup(Dev, "Unable to read data from device");
       return 1;
    }

    ReadBuffer[ibcntl] = '\0';
    printf("DSO IDN:  %s\n", ReadBuffer);
    
    outfile = fopen("U1052.bin","wb");
    printf("dumping U1052.bin\n");
    printf("\nPlease wait ...\n");
    
    
/* the first 8 bytes of the U1052 EEPROM are not mapped and empty (0x00h)
   so we write as well 8 time 0x00h to dump file  */

	for(f = 0; f < 8; f++)
	{
		fputc(0x00, outfile);
	}


/* send first TEKTRONIX Password fr TDS5xxB/7xxA models to allow memory dump */
	ibwrt (Dev, "PASSWORD PITBULL", 17L);
    if (ibsta & ERR)
    {
       GPIBCleanup(Dev, "Unable to write to device");
       return 1;
    }

	unsigned char calbuf[255];
	int caldata;


	for(f = 0; f < 124; f++)
	{
        char cmdbuf[64] = "WORDCONSTANT:ATOFFSET? 262144,";	
	char cmdnr[16];
	sprintf(cmdnr,"%d",f);
	strcat(cmdbuf, cmdnr);
	
    if (debug > 1) printf("DSO IDN:  %s\n", cmdbuf);

	ibwrt (Dev, cmdbuf, sizeof(cmdbuf));
     if (ibsta & ERR)
    	{
    	   GPIBCleanup(Dev, "Unable to write to device");
    	   return 1;
    	}
	ibrd(Dev, calbuf, 6);
     if (ibsta & ERR)
    	{
    	   GPIBCleanup(Dev, "Unable to write to device");
    	   return 1;
    	}
    	
     caldata = atoi((char *) calbuf);
    if (debug > 1) printf("CAL DATA: %X\n", caldata);
  
    unsigned char* abuffer  = (unsigned char *)&caldata;
   	fputc(abuffer[1], outfile);
   	fputc(abuffer[0], outfile);
	}
	fclose(outfile);
	
    outfile = fopen("U1055.bin","wb");
    printf("dumping U1055.bin\n");
    printf("\nPlease wait ...\n");
	
	for(f = 0; f < 128; f++)
	{
	char cmdbuf[64] = "WORDCONSTANT:ATOFFSET? 262144,";	
	char cmdnr[16];
	sprintf(cmdnr,"%d",f+124);
	strcat(cmdbuf, cmdnr);
	
    if (debug > 1) printf("DSO IDN:  %s\n", cmdbuf);

	ibwrt (Dev, cmdbuf, sizeof(cmdbuf));
     if (ibsta & ERR)
    	{
    	   GPIBCleanup(Dev, "Unable to write to device");
    	   return 1;
    	}
	ibrd(Dev, calbuf, 6);
     if (ibsta & ERR)
    	{
    	   GPIBCleanup(Dev, "Unable to write to device");
    	   return 1;
    	}
    	
    caldata = atoi((char *) calbuf);
    if (debug > 1) printf("CAL DATA: %X\n", caldata);
  
    unsigned char* abuffer  = (unsigned char *)&caldata;
   	fputc(abuffer[1], outfile);
   	fputc(abuffer[0], outfile);
	}
	fclose(outfile);
    
    printf("\nCalibration data has been successful dumped\n");

/*
 * ========================================================================
 *
 * CLEANUP SECTION
 *
 * ========================================================================
 */

/*  Take the device offline. */

    
    ibonl (Dev, 0);

    return 0;

}


/*
 *  After each GPIB call, the application checks whether the call
 *  succeeded. If an NI-488.2 call fails, the GPIB driver sets the
 *  corresponding bit in the global status variable. If the call
 *  failed, this procedure prints an error message, takes the
 *  device offline and exits.
 */
void GPIBCleanup(int ud, char* ErrorMsg)
{
    printf("Error : %s\nibsta = 0x%x iberr = %d (%s)\n",
            ErrorMsg, ibsta, iberr, ErrorMnemonic[iberr]);
    if (ud != -1)
    {
       printf("Cleanup: Taking device offline\n");
       ibonl (ud, 0);
    }
}




