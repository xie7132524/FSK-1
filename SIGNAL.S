;**********************************************************************
;* © 2007 Microchip Technology Inc.
;*
;* FileName:       SIGNAL.s
;* Dependencies:    Header (.h) files if applicable, see below
;* Processor:      dsPIC30Fxxxx/dsPIC33Fxxxx
;* Compiler:        MPLAB® C30 v3.00 or higher
;*
;* SOFTWARE LICENSE AGREEMENT:
;* Microchip Technology Incorporated ("Microchip") retains all ownership and 
;* intellectual property rights in the code accompanying this message and in all 
;* derivatives hereto.  You may use this code, and any derivatives created by 
;* any person or entity by or on your behalf, exclusively with Microchip's
;* proprietary products.  Your acceptance and/or use of this code constitutes 
;* agreement to the terms and conditions of this notice.
;*
;* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
;* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
;* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
;* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
;* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
;*
;* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
;* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
;* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
;* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
;* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
;* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
;* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
;* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
;* HAVE THIS CODE DEVELOPED.
;*
;* You agree that you are solely responsible for testing the code and 
;* determining its suitability.  Microchip has no obligation to modify, test, 
;* certify, or support the code.
;************************************************************************



; -----------------------------------------------------------------------------
; -----------------------------------------------------------------------------

    .global    _SinCos
    .section    .text
    
; -----------------------------------------------------------------------------
;  Description: This function is used to generate the sine/cos value 
;               required for generation of carriers in all modes of 
;               operation. This function approximates Taylor series 
;               polynomial.
;               sin(x) = 3.140625x + 0.02026367x^2 - 5.325196X^3 + 
;               0.5446778x^4 + 1.800293x^5
;               This routine that implements this sine approximation, 
;               accurate to within two LSBS. This routine accepts input 
;               values in 1.15 format. The coefficients, which are 
;               initialised in data memory in 4.12 format, have been 
;               adjusted to reflect an input value scaled to the maximum 
;               range allowed by this format.On this scale, 180º equals the
;               maximum positive value, 7FFF, and -180º equals the maximum
;               negative value, 8000.
; -----------------------------------------------------------------------------

_SinCos:

    push CORCON

    clr       	    CORCON              ;fractional multiplication
    mov.d        w0, w4                ;save the inputs

    btsc         w0, #14
    neg          w0, w4                ;Take Absolute value of sine input
    bclr         w4,#15

    btsc         w1, #14
    neg          w1, w5                ;Take Absolute value of sine input
    bclr         w5,#15

    mov          #0x1CCE, w7          
    mov          #0x08B7, w3           ;coefficient of sine series
    lac          w3, B

    mpy          w4*w7, A              ;a * x
    add          A
    sac.r        A, w6

    mpy          w5*w7, A              ;a * y
    add          A
    sac.r        A, w7

    mov          #0xAACC, w3           ;Take second coeff b
    lac          w3, B

    mpy          w4*w6, A              ;x^2 * b
    add          A                     ;a*x +b* x^2
    sac.r        A, w6

    mpy          w5*w7, A              ;y^2 * b
    add          A                     ;a*y +b* y^2
    sac.r        A, w7

    mov          #0x0053, w3           ;Take the next coeff c
    lac          w3, B

    mpy          w4*w6, A              ;x^3 * c
    add          A                     ;a*x + b*x^2 +c*x^3
    sac.r        A, w6

    mpy          w5*w7, A              ;y^3 * c
    add          A                     ;a*y + b*y^2 +c*y^3
    sac.r        A, w7

    mov          #0x3240, w3           ;Take the next coeff d
    lac          w3, B

    mpy          w4*w6, A              ;x^4 * d
    add          A                     ;a*x + b*x^2 +c*x^3 +d*x^4
    sac.r        A, w6

    mpy          w5*w7, A              ;y^4 * d
    add          A                     ;a*y + b*y^2 +c*y^3 +d*y^4
    sac.r        A, w7

    mpy          w4*w6, A             
    sac.r        A, w6

    mpy          w5*w7, A
    sac.r        A, w7

    sl           w6, #3, w6   ; x1
    sl           w7, #3, w7   ; x11

    cp0          w6                    ;check the sin result for saturation
    bra          ge, sin_out
    mov          #0x7fff, w6           ;saturate to max positive value

sin_out:
    mov          w6, w4    

    cp0          w7                    ;check the cos result for saturation
    bra          ge, cos_out1
    mov          #0x7fff,w7            ;saturate to max positive value


cos_out1:
    mov          w7, w5

    btsc         w0, #15
    neg          w4, w4                ;Finally include sign bit to sin result

    btsc         w1, #15
    neg          w5, w5                ;Finally include sign bit to cos result

    mov.d        w4, w0                ;Return sine and cosine outputs

    pop CORCON
    return


.global _Multiply

.section .text

_Multiply:

    push CORCON
    clr    CORCON
    mov       w0, w4
    mov       w1, w5
  
    mpy       w4*w5, A
    sac.r     A, w0                    ;Sample ready for transmission
    pop CORCON
return

