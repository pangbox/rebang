#pragma once

#include "actor.h"

class Wobb;

class CAmbientSound : public IActor
{
public:
	virtual ~CAmbientSound() { }
	DECLARE_OBJECT(CAmbientSound)

protected:
	CAmbientSound();

public:
	virtual void OnInit();
	virtual void OnProcess(float delta);

protected:
	virtual void HandleMsg(const MsgObject& msg);
	float GetVolume(const Wobb& obb, float distance);
};
