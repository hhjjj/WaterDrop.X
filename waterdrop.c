/* 
 * File:   waterdrop.c
 * Author: OSSI
 *
 * Created on 2013년 6월 11일 (화), 오후 11:49
 */

#include <xc.h>


#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, FOSC = INTOSC, FCMEN = OFF, MCLRE = OFF, WDTE = OFF, CP = OFF, PWRTE = OFF, CLKOUTEN = OFF

// just for __delay_ms() as reference
// this is NOT for setting up clock speed
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000
#endif

void SetPWMDutyCyle(unsigned int duty_cycle_value)
{
    CCP1CONbits.DC1B = duty_cycle_value & 0x03; //first set the 2 lsb bits
    CCPR1L =  (duty_cycle_value >> 2);           //now set upper 8 msb bits
}


/*
 * 
 */
void main(void) {

    // set high speed internal osc to 8 MHz and 4xPLL on => 32MHz clock speed
    OSCCON = 0b11110000;

    // Set up I/O pins
    ANSELAbits.ANSELA=0;    // set all analog pins to digital I/O
    ADCON0bits.ADON=0;      // turn ADC off
    DACCON0bits.DACEN=0;    // turn DAC off

    // PORT A Assignments
    TRISAbits.TRISA0 = 0;	// RA0 = nc
    TRISAbits.TRISA1 = 0;	// RA1 = nc
    TRISAbits.TRISA2 = 0;	// RA2 = PWM Output (CCP1) connected to LED
    TRISAbits.TRISA3 = 0;	// RA3 = nc (MCLR)
    TRISAbits.TRISA4 = 0;	// RA4 = nc
    TRISAbits.TRISA5 = 0;	// RA5 = nc

    APFCONbits.CCP1SEL=0;       // The CCP1SEL bit selects which pin the PWM output is on.
                                // The default value for this bit is 0 which sets the
                                // PWM output on RA2.  If you want the PWM output on
                                // RA5 instead then set this bit to a 1.


     //******************************************************************************************
    // PWM Period = (1/Fosc) * 4 * (TMR2 Prescaler)* (PR2+1)
    //******************************************************************************************
    // Here are sample PWM periods for different TMR2 Prescalar values for Fosc=32Mhz and PR2=255
    //******************************************************************************************
    // TMR2 Prescalar=1: PWM Period = (1/32000000)*4*1*256 = 32 us or 31.25 khz
    // TMR2 Prescalar=4: PWM Period = (1/32000000)*4*4*256 = 128 us or 7.81 khz
    // TMR2 Prescalar=16: PWM Period = (1/32000000)*4*16*256= 512 ms or 1.953 khz
    // TMR2 Prescalar=64: PWM Period = (1/32000000)*4*64*256= 2.048 ms or .488 khz
    //
    // For this example we will choose the PWM period of 32us (31.25 kHz) so most people
    // will not be able to hear it.

    // ***** Setup PWM output ******************************************************

    TRISAbits.TRISA2 = 1;       // disable pwm pin output for the moment

    CCP1CONbits.CCP1M=0x0C;     // select PWM mode for CCP module
    CCP1CONbits.P1M=0x00;	// select single output on CCP1 pin (RA5)

    PR2 = 0xff;                 // set PWM period as 255 per our example above

    CCPR1L =  0x00;             // clear high 8 bits of PWM duty cycle
    CCP1CONbits.DC1B=0x00;	// clear low 2 bits of PWM Duty cycle

                                // Note: PWM uses TMR2 so we need to configure it
    PIR1bits.TMR2IF=0;		// clear TMR2 interrupt flag
    T2CONbits.T2CKPS=0b11;      // select TMR2 prescalar as divide by 64 as per our example above
    T2CONbits.TMR2ON=1;		// turn TMR2 on

    TRISAbits.TRISA2 = 0;	// turn PWM output back on


    SetPWMDutyCyle(512);
    //volatile unsigned int i  = 0 ;
    while(1)
    {

        
//    LATAbits.LATA0 = 1;
//
//    // __delay_ms give "Unable to resolve identifier"
//    // but it compiled and works
//    //__delay_ms(1000);
//    for ( i = 0 ; i < 500 ; i++)
//    {
//        NOP();
//    }
//
//
//     LATAbits.LATA0 = 0;
//     // __delay_ms give "Unable to resolve identifier"
//    // but it compiled and works
//      //__delay_ms(1000);
//for ( i = 0 ; i < 500 ; i++)
//    {
//        NOP();
//    }



    }
}

