#pragma once

class WPuppet
{
public:
	__forceinline const WSphere& GetBoundSphere() const
	{
		return m_boundSphere;
	}

	char unknown_000[0x10c];
	WSphere m_boundSphere;
};
