/*
 * TMC5130.c
 *
 *  Created on: 25.01.2017
 *      Author: BS
 */

#include "TMC5130.h"
#include "bsp_tmc5130.h"

// => Shared variables todo: decide if these variables go here (API) or in Evalboard/Module
extern int tmc5130_VMax;
extern u8 tmc5130_VMaxModified;
extern int tmc5130_AMax;
extern u8 tmc5130_AMaxModified;
// <= Shared variables

// => SPI wrapper
extern int tmc5130_spi_readInt(uint8 address);
extern void tmc5130_spi_writeInt(uint8 address, int32 value);
//extern u16 tmc5130_spi_readRegister16BitValue(uint8 address, uint8 channel);
//extern void tmc5130_spi_writeRegister16BitValue(uint8 address, uint8 channel, uint16 value);
// <= SPI wrapper

#define VEL_FACTOR 0.953674316406         //fClk/2 / 2^23   (fClk=16MHz)
#define ACC_FACTOR 116.415321827          //fClk^2 / (512*256) / 2^24   (fClk=16MHz)

// todo: decide if the shadow register is done here (API) or in Evalboard/Module (1)
s32 tmc5130_shadowRegister[128];
static uint8 tmc5130_registerAccess[128]=
{
	// access
	// none :	0
	// r 	:	1
	// w 	: 	2
	// rw 	: 	3
	// r/w 	: 	7

	//	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
		3, 1, 1, 2, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    	//00..0f
		2, 2, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    	//10..1f

		3, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 0, 0,    	//20..2f
		0, 0, 0, 2, 3, 1, 0, 0, 3, 3, 2, 1, 1, 0, 0, 0,    	//30..3f
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    	//40..2f
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    	//50..3f

		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 3, 2, 2, 1,    	//60..6f
		2, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0		//70..7f
};

/*******************************************************************
   Function: tmc5130_writeInt()
   Parameter: Axis: Axis Number
              Address: Register Address
              Value: integer to be written

   Returns: ---

   Purpose: Write a 32-bit value into a TMC5130-Register.
 ********************************************************************/
void tmc5130_writeInt(uint8 Address, int32 Value)
{
	tmc5130_spi_writeInt(Address, Value);

	// todo: decide if the shadow register is done here (API) or in Evalboard/Module (2)
	tmc5130_shadowRegister[Address & 0x7f] = Value;
}

/*******************************************************************
   Function: tmc5130_readInt()
   Parameter: Axis: Axis Number
              Address: Register Address

   Returns: Register value

   Purpose: Read a 32-bit value from a TMC5130-Register.
 ********************************************************************/
int32 tmc5130_readInt(uint8 Address)
{
	int32 Value;
	Address &= 0x7f;
	// todo: decide if the shadow register is done here (API) or in Evalboard/Module (3)
//	if(!(tmc5130_registerAccess[Address] & 1) && !(tmc5130_registerAccess[Address] & 4))
//		return tmc5130_shadowRegister[Address];	// register not readable -> software copy

	Value = tmc5130_spi_readInt(Address);
    // VACTUAL register has only 24 bits => correct sign bits
    if((Address==0x22 || Address==0x42)	&& (Value & BIT23)) Value |= 0xff000000;

	return Value;
}

/*******************************************************************
   Function: tmc5130_init()
   Parameter: ---

   Returns: ---

   Purpose: Initializes registers of TMC5130
 ********************************************************************/
