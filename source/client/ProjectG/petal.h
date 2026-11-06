#pragma once

#include <string>
#include "weather.h"

class CPetBody;

class ntFlutter
{
public:
	ntFlutter(int num, const std::string& petName, int type);
	~ntFlutter();

	virtual void Process(float dt, float gravity);
	virtual void Display();
	virtual void DisplayFullScreenOverlay();
	virtual void Reset(int num, float rate);

	struct sFrag
	{
		WVector pos;
		WVector center;
		unsigned long unknown18;
		WVector offset;
		float yaw;
		float swing;
		bool bVisible;
	};

private:
	void ReleasePets();

	static const int MAX_FLUTTER_PET;

	float m_rate;
	Waabb m_area;
	sFrag* m_frag;
	CPetBody* m_pet[10];
	int m_num;
	const std::string& m_petName;
};

template <class T>
class ntImplFlutter : public CWeatherType
{
public:
	ntImplFlutter(int num, const char* name, int type);
	virtual ~ntImplFlutter();
	virtual void Process(float delta);
	virtual void Display();
	virtual void DisplayFullScreenOverlay();
	virtual void Reset(int type, float value);

private:
	T m_holder;
	ntFlutter m_flutter;
};

template <class T>
ntImplFlutter<T>::ntImplFlutter(int num, const char* name, int type)
	: m_flutter(num, m_holder.m_petName, type)
{
}

template <class T>
ntImplFlutter<T>::~ntImplFlutter()
{
}

template <class T>
void ntImplFlutter<T>::Process(float delta)
{
	m_flutter.Process(delta, m_gravity);
}

template <class T>
void ntImplFlutter<T>::Display()
{
	m_flutter.Display();
}

template <class T>
void ntImplFlutter<T>::DisplayFullScreenOverlay()
{
	m_flutter.DisplayFullScreenOverlay();
}

template <class T>
void ntImplFlutter<T>::Reset(int type, float value)
{
	m_flutter.Reset(type, value);
}

struct sPetalNameHolder
{
	sPetalNameHolder()
		: m_petName("pinkflower%02d.pet")
	{
	}
	std::string m_petName;
};

struct sSilicaParticleNameHolder
{
	sSilicaParticleNameHolder()
		: m_petName("Hell_powder%02d.pet")
	{
	}
	std::string m_petName;
};
