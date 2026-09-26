#pragma once

inline long FloatToFixed16_16(float Value)
{
	return (long)(Value * 65536.0f + 32768.0f);
}

inline short Fixed16_16ToShort(long Value)
{
	return (short)(Value >> 16);
}