void tmc5130_init(void)
{
	tmc5130_writeInt(TMC5130_CHOPCONF, 0x0100C5);
	tmc5130_writeInt(TMC5130_IHOLD_IRUN, 0x061001);
	tmc5130_writeInt(TMC5130_TZEROWAIT, 0x0A);
	tmc5130_writeInt(TMC5130_GCONF, 0x04);
	tmc5130_writeInt(TMC5130_TPWMTHRS, 0x01f4);
	//Standard values for chopper
	tmc5130_writeInt(TMC5130_PWMCONF, 0x0401C8);

	//Reset position
	
	tmc5130_writeInt(TMC5130_XTARGET, 0);
	tmc5130_writeInt(TMC5130_XACTUAL, 0);

	//Standard values for speed and acceleration
	tmc5130_writeInt(TMC5130_A1, 0x03E8);
	tmc5130_writeInt(TMC5130_V1, 0xC350);
	tmc5130_writeInt(TMC5130_AMAX, 0x01F4);
	tmc5130_writeInt(TMC5130_VMAX, 0x030D40);
	tmc5130_writeInt(TMC5130_DMAX, 0x02BC);
	tmc5130_writeInt(TMC5130_D1, 0x578);
	tmc5130_writeInt(TMC5130_VSTOP, 0x0A);
	tmc5130_writeInt(TMC5130_RAMPMODE, TMC5130_MODE_POSITION);

};

/*******************************************************************
   Function: tmc5130_rotate()
   Parameter: velocity: Rotation speed

   Returns: ---

   Purpose: Rotate TMC5130 Motor
 ********************************************************************/
void tmc5130_rotate(int32 velocity)
{
	if(tmc5130_AMaxModified)
	{
		tmc5130_writeInt(TMC5130_AMAX, tmc5130_AMax);
		tmc5130_AMaxModified=FALSE;
	}
	tmc5130_VMaxModified=TRUE;
	if(velocity >= 0)
	{
		tmc5130_writeInt(TMC5130_RAMPMODE, TMC5130_MODE_VELPOS);
	}
	else
	{
		tmc5130_writeInt(TMC5130_RAMPMODE, TMC5130_MODE_VELNEG);
		velocity = - velocity;
	}
	tmc5130_writeInt(TMC5130_VMAX, velocity);
}

/*******************************************************************
   Function: tmc5130_motorStop()
   Parameter: ---

   Returns: ---

   Purpose: Stop TMC5130 Motor
 ********************************************************************/
void tmc5130_motorStop()
{
	tmc5130_VMaxModified=TRUE;
	tmc5130_writeInt(TMC5130_VMAX, 0);
	tmc5130_writeInt(TMC5130_RAMPMODE, TMC5130_MODE_VELPOS);
}

/*******************************************************************
   Function: tmc5130_hardStop()
   Parameter: ---
   Returns: ---

   Purpose: Stop a motor immediately
 ********************************************************************/
void tmc5130_hardStop()
{
	tmc5130_VMaxModified=TRUE;
	tmc5130_AMaxModified=TRUE;
	tmc5130_writeInt(TMC5130_VMAX, 0);
	tmc5130_writeInt(TMC5130_AMAX, 65535);
	tmc5130_writeInt(TMC5130_RAMPMODE, TMC5130_MODE_VELPOS);
}

/*******************************************************************
   Function: tmc5130_moveToAbsolutePosition()
   Parameter: position: target position
              velocity: rotation speed

   Returns: ---

   Purpose: Move TMC5130 Motor to a certain position
 ********************************************************************/
void tmc5130_moveToAbsolutePosition(int32 position, int32 velocity)
{
	if(tmc5130_AMaxModified)
	{
		tmc5130_writeInt(TMC5130_AMAX, tmc5130_AMax);
		tmc5130_AMaxModified = FALSE;
	}
	if(tmc5130_VMaxModified)
	{
		tmc5130_writeInt(TMC5130_VMAX, velocity);
		tmc5130_VMaxModified = FALSE;
	}
	tmc5130_writeInt(TMC5130_XTARGET, position);
	tmc5130_writeInt(TMC5130_RAMPMODE, TMC5130_MODE_POSITION);
}

/*******************************************************************
   Function: tmc5130_moveToRelativePosition()
   Parameter: position: target position
              velocity: rotation speed

   Returns: ---

   Purpose: Increase or decrease TMC5130 position a certain value
 ********************************************************************/
