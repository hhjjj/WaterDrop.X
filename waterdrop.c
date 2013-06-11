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



/*
 * 
 */
void main(void) {

    TRISA = 0;
    OSCCON = 0b11110000;
    
    volatile unsigned int i  = 0 ;
    while(1)
    {
    LATAbits.LATA0 = 1;

    // __delay_ms give "Unable to resolve identifier"
    // but it compiled and works
    //__delay_ms(1000);
    for ( i = 0 ; i < 500 ; i++)
    {
        NOP();
    }


     LATAbits.LATA0 = 0;
     // __delay_ms give "Unable to resolve identifier"
    // but it compiled and works
      //__delay_ms(1000);
for ( i = 0 ; i < 500 ; i++)
    {
        NOP();
    }



    }
}

