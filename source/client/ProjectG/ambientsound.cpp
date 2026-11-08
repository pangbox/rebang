#include "stdafx.h"
#include "polysoup.h"
#include "golfdoc.h"
#include "soundmanager.h"

extern WMatrix g_camera;

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

IMPLEMENT_ACTOR(CAmbientSound, IActor)

CAmbientSound::CAmbientSound()
{
	AddSkipList(0x85040);
}

void CAmbientSound::OnInit()
{
	std::list<sSoundBox>& boxes =
		CSharedDoc::Instance()->m_pGolfDoc->m_pPolySoup->m_soundBox;
	for (std::list<sSoundBox>::iterator it = boxes.begin(); it != boxes.end();
		++it)
	{
		if ((*it).interval < 0 && !g_audio->IsPlaying((*it).name))
			g_audio->PlaySfx((*it).name, NULL, 1, (*it).name);
	}
}

float CAmbientSound::GetVolume(const Wobb& obb, float distance)
{
	if (obb.IsInclude(g_camera.pivot) || distance < 0)
		return 1.0f;
	float squaredDistance = 0;
	for (int i = 0; i < 3; ++i)
	{
		const WVector& axis = obb.extend[i];
		float extent = axis.Magnitude() * 0.5f;
		float projection =
			((g_camera.pivot - obb.center) * 0.5f * axis) / extent;
		if (projection < extent * -1.0f)
			squaredDistance += (extent + projection) * (extent + projection);
		else if (projection > extent)
			squaredDistance += (projection - extent) * (projection - extent);
	}
	float volume = 1.0f - sqrtf(squaredDistance) / (distance * 3.2f);
	return Max(0.0f, volume);
}

void CAmbientSound::OnProcess(float delta)
{
	CGolfDoc* doc = CSharedDoc::Instance()->m_pGolfDoc;
	std::list<sSoundBox>& boxes = doc->m_pPolySoup->m_soundBox;
	std::map<std::string, float> volumes;
	volumes.clear();
	for (std::list<sSoundBox>::iterator it = boxes.begin(); it != boxes.end();
		++it)
	{
		float volume;
		if ((*it).interval > 0)
		{
			(*it).time += delta;
			if ((*it).time > (*it).interval)
			{
				(*it).time = 0;
				g_audio->PlaySfx((*it).name);
			}
			if (g_audio->IsPlaying((*it).name))
				volume = GetVolume((*it).obb, (*it).distance);
			else
				volume = 0;
		}
		else
			volume = GetVolume((*it).obb, (*it).distance);
		std::map<std::string, float>::iterator found;
		found = volumes.find((*it).name);
		if (found == volumes.end())
			volumes[(*it).name] = volume;
		else
			volumes[(*it).name] = Max(volumes[(*it).name], volume);
	}
	for (std::map<std::string, float>::iterator it = volumes.begin();
		it != volumes.end(); ++it)
	{
		if (g_audio->IsPlaying((*it).first.c_str()))
			g_audio->SetSFXVolumeWhilePlaying((*it).first.c_str(),
				(*it).second);
	}
}

void CAmbientSound::HandleMsg(const MsgObject& msg)
{
	if (msg.message == 500)
	{
		std::list<sSoundBox>& boxes =
			CSharedDoc::Instance()->m_pGolfDoc->m_pPolySoup->m_soundBox;
		for (std::list<sSoundBox>::iterator it = boxes.begin();
			it != boxes.end(); ++it)
			g_audio->StopSfx((*it).name);
	}
}