void tmc5130_moveToRelativePosition(int32 position, int32 velocity, int32 reference)
{
	if(tmc5130_AMaxModified)
	{
		tmc5130_writeInt(TMC5130_AMAX, tmc5130_AMax);
		tmc5130_AMaxModified = FALSE;
	}
	if(tmc5130_VMaxModified)
	{
		tmc5130_writeInt(TMC5130_VMAX, velocity);
		tmc5130_VMaxModified = FALSE;
	}
	tmc5130_writeInt(TMC5130_XTARGET, reference+position);
	tmc5130_writeInt(TMC5130_RAMPMODE, TMC5130_MODE_POSITION);
}

/*******************************************************************
  Set and get functions for each setting of the IHOLD_IRUN register
********************************************************************/
void tmc5130_setIHold(uint8 current)
{
	u32 Value=tmc5130_readInt(TMC5130_IHOLD_IRUN) & 0xFFFE0;
	tmc5130_writeInt(TMC5130_IHOLD_IRUN, Value | ((current & 0x1F) << 0));
}

void tmc5130_setIRun(uint8 current)
{
	u32 Value=tmc5130_readInt(TMC5130_IHOLD_IRUN) & 0xFE0FF;
	tmc5130_writeInt(TMC5130_IHOLD_IRUN, Value | ((current & 0x1F) << 8));
}

void tmc5130_setIHoldDelay(uint8 delay)
{
	u32 Value=tmc5130_readInt(TMC5130_IHOLD_IRUN) & 0xFFFF;
	tmc5130_writeInt(TMC5130_IHOLD_IRUN, Value | ((delay & 0xF) << 16));
}

uint8 tmc5130_getIHold(void)
{
  return (tmc5130_readInt(TMC5130_CHOPCONF) >> 0) & 0x1F;
}

uint8 tmc5130_getIRun(void)
{
  return (tmc5130_readInt(TMC5130_CHOPCONF) >> 8) & 0x1F;
}

uint8 tmc5130_getIHoldDelay(void)
{
  return (tmc5130_readInt(TMC5130_CHOPCONF) >> 16) & 0xF;
}

/*******************************************************************
  Set and get functions for each setting of the CHOPCONF register
********************************************************************/
void tmc5130_setStealthChop(uint8 Enable)
{
	u32 Value=tmc5130_readInt(TMC5130_GCONF) & 0x3FFFB;
	tmc5130_writeInt(TMC5130_GCONF, Value | ((Enable & 0x01) << 2));
}

uint8 tmc5130_getStealthChop(void)
{
  return (tmc5130_readInt(TMC5130_GCONF) >> 2) & 0x1;
}

void tmc5130_setChopperMStepRes(uint8 MRes)
{
	u32 Value;

	Value=tmc5130_readInt(TMC5130_CHOPCONF) & 0xf0ffffff;
	tmc5130_writeInt(TMC5130_CHOPCONF, Value | ((MRes & 0x0f) << 24));
}

uint8 tmc5130_getChopperMStepRes(void)
{
  return (tmc5130_readInt(TMC5130_CHOPCONF) >> 24) & 0x0f;
}

void tmc5130_setChopperMStepInterpolation(uint8 Enable)
{
	u32 Value;

	Value=tmc5130_readInt(TMC5130_CHOPCONF) & 0xefffffff;
	tmc5130_writeInt(TMC5130_CHOPCONF, Value | ((Enable & 0x01) << 28));
}

uint8 tmc5130_getChopperMStepInterpolation(void)
{
  return (tmc5130_readInt(TMC5130_CHOPCONF) >> 28) & 0x01;
}

void tmc5130_setChopperVSenseMode(uint8 Mode)
{
	u32 Value;

	Value=tmc5130_readInt(TMC5130_CHOPCONF);

	if(Mode)
		Value|=BIT17;
	else
		Value&= ~BIT17;

	tmc5130_writeInt(TMC5130_CHOPCONF, Value);
}

