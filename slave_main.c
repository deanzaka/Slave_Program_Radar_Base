/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.3 Standard
Automatic Program Generator
� Copyright 1998-2011 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : Slave Program Radar Base 
Version : 1.5
Date    : 21/07/2014
Author  : deanzaka
Company : Solusi247
Comments: 


Chip type               : ATmega8
Program type            : Application
AVR Core Clock frequency: 11,059200 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*****************************************************/

#include <mega8.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <delay.h>

int     STEP    =   5000;
int   RES     =   1;

unsigned char degchar[7];
unsigned char rpmchar[7];
int i = 0;
int c = 0;

float rpm;
int deg;
int degint, rpmint;
int freq = 0;

// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
   // Place your code here
    freq++;
    if(PIND.4 == 0) 
    {
        if(PIND.3 == 0) 
        deg = deg + 1;
    }
    else if (PIND.4 == 1)
    {
        if(PIND.3 == 0) 
        deg = deg - 1;
    }
}

// External Interrupt 1 service routine
interrupt [EXT_INT1] void ext_int1_isr(void)
{
    // Place your code here
    if(PIND.2 == 1) deg = 0;
    else if (PIND.2 == 0) deg = 5000;
}

// Timer1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
// Reinitialize Timer1 value
TCNT1H=0xD5D0 >> 8;
TCNT1L=0xD5D0 & 0xff;
// Place your code here
rpm = (float) freq*60/STEP;
freq=0;
}

// TWI functions
#include <twi.h>

// TWI Slave receive buffer
#define TWI_RX_BUFFER_SIZE 32
unsigned char twi_rx_buffer[TWI_RX_BUFFER_SIZE];

// TWI Slave transmit buffer
#define TWI_TX_BUFFER_SIZE 32
unsigned char twi_tx_buffer[TWI_TX_BUFFER_SIZE];

// TWI Slave receive handler
// This handler is called everytime a byte
// is received by the TWI slave
bool twi_rx_handler(bool rx_complete)
{
if (twi_result==TWI_RES_OK)
   {
   // A data byte was received without error
   // and it was stored at twi_rx_buffer[twi_rx_index]
   // Place your code here to process the received byte
   // Note: processing must be VERY FAST, otherwise
   // it is better to process the received data when
   // all communication with the master has finished
   }
else
   {
   // Receive error
   // Place your code here to process the error
   return false; // Stop further reception
   }

// The TWI master has finished transmitting data?
if (rx_complete) return false; // Yes, no more bytes to receive

// Signal to the TWI master that the TWI slave
// is ready to accept more data, as long as
// there is enough space in the receive buffer
return (twi_rx_index<sizeof(twi_rx_buffer));
}

// TWI Slave transmission handler
// This handler is called for the first time when the
// transmission from the TWI slave to the master
// is about to begin, returning the number of bytes
// that need to be transmitted
// The second time the handler is called when the
// transmission has finished
// In this case it must return 0
unsigned char twi_tx_handler(bool tx_complete)
{
if (tx_complete==false)
{
	// Transmission from slave to master is about to start
    //twi_tx_buffer[0] = degsend[c];
    //if(c < 6) c++;
    //else c = 0;
    
    
    if(twi_rx_buffer[0] == 'D')
    {
        //degint = (int) deg;
        itoa(deg, degchar);
        strcpy(twi_tx_buffer, degchar);
    }   
    else if(twi_rx_buffer[0] == 'R')
    {
        //rpmint = (int) rpm;
        ftoa(rpm, 3, rpmchar);
        strcpy(twi_tx_buffer, rpmchar);
    }
    else if(twi_rx_buffer[0] == 'N')
    {
        //force reset with watchdogs timer
		#asm("cli")
		WDTCR=0x18;
		WDTCR=0x08;
		while(1);
    }
    
    
    //strcpy(twi_tx_buffer, " COBA ");
    return strlen(twi_tx_buffer);	
}


// Transmission from slave to master has finished
// Place code here to eventually process data from
// the twi_rx_buffer, if it wasn't yet processed
// in the twi_rx_handler

// No more bytes to send in this transaction
return 0;
}

// Declare your global variables here

void main(void)
{
// Declare your local variables here

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 10,800 kHz
// Mode: Normal top=0xFFFF
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: On
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x05;
TCNT1H=0xD5;
TCNT1L=0xD0;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// External Interrupt(s) initialization
// INT0: On
// INT0 Mode: Rising Edge
// INT1: On
// INT1 Mode: Rising Edge
GICR|=0xC0;
MCUCR=0x0F;
GIFR=0xC0;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x04;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// SPI initialization
// SPI disabled
SPCR=0x00;

// TWI initialization
// Mode: TWI Slave
// Match Any Slave Address: On
twi_slave_init(true,0x00,twi_rx_buffer,1,twi_tx_buffer,twi_rx_handler,twi_tx_handler);

// Global enable interrupts
#asm("sei")

while (1)
{
    // Place your code here   
}
}
