/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        main.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC30Fxxxx/dsPIC33Fxxxx
* Compiler:        MPLAB® C30 v3.00 or higher
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.*/
/**************************************************************************************/

#ifdef __dsPIC33F__
#include <p33Fxxxx.h>
#else
#include <p30fxxxx.h>
#endif

#include "..\h\Si3000CodecDrv.h"
#include "..\h\FSK.h"

#ifdef __dsPIC33F__

_FOSCSEL(FNOSC_FRC );		// FRC Oscillator 
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT); 
								// Clock Switching is enabled and Fail Safe Clock Monitor is disabled
								// OSC2 Pin Function: OSC2 is Clock Output
								// Primary Oscillator Mode: Disabled


_FWDT(FWDTEN_OFF);              // Watchdog Timer Enabled/disabled by user software
		
#else

_FOSC(CSW_FSCM_OFF & XT_PLL8);   // XT with 8xPLL oscillator, Failsafe clock off
_FWDT(WDT_OFF);                  // Watchdog timer disabled
_FBORPOR(PBOR_OFF & MCLR_EN);    // Brown-out reset disabled, MCLR reset enabled
_FGS(CODE_PROT_OFF);             // Code protect disabled

#endif

// Handle for the codec driver
Si3000CodecHandle si3000Codec;
//Codec registers
Si3000CodecRegisters si3000CodecRegisters;
// Buffer for codec IO buffering
int si3000DrvBuffer[SI3000_DRV_BUFFER_SIZE];

int OutDataBuf[OUTSAMPCNT];

extern int SineWave_1k[32];   // Array to be modulated 


int main(void)
{
    int SampleCount=0,i;

    FskGen_sConfigure CF1;

   /* Configure Oscillator to operate the device at 40MHz.
	 * Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	 * Fosc= 8.0M*40/(2*2)=80Mhz for 8.0M input clock */
#ifdef __dsPIC33F__	 
	PLLFBD=38;				/* M=40	*/
	CLKDIVbits.PLLPOST=0;		/* N1=2	*/
	CLKDIVbits.PLLPRE=0;		/* N2=2	*/
	OSCTUN=0;			
	
	__builtin_write_OSCCONH(0x03);		/*	Initiate Clock Switch to FRC with PLL*/
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0b011);	/*	Wait for Clock switch to occur	*/
	while(!OSCCONbits.LOCK);
#endif   
    TRISDbits.TRISD0=0;        /* LEDs on dsPICDEM 1.1 board are connected to RD0-RD3 */
    TRISDbits.TRISD1=0;        /* We will configure Port D to be output*/
    LATD=(PORTD | 0xff);       	// Switch all LED's off

     /*Initialize FSK Modulator*/
	InitializeFSKTx(&CF1); 
 	// Initialise the Si3000 codec
	Si3000CodecInit(&si3000Codec,si3000DrvBuffer);
	// Start the Si3000 codec
	Si3000CodecStart(&si3000Codec, &si3000CodecRegisters);

 	while(1)
		{

  			 if( SampleCount < INSAMPCNT)
			 {
		 		FSKModulator(&CF1,SineWave_1k[SampleCount],OutDataBuf,OUTSAMPCNT);
  
 		 		SampleCount++;
         		LATD=(PORTD ^ 0x01);    //Toggle RD0 
    		 }
			else 
			 {
 
    			/* Send Idle data*/

				for (i=0;i<OUTSAMPCNT;i++)
 	  		    OutDataBuf[i]=0;   
     		    LATD=(PORTD ^ 0x02);    //Toggle RD1  
  			  }
	
		// Wait until we have an empty buffer to write to
		while(Si3000CodecWriteBusy(&si3000Codec));
    	Si3000CodecWrite(&si3000Codec,OutDataBuf,OUTSAMPCNT);
	}
}