uint8 tmc5130_getChopperVSenseMode(void)
{
  return tmc5130_readInt(TMC5130_CHOPCONF) & BIT17 ? 1:0;
}

void tmc5130_setChopperTOff(uint8 TOff)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF) & 0xfffffff0;
  tmc5130_writeInt(TMC5130_CHOPCONF, Value | (TOff & 0x0f));
}

void tmc5130_setChopperHysteresisStart(uint8 HysteresisStart)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF) & 0xffffff8f;
  tmc5130_writeInt(TMC5130_CHOPCONF, Value | ((HysteresisStart & 0x07) << 4));
}

void tmc5130_setChopperHysteresisEnd(uint8 HysteresisEnd)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF) & 0xfffff87f;
  tmc5130_writeInt(TMC5130_CHOPCONF, Value | ((HysteresisEnd & 0x0f) << 7));
}

void tmc5130_setChopperBlankTime(uint8 BlankTime)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF) & 0xfffe7fff;
  tmc5130_writeInt(TMC5130_CHOPCONF, Value | ((BlankTime & 0x03) << 15));
}

void tmc5130_setChopperSync(uint8 Sync)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF) & 0xff0fffff;
  tmc5130_writeInt(TMC5130_CHOPCONF, Value | ((Sync & 0x0f) << 20));
}

void tmc5130_setChopperDisableShortToGround(uint8 Disable)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF);
  if(Disable)
    Value|=BIT30;
  else
    Value&= ~BIT30;

  tmc5130_writeInt(TMC5130_CHOPCONF, Value);
}

void tmc5130_setChopperVHighChm(uint8 VHighChm)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF);
  if(VHighChm)
    Value|=BIT19;
  else
    Value&= ~BIT19;

  tmc5130_writeInt(TMC5130_CHOPCONF, Value);
}

void tmc5130_setChopperVHighFs(uint8 VHighFs)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF);
  if(VHighFs)
    Value|=BIT18;
  else
    Value&= ~BIT18;

  tmc5130_writeInt(TMC5130_CHOPCONF, Value);
}

void tmc5130_setChopperConstantTOffMode(uint8 ConstantTOff)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF);
  if(ConstantTOff)
    Value|=BIT14;
  else
    Value&= ~BIT14;

  tmc5130_writeInt(TMC5130_CHOPCONF, Value);
}

void tmc5130_setChopperRandomTOff(uint8 RandomTOff)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF);
  if(RandomTOff)
    Value|=BIT13;
  else
    Value&= ~BIT13;

  tmc5130_writeInt(TMC5130_CHOPCONF, Value);
}

void tmc5130_setChopperDisableFastDecayComp(uint8 Disable)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF);
  if(Disable)
    Value|=BIT12;
  else
    Value&= ~BIT12;

  tmc5130_writeInt(TMC5130_CHOPCONF, Value);
}

void tmc5130_setChopperFastDecayTime(uint8 Time)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF) & 0xffffff8f;

  if(Time & BIT3)
    Value|=BIT11;
  else
    Value&= ~BIT11;

  tmc5130_writeInt(TMC5130_CHOPCONF, Value | ((Time & 0x07) << 4));
}

void tmc5130_setChopperSineWaveOffset(uint8 Offset)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_CHOPCONF) & 0xfffff87f;
  tmc5130_writeInt(TMC5130_CHOPCONF, Value | ((Offset & 0x0f) << 7));
}

uint8 tmc5130_getChopperTOff(void)
{
  return tmc5130_readInt(TMC5130_CHOPCONF) & 0x0000000f;
}

uint8 tmc5130_getChopperHysteresisStart(void)
{
  return (tmc5130_readInt(TMC5130_CHOPCONF) >> 4) & 0x07;
}

