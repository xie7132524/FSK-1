/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        FskModulator.c
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
* certify, or support the code.
************************************************************************/

#include "..\h\FSK.h"

/********************************************************************************/
//  Description:   This function initializes the Interpolation factor and also the
//   phase Advance for Mark and Space frequency         
/********************************************************************************/

void InitializeFSKTx(FskGen_sConfigure * CF1)
{
   CF1->FSKMarkAdvance= MARKADVANCE;  //advance for 1300Hz, for ones
   CF1->FSKSpaceAdvance=SPACEADVANCE;  //advance for 2100Hz, for zeroes
   CF1->FSKTxIpFactor=IPFACTOR;       //Interpolation Factor for 1200 Bauds
   CF1->FSKTxSampleCnt=0;
   CF1->FSKCurPhase=0;
   CF1->FSKCarAdvance=0;
}


/********************************************************************************/
//  Description:   The FSK Modulatator consists of only a carrier 
//                 generator. Every baud internal, the new bit signals the 
//                 change in the phase advance of the carrier.  The resulting 
//                 signal is of continuous phase.
/********************************************************************************/


void FSKModulator( FskGen_sConfigure * CF1,int PcmTxData,  int*OutDataBuf,int OutSampCnt )

{
  int Data; 
  int Count=0;
  int SinOut=0;
  int SinOut1=0;
  int dataTxBitCnt=0;
 
while (OutSampCnt !=0 )
{
      if ( CF1->FSKTxSampleCnt > 0)                //check for samples count
	  {  
      		CF1->FSKTxSampleCnt--;
			
     		CF1->FSKCurPhase+=CF1->FSKCarAdvance;       //update the phase with carrier advance
           
            SinOut=SinCos(CF1->FSKCurPhase);

            SinOut1=Multiply( SinOut,0x6f0);
            OutSampCnt--;
      }

	  else {

			CF1->FSKTxSampleCnt =	CF1->FSKTxIpFactor;    //Reset the samples counter

            Data = GetLsb2Msb(PcmTxData,dataTxBitCnt);
            dataTxBitCnt++;

            if( Data > 0)
            CF1->FSKCarAdvance=CF1->FSKMarkAdvance;     // Init carrier advance with Mark 
            else
            CF1->FSKCarAdvance=CF1->FSKSpaceAdvance;    // Init carrier advance with Space 


            CF1->FSKTxSampleCnt--;
				
            CF1->FSKCurPhase+=CF1->FSKCarAdvance;       //update the phase with carrier advance
			
            SinOut=SinCos(CF1->FSKCurPhase);       //Modulation  

            SinOut1=Multiply( SinOut,0x6f0);  
            OutSampCnt--;
            }
      
     OutDataBuf[Count++]= SinOut1;            //Sample ready for transmission
    


} /*end of while*/


 }

/********************************************************************************/
//  Description:   This function gets the transmit data and extract bits from
//                LSB to MSB.
/********************************************************************************/

int GetLsb2Msb(int Data,int Index)
{
   
   int temp=0,Output;
   temp = Data >> Index;
   Output = temp  & 0x1;
             
   return(Output);
} 




