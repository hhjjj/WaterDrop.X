/* 
 * File:   waterdrop.c
 * Author: OSSI
 *
 * Created on 2013년 6월 11일 (화), 오후 11:49
 */

#include <xc.h>

const char SineTable[256] = {128,	131,	134,	137,	140,	143,	146,	149,	152,	155,	158,	162,	165,	167,	170,	173,	176,	179,	182,	185,	188,	190,	193,	196,	198,	201,	203,	206,	208,	211,	213,	215,	218,	220,	222,	224,	226,	228,	230,	232,	234,	235,	237,	238,	240,	241,	243,	244,	245,	246,	248,	249,	250,	250,	251,	252,	253,	253,	254,	254,	254,	255,	255,	255,	255,	255,	255,	255,	254,	254,	254,	253,	253,	252,	251,	250,	250,	249,	248,	246,	245,	244,	243,	241,	240,	238,	237,	235,	234,	232,	230,	228,	226,	224,	222,	220,	218,	215,	213,	211,	208,	206,	203,	201,	198,	196,	193,	190,	188,	185,	182,	179,	176,	173,	170,	167,	165,	162,	158,	155,	152,	149,	146,	143,	140,	137,	134,	131,	128,	124,	121,	118,	115,	112,	109,	106,	103,	100,	97,	93,	90,	88,	85,	82,	79,	76,	73,	70,	67,	65,	62,	59,	57,	54,	52,	49,	47,	44,	42,	40,	37,	35,	33,	31,	29,	27,	25,	23,	21,	20,	18,	17,	15,	14,	12,	11,	10,	9,	7,	6,	5,	5,	4,	3,	2,	2,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	2,	2,	3,	4,	5,	5,	6,	7,	9,	10,	11,	12,	14,	15,	17,	18,	20,	21,	23,	25,	27,	29,	31,	33,	35,	37,	40,	42,	44,	47,	49,	52,	54,	57,	59,	62,	65,	67,	70,	73,	76,	79,	82,	85,	88,	90,	93,	97,	100,	103,	106,	109,	112,	115,	118,	121,	124,
};







#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, FOSC = INTOSC, FCMEN = OFF, MCLRE = OFF, WDTE = OFF, CP = OFF, PWRTE = OFF, CLKOUTEN = OFF
// when CLKOUTEN = ON,  Fosc / 4 frequency can be measured via RA4 pin

// just for __delay_ms() as reference
// this is NOT for setting up clock speed
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000
#endif

// NOTE: PR2 value is closely related to maximum PWM resolution!!
void SetPWMDutyCyle(unsigned int duty_cycle_value)
{
    CCP1CONbits.DC1B = duty_cycle_value & 0x03; //first set the 2 lsb bits
    CCPR1L =  (duty_cycle_value >> 2);           //now set upper 8 msb bits
}


char step;
void main(void) {

    // set high speed internal osc to 8 MHz and 4xPLL on => 32MHz clock speed
    OSCCON = 0b11110000;
    //OSCTUNE= 0x1F;			//33 MHz clok really , tune this if you want to have accurate frequency

    // Set up I/O pins
    ANSELAbits.ANSELA=0;    // set all analog pins to digital I/O
    ADCON0bits.ADON=0;      // turn ADC off
    DACCON0bits.DACEN=0;    // turn DAC off

    INTCON = 0b10000000;	// global ints enabled, all sources masked
    OPTION_REG = 0b10000000;	//no weak pull-ups, timer0 at OSC/4/2 = osc/ 8

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

    //PR2 = 0xff;                 // set PWM period as 255 per our example above
    // NOTE: PWM resolution is a function of PR2
    PR2 = 0x3F;                 // set PWM period as 0x3F (125kHz), then PWM resoution is 8 bit

    CCPR1L =  0x00;             // clear high 8 bits of PWM duty cycle
    CCP1CONbits.DC1B=0x00;	// clear low 2 bits of PWM Duty cycle

                                // Note: PWM uses TMR2 so we need to configure it
    PIR1bits.TMR2IF=0;		// clear TMR2 interrupt flag
    T2CONbits.T2CKPS=0b00;      // select TMR2 prescalar as divide by 1 as per our example above
    T2CONbits.TMR2ON=1;		// turn TMR2 on

    TRISAbits.TRISA2 = 0;	// turn PWM output back on


    TMR0 = 0;
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;

    step = 0;

    SetPWMDutyCyle(0);
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

void interrupt isr(void)
{
    TMR0 = 163; // sound update rate around 44.1kHz
    
    SetPWMDutyCyle(SineTable[step]);
    step++;

    if (step > 255)
    {
        step = 0;
    }
    
    INTCONbits.TMR0IF = 0;

}