uint8 tmc5130_getChopperHysteresisEnd(void)
{
  return (tmc5130_readInt(TMC5130_CHOPCONF) >> 7) & 0x0f;
}

uint8 tmc5130_getChopperBlankTime(void)
{
  return (tmc5130_readInt(TMC5130_CHOPCONF) >> 15) & 0x03;
}

uint8 tmc5130_getChopperSync(void)
{
  return (tmc5130_readInt(TMC5130_CHOPCONF) >> 20) & 0x0f;
}

uint8 tmc5130_getChopperDisableShortToGround(void)
{
  return tmc5130_readInt(TMC5130_CHOPCONF) & BIT30 ? 1:0;
}

uint8 tmc5130_getChopperVHighChm(void)
{
  return tmc5130_readInt(TMC5130_CHOPCONF) & BIT19 ? 1:0;
}

uint8 tmc5130_getChopperVHighFs(void)
{
  return tmc5130_readInt(TMC5130_CHOPCONF) & BIT18 ? 1:0;
}

uint8 tmc5130_getChopperConstantTOffMode(void)
{
  return tmc5130_readInt(TMC5130_CHOPCONF) & BIT14 ? 1:0;
}

uint8 tmc5130_getChopperRandomTOff(void)
{
  return tmc5130_readInt(TMC5130_CHOPCONF) & BIT13 ? 1:0;
}

uint8 tmc5130_getChopperDisableFastDecayComp(void)
{
  return tmc5130_readInt(TMC5130_CHOPCONF) & BIT12 ? 1:0;
}

uint8 tmc5130_getChopperFastDecayTime(void)
{
  u32 Value;
  uint8 Time;

  Value=tmc5130_readInt(TMC5130_CHOPCONF);
  Time=(Value >> 4) & 0x07;
  if(Value & BIT11) Time|=BIT3;

  return Time;
}

uint8 tmc5130_getChopperSineWaveOffset(void)
{
  return (tmc5130_readInt(TMC5130_CHOPCONF) >> 7) & 0x0f;
}

/*******************************************************************
  Set and get functions for each setting of the COOLCONF register
********************************************************************/
void tmc5130_setSmartEnergyUpStep(uint8 UpStep)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_COOLCONF) & 0xffffff9f;
  tmc5130_writeInt(TMC5130_COOLCONF, Value | ((UpStep & 0x03) << 5));
}

void tmc5130_setSmartEnergyDownStep(uint8 DownStep)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_COOLCONF) & 0xffff9fff;
  tmc5130_writeInt(TMC5130_COOLCONF, Value | ((DownStep & 0x03) << 13));
}

void tmc5130_setSmartEnergyStallLevelMax(uint8 Max)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_COOLCONF) & 0xfffff0ff;
  tmc5130_writeInt(TMC5130_COOLCONF, Value | ((Max & 0x0f) << 8));
}

void tmc5130_setSmartEnergyStallLevelMin(uint8 Min)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_COOLCONF) & 0xfffffff0;
  tmc5130_writeInt(TMC5130_COOLCONF, Value | (Min & 0x0f));
}

void tmc5130_setSmartEnergyStallThreshold(int8 Threshold)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_COOLCONF) & 0xff00ffff;
  tmc5130_writeInt(TMC5130_COOLCONF, Value | ((Threshold & 0xff) << 16));
}

void tmc5130_setSmartEnergyIMin(uint8 IMin)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_COOLCONF);
  if(IMin)
    Value|=BIT15;
  else
    Value&= ~BIT15;

  tmc5130_writeInt(TMC5130_COOLCONF, Value);
}

void tmc5130_setSmartEnergyFilter(uint8 Filter)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_COOLCONF);
  if(Filter)
    Value|=BIT24;
  else
    Value&= ~BIT24;

  tmc5130_writeInt(TMC5130_COOLCONF, Value);
}


uint8 tmc5130_getSmartEnergyUpStep(void)
{
  return (tmc5130_readInt(TMC5130_COOLCONF) >> 5) & 0x03;
}

