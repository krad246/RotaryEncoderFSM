#include "rotary_encoder.h"

void InitializeRotaryEncoderPortPins(void)
{
    SET_ROTARY_ENCODER_A_AS_AN_INPUT;
    SET_ROTARY_ENCODER_B_AS_AN_INPUT;
}

EncoderState RotaryEncoder(EncoderDefinitions *myRotorEncoder, EncoderState *myEncoderState)
{
    EncoderState NextEncoderState;

        switch (*myEncoderState) {
        case Initialize:
            //myRotorEncoder->RotaryEncoderStateCount=0;
            if (myRotorEncoder->SwitchA== High && myRotorEncoder->SwitchB== High && myRotorEncoder->Reset==Low){
                NextEncoderState=AhighBhigh;

            }
            else if (myRotorEncoder->SwitchA== High && myRotorEncoder->SwitchB== Low && myRotorEncoder->Reset==Low){
                NextEncoderState=AhighBlow;
            }
            else if (myRotorEncoder->SwitchA== Low && myRotorEncoder->SwitchB== High && myRotorEncoder->Reset==Low){
                NextEncoderState=AlowBhigh;
            }
            else if (myRotorEncoder->SwitchA== Low && myRotorEncoder->SwitchB== Low && myRotorEncoder->Reset==Low){
                NextEncoderState=AlowBlow;
            }
            else  if(myRotorEncoder->Reset==High ){
                NextEncoderState=Initialize;
            }
            break;

        case AlowBlow:
            if (myRotorEncoder->SwitchA== High && myRotorEncoder->SwitchB== Low && myRotorEncoder->Reset==Low){
                NextEncoderState=AhighBlow;
                ++myRotorEncoder->RotaryEncoderStateCount;
            }
            else if (myRotorEncoder->SwitchA== Low && myRotorEncoder->SwitchB== High && myRotorEncoder->Reset==Low){
                NextEncoderState=AlowBhigh;
                --myRotorEncoder->RotaryEncoderStateCount;
            }
            else  if(myRotorEncoder->Reset==High ){
                NextEncoderState=Initialize;
            }
            else{
                NextEncoderState=AlowBlow;
            }

            break;

        case AlowBhigh:
            if (myRotorEncoder->SwitchA== High && myRotorEncoder->SwitchB== High && myRotorEncoder->Reset==Low){
                NextEncoderState=AhighBhigh;
                --myRotorEncoder->RotaryEncoderStateCount;
            }
            else if (myRotorEncoder->SwitchA== Low && myRotorEncoder->SwitchB== Low && myRotorEncoder->Reset==Low){
                NextEncoderState=AlowBlow;
                ++myRotorEncoder->RotaryEncoderStateCount;
            }
            else  if(myRotorEncoder->Reset==High ){
                NextEncoderState=Initialize;
            }
            else{
                NextEncoderState=AlowBhigh;
            }
            break;

        case AhighBlow:
            if (myRotorEncoder->SwitchA== High && myRotorEncoder->SwitchB== High && myRotorEncoder->Reset==Low){
                NextEncoderState=AhighBhigh;
                ++myRotorEncoder->RotaryEncoderStateCount;
            }
            else if (myRotorEncoder->SwitchA== Low && myRotorEncoder->SwitchB== Low && myRotorEncoder->Reset==Low){
                NextEncoderState=AlowBlow;
                --myRotorEncoder->RotaryEncoderStateCount;
            }
            else  if(myRotorEncoder->Reset==High ){
                NextEncoderState=Initialize;
            }
            else{
                NextEncoderState=AhighBlow;
            }
            break;

        case AhighBhigh:
            if (myRotorEncoder->SwitchA== High && myRotorEncoder->SwitchB== Low && myRotorEncoder->Reset==Low){
                NextEncoderState=AhighBlow;
                --myRotorEncoder->RotaryEncoderStateCount;
            }
            else if (myRotorEncoder->SwitchA== Low && myRotorEncoder->SwitchB== High && myRotorEncoder->Reset==Low){
                NextEncoderState=AlowBhigh;
                ++myRotorEncoder->RotaryEncoderStateCount;
            }
            else if(myRotorEncoder->Reset==High ){
                NextEncoderState=Initialize;
            }
            else{
                NextEncoderState=AhighBhigh;
            }
            break;

        default:
            NextEncoderState = Initialize;
        }

        return NextEncoderState;
}
