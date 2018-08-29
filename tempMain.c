#include <msp430.h> 

inline void latch() {
    P2OUT |= BIT0;
    P2OUT &= ~BIT0;
}

inline void send(unsigned char value) {
    unsigned char i;
    for (i = 8; i > 0; i--) {
        if (((1 << (i - 1)) & value) > 0)  {
            P1OUT |= BIT7;
        } else {
            P1OUT &= ~BIT7;
        }
        P1OUT |= BIT5;
        P1OUT &= ~BIT5;
    }

    latch();
}

void setDigit(unsigned char digit, unsigned char values[]) {
    send(values[digit]);
}

int main(void)
{
    unsigned char digits[16] = {0x3F, 0x06, 0x5B, 0x4F,
                                0x66, 0x6D, 0x7D, 0x07,
                                0x7F, 0x67, 0x77, 0x7C,
                                0x39, 0x5E, 0x79, 0x71};
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
    P1DIR |= (BIT4 | BIT5 | BIT7);
    P2DIR |= BIT0;

    while(1){
        setDigit(9, digits);
        setDigit(7, digits);
    }

	return 0;
}