uint8 tmc5130_getSmartEnergyDownStep(void)
{
  return (tmc5130_readInt(TMC5130_COOLCONF) >> 13) & 0x03;
}

uint8 tmc5130_getSmartEnergyStallLevelMax(void)
{
  return (tmc5130_readInt(TMC5130_COOLCONF) >> 8) & 0x0f;
}

uint8 tmc5130_getSmartEnergyStallLevelMin(void)
{
  return tmc5130_readInt(TMC5130_COOLCONF) & 0x0f;
}

int32 tmc5130_getSmartEnergyStallThreshold(void)
{
  int32 Value;

  Value=(tmc5130_readInt(TMC5130_COOLCONF) >> 16) & 0xff;
  if(Value & BIT7) Value|=0xffffff00;

  return Value;
}

uint8 tmc5130_getSmartEnergyIMin(void)
{
  if(tmc5130_readInt(TMC5130_COOLCONF) & BIT15)
    return 1;
  else
    return 0;
}

uint8 tmc5130_getSmartEnergyFilter(void)
{
  if(tmc5130_readInt(TMC5130_COOLCONF) & BIT24)
    return 1;
  else
    return 0;
}

/*******************************************************************
  Set and get functions for each setting of the PWMCONF register
********************************************************************/
void tmc5130_setPWMFreewheelMode(uint8 Mode)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_PWMCONF) & 0xffcfffff;
  Mode&=0x03;
  Value|=Mode << 20;
  tmc5130_writeInt(TMC5130_PWMCONF, Value);
}

void tmc5130_setPWMSymmetric(uint8 Symmetric)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_PWMCONF);
  if(Symmetric)
    Value|=BIT19;
  else
    Value&= ~BIT19;
  tmc5130_writeInt(TMC5130_PWMCONF, Value);
}

void tmc5130_setPWMAutoscale(uint8 Autoscale)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_PWMCONF);
  if(Autoscale)
    Value|=BIT18;
  else
    Value&= ~BIT18;
  tmc5130_writeInt(TMC5130_PWMCONF, Value);
}

void tmc5130_setPWMFrequency(uint8 Frequency)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_PWMCONF) & 0xfffcffff;
  Frequency&=0x03;
  Value|=Frequency << 16;
  tmc5130_writeInt(TMC5130_PWMCONF, Value);
}

void tmc5130_setPWMGrad(uint8 PWMGrad)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_PWMCONF) & 0xffff00ff;
  Value|=PWMGrad << 8;
  tmc5130_writeInt(TMC5130_PWMCONF, Value);
}

void tmc5130_setPWMAmpl(uint8 PWMAmpl)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_PWMCONF) & 0xffffff00;
  Value|=PWMAmpl;
  tmc5130_writeInt(TMC5130_PWMCONF, Value);
}


uint8 tmc5130_getPWMFreewheelMode(void)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_PWMCONF);
  Value>>=20;

  return Value & 0x03;
}

uint8 tmc5130_getPWMSymmetric(void)
{
  if(tmc5130_readInt(TMC5130_PWMCONF) & BIT19)
    return TRUE;
  else
    return FALSE;
}

uint8 tmc5130_getPWMAutoscale(void)
{
  if(tmc5130_readInt(TMC5130_PWMCONF) & BIT18)
    return TRUE;
  else
    return FALSE;
}

uint8 tmc5130_getPWMFrequency(void)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_PWMCONF);
  Value>>=16;

  return Value & 0x03;
}

uint8 tmc5130_getPWMGrad(void)
{
  u32 Value;

  Value=tmc5130_readInt(TMC5130_PWMCONF);
  Value>>=8;

  return Value & 0xff;
}

uint8 tmc5130_getPWMAmpl(void)
{
   return tmc5130_readInt(TMC5130_PWMCONF) & 0xff;
}
