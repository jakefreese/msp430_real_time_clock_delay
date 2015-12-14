#include "RTC.h"
#include "msp430g2553.h"
#define BUTTON BIT3  // P1.3 Button on MSP430G2553
#define LEDR   BIT6  // P1.6 LED 
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
	P1DIR |= 0x01; // Set P1.0 to output direction
	TA0CCR0 = 32768 - 1;
	TA0CTL = TASSEL_1 + MC_1; // ACLK, upmode
	TA0CCTL0 |= CCIE; // enable CCRO interrupt
	_EINT();
	P1OUT &= 0x00;               // Shut down everything
	P1DIR &= 0x00;
	P1DIR |= BIT0 + BIT6;        // P1.0 and P1.6 pins output the rest are input
	P1REN |= BIT3;                   // Enable internal pull-up/down resistors
	P1OUT |= BIT3;                   //Select pull-up mode for P1.3
	P1IE |= BIT3;                       // P1.3 interrupt enabled
	P1IES |= BIT3;                     // P1.3 Hi/lo edge
	P1IFG &= ~BIT3;                  // P1.3 IFG cleared
	while (1)
	{
		LPM3; // enter LPM3, clock will be updated
		P1OUT ^= 0x01; // do any other needed items in loop
		_NOP(); // set breakpoint here to see 1 second int.
	}
}
// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A(void)
{
	incrementSeconds();
	LPM3_EXIT;
}
// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	if(P1IN & BUTTON);		// Start timer when button is high
	incrementSeconds(2);    // count 2 seconds as long as button is high
	P1OUT |= LEDR;		    // Set output high
	incrementSeconds(60);	// count 60 seconds after output is high
	P1IFG &= ~LEDR;         // Clear IFG, reset P1.6 low
}
