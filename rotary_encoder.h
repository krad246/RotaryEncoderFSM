#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

#include <msp430.h>
#include "debounce.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rotary Encoder A
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P2.4
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#define ROTARY_ENCODER_A_BIT				BIT4
#define ROTARY_ENCODER_A_REN				P2REN
#define ROTARY_ENCODER_A_PORT				P2IN
#define ROTARY_ENCODER_A_DDR				P2DIR
#define SET_ROTARY_ENCODER_A_AS_AN_INPUT	ROTARY_ENCODER_A_DDR &= ~ROTARY_ENCODER_A_BIT;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rotary Encoder B
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P2.3
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#define ROTARY_ENCODER_B_BIT				BIT3
#define ROTARY_ENCODER_B_REN				P2REN
#define ROTARY_ENCODER_B_PORT				P2IN
#define ROTARY_ENCODER_B_DDR				P2DIR
#define SET_ROTARY_ENCODER_B_AS_AN_INPUT	ROTARY_ENCODER_B_DDR &= ~ROTARY_ENCODER_B_BIT;

#define ROTARY_ENCODER_BOUNCE_TIME_MAX 5

typedef enum {AlowBlow,AhighBlow,AlowBhigh,AhighBhigh, Initialize} EncoderState;
typedef struct {
	SwitchStatus SwitchA;
	SwitchStatus SwitchB;
	SwitchStatus Reset;
	int RotaryEncoderStateCount;
	} EncoderDefinitions;

// Prototypes
void InitializeRotaryEncoderPortPins(void);
EncoderState RotaryEncoder(EncoderDefinitions *myRotorEncoder, EncoderState *myEncoderState);

#endif /* ROTARY_ENCODER_H_ */
