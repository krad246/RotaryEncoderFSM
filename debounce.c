#include "debounce.h"

#define FALSE 0
#define TRUE 1

extern unsigned int g1msTimer;

void InitializeSwitch(SwitchDefine *Switch,char *SwitchPort,unsigned char SwitchBit,
		unsigned char HoldTime,unsigned char ReleaseTime)
{
	Switch->CurrentState = DbExpectHigh;
	Switch->SwitchPort = SwitchPort;
	Switch->SwitchPortBit = SwitchBit;

	Switch->HoldTime = HoldTime; // units equal milliseconds
	Switch->ReleaseTime = ReleaseTime; // units equal milliseconds
	Switch->EventTime = 0;
}

SwitchStatus GetSwitch(SwitchDefine *Switch)
{
	volatile unsigned char SwitchValue;

	SwitchValue = *(Switch->SwitchPort) & Switch->SwitchPortBit;
	if (SwitchValue)
		return Low;
	else
		return High;
}

SwitchStatus Debouncer(SwitchDefine *Switch)
{
/*
 * This function implements a 4-state Mealy state machine:
 *
 * DbExpectHigh (DEH): 		The switch is currently in the inactive state and awaiting a transition to the
 * 							active state.
 * DbValidateHigh (DVH): 	The state has transitioned to the active state, but the elapsed time from the
 * 							transition to the current time (in g1msTimer) has not exceeded the threshold,
 * 							HoldTime.
 * DbExpectLow (DEL):		The switch is currently in the active state and awaiting a transition to the
 * 							inactive state.
 * DbValidateLow (DVL):		The state has transitioned to the inactive state, but the elapsed time from the
 * 							transition to the current time (in g1msTimer) has not exceeded the threshold,
 * 							ReleaseTime.
 *
 * The state machine has two inputs:
 *
 * X1:	This input is a logical condition based on the elapsed time from the transition from the inactive state
 * 		to the active state (or vice-versa). X1 = true implies the elapsed time is greater than the threshold.
 * X0:	This input is a logical condition based on the state of the switch (high or low). X0 = true implies
 * 		that the switch is in the active (High) state.
 *
 * State Transition Table:
 * 		NS
 * CS	X1X0 = 	FF  FT  TF  TT
 * --------------------------------
 * DEH			DEH DVH DEH DVH
 * DVH			DEH DVH DEH DEL
 * DEL			DVL DEL DVL DEL
 * DVL			DVL DEL DEH DEL
 *
 */

	int ElapsedTime = 0;
	SwitchStatus CurrentSwitchReading;
	SwitchStatus DebouncedSwitchStatus;
	unsigned char X1;
	unsigned char X0;
	DbState NextState;

	// First, determine the current inputs, X1 and X0.
	ElapsedTime = g1msTimer-Switch->EventTime;
	if (ElapsedTime < 0) {
		ElapsedTime = ElapsedTime + (unsigned int) 0xFFFF + 1;
	}
	if (Switch->CurrentState == DbValidateHigh) {
		if (ElapsedTime > Switch->HoldTime) {X1 = TRUE;}
		else {X1 = FALSE;}
	}
	else if (Switch->CurrentState == DbValidateLow){
		if (ElapsedTime > Switch->ReleaseTime) {X1 = TRUE;}
		else {X1 = FALSE;}
	}

	CurrentSwitchReading = GetSwitch(Switch);
	if (CurrentSwitchReading == High) {X0 = TRUE;}
	else {X0 = FALSE;}

	// Next, based on the input values and the current state, determine the next state.
	switch (Switch->CurrentState) {
		case DbExpectHigh:
			if (X0 == TRUE) {NextState = DbValidateHigh;}
			else {NextState = DbExpectHigh;}
		break;
		case DbValidateHigh:
			if (X0 == FALSE) {NextState = DbExpectHigh;}
			else {
				if (X1 == TRUE) {NextState = DbExpectLow;}
				else {NextState = DbValidateHigh;}
			}
		break;
		case DbExpectLow:
			if (X0 == FALSE) {NextState = DbValidateLow;}
			else {NextState = DbExpectLow;}
		break;
		case DbValidateLow:
			if (X0 == TRUE) {NextState = DbExpectLow;}
			else {
				if (X1 == TRUE) {NextState = DbExpectHigh;}
				else {NextState = DbValidateLow;}
			}
		break;
		default: NextState = DbExpectHigh;
	}
	
	// Perform the output function based on the inputs and current state.
	switch (Switch->CurrentState) {
		case DbExpectHigh: 
			if (X0 == TRUE) { Switch->EventTime = g1msTimer; }
			DebouncedSwitchStatus = Low;
		break;
		case DbValidateHigh:
			if ((X1 == TRUE) && (X0 == TRUE)) {DebouncedSwitchStatus = High;}
			else {DebouncedSwitchStatus = Low;}
		break;
		case DbExpectLow: 
			if (X0 == FALSE) { Switch->EventTime = g1msTimer; }
			DebouncedSwitchStatus = High;
		break;
		case DbValidateLow:
			if ((X1 == TRUE) && (X0 == FALSE)) {DebouncedSwitchStatus = Low;}
			else {DebouncedSwitchStatus = High;}
		break;
//		default: DebouncedSwitchStatus = Low;
	}
	
	// Finally, update the current state.
	Switch->CurrentState = NextState;

	return DebouncedSwitchStatus;
}

