/*
 * TMC5130.h
 *
 *  Created on: 25.01.2017
 *      Author: BS
 */

#ifndef API_IC_TMC5130_H
#define API_IC_TMC5130_H

	//#include "../../helpers/API_Header.h"
	#include "TMC5130_Register.h"
	#include "TypeDefs.h"
	#include "Bits.h"
	//Factor between 10ms units and internal units for 16MHz
	#define TPOWERDOWN_FACTOR (4.17792*100.0/255.0)
	// TPOWERDOWN_FACTOR = k * 100 / 255 where k = 2^18 * 255 / fClk for fClk = 16000000)

	void tmc5130_writeInt(uint8 Address, int32 Value);
	int32 tmc5130_readInt(uint8 Address);
	void tmc5130_init(void);
	void tmc5130_rotate(int32 velocity);
	void tmc5130_motorStop(void);
	void tmc5130_hardStop(void);
	void tmc5130_moveToAbsolutePosition(int32 position, int32 velocity);
	void tmc5130_moveToRelativePosition(int32 position, int32 velocity, int32 reference);

	// IHold IRun
	void tmc5130_setIHold(uint8 current);
	void tmc5130_setIRun(uint8 current);
	void tmc5130_setIHoldDelay(uint8 delay);
	uint8 tmc5130_getIHold(void);
	uint8 tmc5130_getIRun(void);
	uint8 tmc5130_getIHoldDelay(void);

	// Chopper Conf
	void tmc5130_setStealthChop(uint8 Enable);
	void tmc5130_setChopperMStepRes(uint8 MRes);
	void tmc5130_setChopperMStepInterpolation(uint8 Enable);
	void tmc5130_setChopperVSenseMode(uint8 Mode);
	uint8 tmc5130_getStealthChop(void);
	uint8 tmc5130_getChopperMStepRes(void);
	uint8 tmc5130_getChopperMStepInterpolation(void);
	uint8 tmc5130_getChopperVSenseMode(void);

	void tmc5130_setChopperTOff(uint8 TOff);
	void tmc5130_setChopperHysteresisStart(uint8 HysteresisStart);
	void tmc5130_setChopperHysteresisEnd(uint8 HysteresisEnd);
	void tmc5130_setChopperBlankTime(uint8 BlankTime);
	void tmc5130_setChopperSync(uint8 Sync);
	void tmc5130_setChopperDisableShortToGround(uint8 Disable);
	void tmc5130_setChopperVHighChm(uint8 VHighChm);
	void tmc5130_setChopperVHighFs(uint8 VHighFs);
	void tmc5130_setChopperConstantTOffMode(uint8 ConstantTOff);
	void tmc5130_setChopperRandomTOff(uint8 RandomTOff);
	void tmc5130_setChopperDisableFastDecayComp(uint8 Disable);
	void tmc5130_setChopperFastDecayTime(uint8 Time);
	void tmc5130_setChopperSineWaveOffset(uint8 Offset);
	uint8 tmc5130_getChopperTOff(void);
	uint8 tmc5130_getChopperHysteresisStart(void);
	uint8 tmc5130_getChopperHysteresisEnd(void);
	uint8 tmc5130_getChopperBlankTime(void);
	uint8 tmc5130_getChopperSync(void);
	uint8 tmc5130_getChopperDisableShortToGround(void);
	uint8 tmc5130_getChopperVHighChm(void);
	uint8 tmc5130_getChopperVHighFs(void);
	uint8 tmc5130_getChopperConstantTOffMode(void);
	uint8 tmc5130_getChopperRandomTOff(void);
	uint8 tmc5130_getChopperDisableFastDecayComp(void);
	uint8 tmc5130_getChopperFastDecayTime(void);
	uint8 tmc5130_getChopperSineWaveOffset(void);

	// Coolstep Conf
	void tmc5130_setSmartEnergyUpStep(uint8 UpStep);
	void tmc5130_setSmartEnergyDownStep(uint8 DownStep);
	void tmc5130_setSmartEnergyStallLevelMax(uint8 Max);
	void tmc5130_setSmartEnergyStallLevelMin(uint8 Min);
	void tmc5130_setSmartEnergyStallThreshold(int8 Threshold);
	void tmc5130_setSmartEnergyIMin(uint8 IMin);
	void tmc5130_setSmartEnergyFilter(uint8 Filter);
	uint8 tmc5130_getSmartEnergyUpStep(void);
	uint8 tmc5130_getSmartEnergyDownStep(void);
	uint8 tmc5130_getSmartEnergyStallLevelMax(void);
	uint8 tmc5130_getSmartEnergyStallLevelMin(void);
	int32 tmc5130_getSmartEnergyStallThreshold(void);
	uint8 tmc5130_getSmartEnergyIMin(void);
	uint8 tmc5130_getSmartEnergyFilter(void);

	// PWMConf
	void tmc5130_setPWMFreewheelMode(uint8 Mode);
	void tmc5130_setPWMSymmetric(uint8 Symmetric);
	void tmc5130_setPWMAutoscale(uint8 Autoscale);
	void tmc5130_setPWMFrequency(uint8 Frequency);
	void tmc5130_setPWMGrad(uint8 PWMGrad);
	void tmc5130_setPWMAmpl(uint8 PWMAmpl);
	uint8 tmc5130_getPWMFreewheelMode(void);
	uint8 tmc5130_getPWMSymmetric(void);
	uint8 tmc5130_getPWMAutoscale(void);
	uint8 tmc5130_getPWMFrequency(void);
	uint8 tmc5130_getPWMGrad(void);
	uint8 tmc5130_getPWMAmpl(void);

#endif /* API_IC_TMC5130_H */
