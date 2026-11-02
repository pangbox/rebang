#pragma once

#include "actor.h"
#include "scenemanager.h"
#include "wmath.h"
#include "../../shared/classdefine.h"
#include "../../shared/globalgamedefine.h"

#include <list>
#include <map>
#include <string>
#include <vector>

class CCaddieTalk;
class CPetBody;
class CRing;
class IObject;

struct sTalk
{
	const char* m_name;
	bool m_once;
};

extern sTalk talkTable[];

class CGolfCaddie : public IActor, public CRenderFuncPtr
{
public:
	virtual ~CGolfCaddie(void) { }

	struct sBehavior
	{
		unsigned char m_level;
		std::string m_motion;
	};

	struct sCaddie
	{
		sCaddie(void)
			: m_pIffCaddie(0), m_pInfo(0)
		{
		}

		CPetBody* m_pBody;
		int m_unknown4;
		int m_unknown8;
		int m_unknownC;
		IFF_STRUCT::sCaddie* m_pIffCaddie;
		sCaddieInfo* m_pInfo;
		std::map<std::string, sBehavior> m_talks;
		std::map<std::string, sBehavior> m_motions;
		float m_stopDist;
		float m_startDelay;
		float m_petSpeed;
		bool m_showRing;
		bool m_caddieView;
	};

	struct sFindCaddie
	{
		sFindCaddie(unsigned long typeId, unsigned long cadItemTypeId)
			: m_typeId(typeId), m_cadItemTypeId(cadItemTypeId)
		{
		}

		bool operator()(sCaddie* caddie)
		{
			if (!caddie->m_pInfo)
				return caddie->m_pIffCaddie->c.TypeId == m_typeId;

			return caddie->m_pInfo->tidPart == m_cadItemTypeId &&
				caddie->m_pIffCaddie->c.TypeId == m_typeId;
		}

		unsigned long m_typeId;
		unsigned long m_cadItemTypeId;
	};

	DECLARE_OBJECT(CGolfCaddie)

	virtual void OnPreLoadInit(void);
	virtual void OnLoad(void);
	virtual void OnInit(void);
	virtual void OnReload(void);
	virtual void OnProcess(float delta);
	virtual void Display(void);
	virtual void DisplayShadow(void);
	virtual void OnDestroy(void);
	virtual void HandleMsg(const MsgObject& msg);

protected:
	CGolfCaddie(void)
	{
		m_pRing = 0;
		m_pCaddieTalk = 0;
		m_visible = true;
	}

	sCaddie* FindCaddie(unsigned long typeId, unsigned long cadItemTypeId);
	sCaddie* AddCaddie(unsigned long typeId, unsigned long cadItemTypeId,
		bool find);
	void LoadTalkText(sCaddie* caddie);
	void LoadMotionLevel(sCaddie* caddie);
	bool ReadAttribute(sCaddie* caddie);
	bool ShouldDisplay(void);
	bool IsGoodMotion(sCaddie* caddie, const char* motion);
	sBehavior* SetTalkToBubble(unsigned char type, bool chatOnly,
		unsigned char param);
	void SetBubble(unsigned char type, bool chatOnly, unsigned char param);
	void SetBubble(unsigned char type, const char* msg);
	void SetCaddieTalk_ChatOnly(unsigned char type);
	void ResetTalk(void);
	void LookUp(WVector* target, bool immediate);
	void DontLookUp(void);
	char* GetBehaviorToken(char* token, char* src);
	char* GetBehaviorEndToken(char* token, char* src);
	void UseSpecialAbility(void);
	void SearchGround(void);
	void Dodge(void);
	void Run(void);
	void TalkProcess(float delta);

	sCaddie* m_pCaddie;
	std::list<sCaddie*> m_caddies;
	int m_unknown2c;
	unsigned long m_unknown30;
	int m_runDust;
	WVector m_position;
	float m_runDelay;
	float m_angle;
	WVector m_destination;
	bool m_dodging;
	bool m_visible;
	float m_idleTime;
	int m_unknown60;
	unsigned char m_unknown64;
	float m_talkTime;
	float m_turbulenceTime;
	int m_fxScript;
	CRing* m_pRing;
	unsigned char m_bubbleTalkType;
	bool m_talked[36];
	bool m_talkFlag;
	unsigned long m_talkResetTime;
	CCaddieTalk* m_pCaddieTalk;
};

struct ICaddieTalk
{
public:
	virtual void Load(void) = 0;
	virtual bool FindMsg(void) = 0;

	virtual const char* GetTalkMsg(void) const { return 0; }

	virtual int GetPriority(void) { return 0; }

	virtual ~ICaddieTalk(void) { }
};

struct XmlCaddieTalk
{
public:
	struct parameters
	{
		void add(const std::string& value) { m_values.push_back(value); }

		std::vector<std::string> m_values;
	};

	struct attributes
	{
		const char* get(const std::string& name)
		{
			std::map<std::string, std::string>::iterator it =
				m_values.find(name);
			if (it != m_values.end())
				return it->second.c_str();
			return 0;
		}

		std::map<std::string, std::string> m_values;
	};

	virtual bool LoadSituation(const char* situation, const parameters& params,
		std::vector<attributes>& result);
};

class cCaddieTalkTeeShot : public XmlCaddieTalk, public ICaddieTalk
{
public:
	struct sTalkData
	{
		bool operator==(const sTalkData& other) const
		{
			if (m_caddie == -1)
				return m_course == other.m_course && m_hole == other.m_hole;

			return m_caddie == other.m_caddie && m_course == other.m_course &&
				m_hole == other.m_hole;
		}

		int m_caddie;
		int m_course;
		int m_hole;
		std::string m_msg;
	};

	cCaddieTalkTeeShot(void) { }

	virtual ~cCaddieTalkTeeShot(void) { }

	virtual void Load(void);
	virtual bool FindMsg(void);

	virtual const char* GetTalkMsg(void) const { return m_talkMsg.c_str(); }

	std::vector<sTalkData> m_talkData;
	std::string m_talkMsg;
};

class CCaddieTalk
{
public:
	~CCaddieTalk(void);

	void Init(void);
	void Release(void);
	const ICaddieTalk* FindTalk(void);

	void OnStop(void) { m_isTalking = 0; }

	std::list<ICaddieTalk*> m_talks;
	int m_isTalking;
};
