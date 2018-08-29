#include <msp430.h> 

#include "pushbutton.h"
#include "debounce.h"
#include "timerA.h"
#include "rotary_encoder.h"

// Function Prototypes
void ConfigureClockModule();
void ManageSoftwareTimers(void);

// Global variables
unsigned int g1msTimeout = 0;
unsigned int g1msTimer = 0;

inline void latch();
inline void send(unsigned char value);
void setDigit(unsigned char digit, unsigned char values[]);
void sendValue(int count, unsigned char values[]);

void main(void)
{
    unsigned char digits[16] = {0x3F, 0x06, 0x5B, 0x4F,
                                0x66, 0x6D, 0x7D, 0x07,
                                0x7F, 0x67, 0x77, 0x7C,
                                0x39, 0x5E, 0x79, 0x71};

    SwitchDefine gRotaryEncoderReset; // Used to reset the timer and LED display.
    SwitchDefine gRotaryEncoderA;
    SwitchDefine gRotaryEncoderB;

    EncoderDefinitions myRotaryEncoder;
    EncoderState myEncoderState = Initialize; // Current state of the rotary encoder

    WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
    P1DIR |= (BIT4 | BIT5 | BIT7);
    P2DIR |= BIT0;
    P1OUT = 0x00;

    ConfigureClockModule();

    // Initialize hardware.
    InitializePushButtonPortPin();
    ConfigureTimerA();
    InitializeRotaryEncoderPortPins();

    // Initialize the input "switches" to the rotary encoder.
    InitializeSwitch(&gRotaryEncoderReset,(char *) &PUSHBUTTON_PORT_IN,(unsigned char) PUSHBUTTON_BIT,
                     HIGH_THRESHOLD,LOW_THRESHOLD);
    InitializeSwitch(&gRotaryEncoderA,(char *) &ROTARY_ENCODER_A_PORT,(unsigned char) ROTARY_ENCODER_A_BIT,
                     ROTARY_ENCODER_BOUNCE_TIME_MAX,ROTARY_ENCODER_BOUNCE_TIME_MAX);
    InitializeSwitch(&gRotaryEncoderB,(char *) &ROTARY_ENCODER_B_PORT,(unsigned char) ROTARY_ENCODER_B_BIT,
                     ROTARY_ENCODER_BOUNCE_TIME_MAX,ROTARY_ENCODER_BOUNCE_TIME_MAX);

    _enable_interrupts();

    while (1) {

        // First, update software timers.
        ManageSoftwareTimers();

        if (g1msTimer >= 1000) {
            g1msTimer -= 1000;
            myRotaryEncoder.RotaryEncoderStateCount -= 4;
        }

        // Then, determine the debounced inputs to rotary encoder state machine.
        myRotaryEncoder.Reset = Debouncer(&gRotaryEncoderReset);
        myRotaryEncoder.SwitchA = Debouncer(&gRotaryEncoderA);
        myRotaryEncoder.SwitchB = Debouncer(&gRotaryEncoderB);

        // Next, use the debounced inputs and the current state to generate the next state.
        myEncoderState = RotaryEncoder(&myRotaryEncoder,&myEncoderState);

        // Now, perform the appropriate output task(s).
        if (myEncoderState == Initialize) {

            // Code for when the button is pushed to reset the timer/LED display.
            sendValue(0, digits);
        }
        else {
            if (myRotaryEncoder.RotaryEncoderStateCount >= 396) myRotaryEncoder.RotaryEncoderStateCount = 396;
            else if (myRotaryEncoder.RotaryEncoderStateCount <= 0) myRotaryEncoder.RotaryEncoderStateCount = 0;
            // Code for when rotary encoder is being rotated to set the timer,
            // or the timer is counting down.
            sendValue(myRotaryEncoder.RotaryEncoderStateCount, digits);
        }
    }
}

void ConfigureClockModule()
{
    // Configure Digitally Controlled Oscillator (DCO) using factory calibrations
    DCOCTL  = CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;
}

void ManageSoftwareTimers(void)
{
    if (g1msTimeout) {
        g1msTimeout--;
        g1msTimer++;
    }
}

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
}

void setDigit(unsigned char digit, unsigned char values[]) {
    send(values[digit]);
}

void sendValue(int count, unsigned char values[]) {
    count >>= 2;

    if (count > 98) {
        setDigit(9, values);
        setDigit(9, values);
        latch();
        return;
    }

    if (count < 0) {
        setDigit(0, values);
        setDigit(0, values);
        latch();
        return;
    }

    int tens = 0;

    while (count >= 10) {
        count -= 10;
        tens++;
    }

    setDigit(count, values);
    setDigit(tens, values);

    latch();
}
