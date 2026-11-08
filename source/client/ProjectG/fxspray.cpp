#include "stdafx.h"
#include "fxbox.h"
#include "fxbox.inl"
#include "petbody.h"
#include "binstr.h"
#include "soundmanager.h"
#include "mathconsts.h"

CFxParticle::CFxParticle()
{
	sprite = NULL;
	pet = NULL;
	wind = 0.0f;
	volOpacity = 1.0f;
}

CFxParticle::~CFxParticle()
{
	if (pet)
	{
		delete pet;
		pet = NULL;
	}
}

static unsigned long MulColor(unsigned long color, unsigned long light)
{
	unsigned long r = ((color >> 16 & 0xff) * (light >> 16 & 0xff)) >> 8;
	unsigned long g = ((color >> 8 & 0xff) * (light >> 8 & 0xff)) >> 8;
	unsigned long b = ((color & 0xff) * (light & 0xff)) >> 8;

	return (((r << 8) + g) << 8) + b;
}

static unsigned long ScaleAlpha(unsigned long color, float rate)
{
	return color & 0xffffff |
		(unsigned char)(unsigned long)((color >> 24) * rate) << 24;
}

static void GetCoreBox(Waabb& core, const Waabb& genPos, bool bSphere,
	float coreX, float coreY)
{
	if (!bSphere)
	{
		if (coreX < 1.0f || coreY < 1.0f)
		{
			WVector center = (genPos.max + genPos.min) * 0.5f;
			WVector half = (genPos.max - genPos.min) * 0.5f;

			if (coreX < 1.0f)
			{
				core.min.x = (center.x - half.x) * coreX;
				core.min.z = (center.z - half.z) * coreX;
				core.max.x = (center.x + half.x) * coreX;
				core.max.z = (center.z + half.z) * coreX;
			}
			else
			{
				core.min.x = genPos.min.x;
				core.max.x = genPos.max.x;
				core.min.z = genPos.min.z;
				core.max.z = genPos.max.z;
			}

			if (coreY < 1.0f)
			{
				core.min.y = (center.y - half.y) * coreY;
				core.max.y = (center.y + half.y) * coreY;
			}
			else
			{
				core.min.y = genPos.min.y;
				core.max.y = genPos.max.y;
			}
		}
		else
			core = genPos;
	}
	else
	{
		core.min = genPos.min;
		core.max.x = coreX * genPos.max.y;
		core.max.y = coreY * genPos.max.y;
		core.max.z = 0.0f;
	}
}

static float GetVolumeOpacity(const WVector& pos, const Waabb& core,
	const Waabb& genPos, float coreX, float coreY, bool bSphere)
{
	float opacity = 1.0f;

	if (!bSphere)
	{
		if (coreX < 1.0f)
		{
			if (pos.x < core.min.x)
				opacity =
					1.0f - (pos.x - core.min.x) / (genPos.min.x - core.min.x);
			else if (core.max.x < pos.x)
				opacity =
					1.0f - (pos.x - core.max.x) / (genPos.max.x - core.max.x);

			if (pos.z < core.min.z)
				opacity *=
					1.0f - (pos.z - core.min.z) / (genPos.min.z - core.min.z);
			else if (core.max.z < pos.z)
				opacity *=
					1.0f - (pos.z - core.max.z) / (genPos.max.z - core.max.z);
		}

		if (coreY < 1.0f)
		{
			if (pos.y < core.min.y)
				opacity *=
					1.0f - (pos.y - core.min.y) / (genPos.min.y - core.min.y);
			else if (core.max.y < pos.y)
				opacity *=
					1.0f - (pos.y - core.max.y) / (genPos.max.y - core.max.y);
		}
	}
	else
	{
		float dist = (pos - genPos.min).Magnitude();

		if (dist <= core.max.x)
		{
			if (!WisZero(core.max.x, g_EPSILON))
				opacity = dist / core.max.x;
		}
		else if (core.max.y <= dist)
		{
			if (!WisZero(genPos.max.y - core.max.y, g_EPSILON))
				opacity =
					1.0f - (dist - core.max.y) / (genPos.max.y - core.max.y);
		}
	}

	return opacity;
}

static float GetAngle(const WVector& a, const WVector& b)
{
	float la = a.Magnitude();
	if (Wabs(la) < g_EPSILON)
		return 0.0f;

	float lb = b.Magnitude();
	if (Wabs(lb) < g_EPSILON)
		return 0.0f;

	return acosf(Between(-1.0f, (a * b) / (lb * la), 1.0f));
}

CFxSpray::CFxSpray(CFxBox& fxBox)
	: m_fxBox(fxBox)
{
	Reset();

	m_centerX = g_HUGE;
	m_centerY = g_HUGE;
	memset(m_attachMat, 0, sizeof(m_attachMat));
	memset(m_attachPos, 0, sizeof(m_attachPos));
}

CFxSpray::~CFxSpray()
{
	for (std::list<CFxParticle*>::iterator it = m_particle.begin();
		it != m_particle.end(); ++it)
		delete *it;

	m_particle.clear();
}

void CFxSpray::Reset()
{
	m_bMark = true;
	m_bActive = true;
	m_attacher = NULL;
	m_parent = NULL;
	m_time = 0.0f;

	m_pos = WVector::ZERO;
	m_vel = WVector::ZERO;
	m_rot.Reset();
	m_nozzleRot.Reset();

	m_quake.Reset();
	m_quakePos.Reset();
	m_flash = 0;
	m_spawnParent = NULL;

	for (std::list<CFxParticle*>::iterator it = m_particle.begin();
		it != m_particle.end(); ++it)
		delete *it;

	m_particle.clear();

	m_param.bSphere = false;

	m_param.genPos.min = m_param.genPos.max = WVector::ZERO;
	m_param.bRegenAtSamePos = false;
	m_param.camOffset = 0.0f;
	m_param.bBillboard = true;
	m_param.sprite = NULL;
	m_param.petName[0] = 0;
	m_param.bPetLight = false;
	m_param.bStrongBond = false;
	m_param.bAttachedToParent = false;
	m_param.attachedAxis = 0;

	m_param.lifeTime[0] = m_param.lifeTime[1] = 1000;
	m_param.volMaxOpacity = 1.0f;

	m_param.volCore[0] = m_param.volCore[1] = 1.0f;
	m_param.friction = 0.0f;
	m_param.groundFriction = 0.0f;
	m_param.gravity = WVector::ZERO;
	m_param.groundHeight = -1000.0f;
	m_param.groundBounce = 0.0f;
	m_param.groundRadius = 0.0f;
	m_param.blend[0] = 0x800000;
	m_param.blend[1] = 0x800000;

	m_param.lighting = 0;
	m_param.lightColor = 0xffffff;

	m_param.initVelocity.min = m_param.initVelocity.max = WVector::ZERO;

	m_param.addWind[0] = m_param.addWind[1] = 0.0f;
	m_param.extWind.Reset();
	m_param.extOrientation = 0;

	m_param.initAngle.min = m_param.initAngle.max = WVector::ZERO;

	m_param.initRotation.min = m_param.initRotation.max = WVector::ZERO;

	m_param.initFrame[0] = m_param.initFrame[1] = 0;

	m_param.initSize[0] = m_param.initSize[1] = 1.0f;
	m_param.initRatio = 1.0f;

	m_param.tailColor[0] = 0;
	m_param.tailColor[1] = 255;
	m_param.tailColor[2] = 255;
	m_param.tailColor[3] = 255;

	m_param.tailNum = 0;
	m_param.tailInterval = 0.0f;
	m_param.tailTexture[0] = 0;
	m_param.tailTexHandle = 0;
	m_param.tailTexScale = 50.0f;
	m_param.bMonolith = false;
	m_param.bCameraSide = false;

	m_param.rotation.clear();
	m_param.nozzleRotation.clear();
	m_param.sprayRotation.clear();
	m_param.revolution.clear();
	m_param.generation.clear();
	m_param.gravityPoint.clear();
	m_param.fade.clear();
	m_param.size.clear();
	m_param.frame.clear();

	m_param.distLimit[0] = m_param.distLimit[1] = 0.0f;

	m_param.bEvenSphere = false;
	m_sphereAngle = 0.0f;

	memset(&m_defines, 0, sizeof(m_defines));
}

bool CFxSpray::ParseFile(const char* filename, bool bUseOriginal)
{
	strcpy(m_filename, filename);

	std::map<std::string, CFxSpray*>::iterator it =
		m_fxBox.m_originalSpray.find(filename);
	cFile* file;
	if (it == m_fxBox.m_originalSpray.end())
	{
		file = m_fxBox.m_resrcMng->GetCFile(filename, 0x2000);
		if (!file)
			return false;

		char line[1024];
		while (file->Scan("%n", line))
		{
			if (!ParseLine(TrimA(line, " \t\n\r\0")))
			{
				CloseCFile(file);
				return false;
			}
		}
		CloseCFile(file);

		CFxSpray* original = new CFxSpray(m_fxBox);
		original->m_param = m_param;
		m_fxBox.m_originalSpray[filename] = original;
	}
	else if (bUseOriginal)
	{
		m_param = (*it).second->m_param;
	}
	else
	{
		file = m_fxBox.m_resrcMng->GetCFile(filename, 0x2000);
		if (!file)
			return false;

		char line[1024];
		while (file->Scan("%n", line))
		{
			if (!ParseLine(TrimA(line, " \t\n\r\0")))
			{
				CloseCFile(file);
				return false;
			}
		}
		CloseCFile(file);
	}

	return true;
}

bool CFxSpray::ParseLine(const char* line)
{
	if (!*line || *line == ';' || m_fxBox.PreprocessDefines(m_defines, line))
		return true;

	cTokenV tokens;
	tokens.Init(line, strlen(line));

	char command[260], args[16][260];
	tokens.GetToken(command, "=", 1);
	TrimA(command, " \t\n\r\0");
	for (int i = 0; i < 16; i++)
	{
		tokens.GetToken(args[i], ",", 1);
		TrimA(args[i], " \t()%");
		m_fxBox.ProcessDefines(m_defines, args[i]);
	}

	if (!strcmpi(command, "source"))
	{
		cTokenV source;
		source.Init(args[0], strlen(args[0]));
		char* name;
		source.GetTokenWithoutFail(&name, " \t", 2);
		if (!strcmpi(name, "*pet"))
		{
			source.GetTokenWithoutFail(&name, " \t", 2);
			if (!m_fxBox.m_resrcMng)
				return false;
			const char* match = m_fxBox.m_resrcMng->FindMatchFile(name);
			if (!strcmpi(match, name))
				return false;
			strcpy(m_param.petName, args[0]);
			m_param.bPetLight = !strcmpi(args[1], "petlight");
		}
		else
		{
			m_param.sprite =
				m_fxBox.LoadSprite(args[0], atoi(args[1]), atoi(args[2]));
			if (!m_param.sprite)
				return false;
			m_param.bBillboard =
				!strlen(args[3]) || !strcmpi(args[3], "billboard");
		}
	}
	else if (!strcmpi(command, "tail"))
	{
		m_param.tailNum = atoi(args[0]);
		m_param.tailInterval = m_fxBox.ParseNumber(args[1]);
		m_param.tailColor[0] = atoi(args[2]);
		m_param.tailColor[1] = atoi(args[3]);
		m_param.tailColor[2] = atoi(args[4]);
		m_param.tailColor[3] = atoi(args[5]);
		strcpy(m_param.tailTexture, args[6]);
		if (strlen(m_param.tailTexture))
		{
			if (!m_fxBox.m_resrcMng)
				return false;
			m_param.tailTexHandle = m_fxBox.m_resrcMng->LoadTexture(
				m_param.tailTexture, 0, 0, NULL);
			if (!m_param.tailTexHandle)
				return false;
			m_param.tailTexScale = atof(args[7]);
			m_param.bMonolith = !strcmpi(args[8], "monolith");
			m_param.bCameraSide = !strcmpi(args[9], "cameraside");
		}
	}
	else if (!strcmpi(command, "issphere"))
	{
		int type = atoi(args[0]);
		m_param.bSphere = type != 0;
		if (type == 2)
			m_param.bEvenSphere = true;
	}
	else if (!strcmpi(command, "genpos"))
	{
		m_param.genPos.min =
			WVector(atof(args[0]), atof(args[1]), atof(args[2]));
		if (!m_param.bSphere)
			m_param.genPos.max =
				WVector(atof(args[3]), atof(args[4]), atof(args[5]));
		else
			m_param.genPos.max = WVector(atof(args[3]), atof(args[4]),
				m_fxBox.ParseNumber(args[5]));
	}
	else if (!strcmpi(command, "regenatsamepos"))
	{
		m_param.bRegenAtSamePos = atoi(args[0]) != 0;
	}
	else if (!strcmpi(command, "camoffset"))
	{
		m_param.camOffset = atof(args[0]);
	}
	else if (!strcmpi(command, "add_generation"))
	{
		sGen gen;
		gen.start = (int)m_fxBox.ParseNumber(args[0]);
		gen.end = (int)m_fxBox.ParseNumber(args[1]);
		gen.num = atoi(args[2]);
		gen.spread = m_fxBox.ParseNumber(args[3]);
		gen.count = 0;
		gen.time = 0.0f;
		m_param.generation.push_back(gen);
	}
	else if (!strcmpi(command, "lifetime"))
	{
		m_param.lifeTime[0] = (int)m_fxBox.ParseNumber(args[0]);
		m_param.lifeTime[1] = (int)m_fxBox.ParseNumber(args[1]);
		if (m_param.lifeTime[0] == 0)
		{
			for (std::list<sFade>::iterator it = m_param.fade.begin();
				it != m_param.fade.end(); it++)
			{
				if ((*it).start == 0.0f)
				{
					(*it).startColor[0] = (*it).endColor[0] = 255;
					break;
				}
			}
		}
		else
		{
			m_param.volMaxOpacity = m_param.volCore[0] = m_param.volCore[1] =
				1.0f;
		}
	}
	else if (!strcmpi(command, "vol_max_opacity"))
	{
		m_param.volMaxOpacity = Between(0.0f, (float)atof(args[0]), 1.0f);
		if (m_param.lifeTime[0])
			m_param.volMaxOpacity = 1.0f;
	}
	else if (!strcmpi(command, "vol_core"))
	{
		if (m_param.lifeTime[0])
		{
			m_param.volCore[0] = m_param.volCore[1] = 1.0f;
		}
		else
		{
			m_param.volCore[0] = Between(0.0f, (float)atof(args[0]), 1.0f);
			m_param.volCore[1] = Between(0.0f, (float)atof(args[1]), 1.0f);
		}
	}
	else if (!strcmpi(command, "init_velocity"))
	{
		m_param.initVelocity.min =
			WVector(atof(args[0]), atof(args[1]), atof(args[2]));
		m_param.initVelocity.max =
			WVector(atof(args[3]), atof(args[4]), atof(args[5]));
	}
	else if (!strcmpi(command, "add_velocity"))
	{
		sVel vel;
		vel.start = (int)m_fxBox.ParseNumber(args[0]);
		vel.startRate = atof(args[1]);
		vel.end = (int)m_fxBox.ParseNumber(args[2]);
		vel.endRate = atof(args[3]);
		m_param.velocity.push_back(vel);
	}
	else if (!strcmpi(command, "add_wind"))
	{
		m_param.addWind[0] = atof(args[0]);
		m_param.addWind[1] = atof(args[1]);
	}
	else if (!strcmpi(command, "ext_wind"))
	{
		m_param.extWind = WVector(atof(args[0]), atof(args[1]), atof(args[2]));
	}
	else if (!strcmpi(command, "ext_orientation"))
	{
		int x = 0, y = 0, z = 0;
		sscanf(args[0], "%d", &x);
		sscanf(args[1], "%d", &y);
		sscanf(args[2], "%d", &z);
		m_param.extOrientation = 0;
		if (x)
			m_param.extOrientation |= 1;
		if (y)
			m_param.extOrientation |= 2;
		if (z)
			m_param.extOrientation |= 4;
	}
	else if (!strcmpi(command, "init_angle"))
	{
		m_param.initAngle.min.x = m_fxBox.ParseNumber(args[0]);
		m_param.initAngle.min.y = m_fxBox.ParseNumber(args[1]);
		m_param.initAngle.min.z = m_fxBox.ParseNumber(args[2]);
		m_param.initAngle.max.x = m_fxBox.ParseNumber(args[3]);
		m_param.initAngle.max.y = m_fxBox.ParseNumber(args[4]);
		m_param.initAngle.max.z = m_fxBox.ParseNumber(args[5]);
	}
	else if (!strcmpi(command, "init_rotation"))
	{
		m_param.initRotation.min.x = m_fxBox.ParseNumber(args[0]);
		m_param.initRotation.min.y = m_fxBox.ParseNumber(args[1]);
		m_param.initRotation.min.z = m_fxBox.ParseNumber(args[2]);
		m_param.initRotation.max.x = m_fxBox.ParseNumber(args[3]);
		m_param.initRotation.max.y = m_fxBox.ParseNumber(args[4]);
		m_param.initRotation.max.z = m_fxBox.ParseNumber(args[5]);
	}
	else if (!strcmpi(command, "add_rotation"))
	{
		sRot rot;
		rot.start = atof(args[0]);
		rot.startRate = m_fxBox.ParseNumber(args[1]);
		rot.end = atof(args[2]);
		rot.endRate = m_fxBox.ParseNumber(args[3]);
		m_param.rotation.push_back(rot);
	}
	else if (!strcmpi(command, "friction"))
	{
		m_param.friction = atof(args[0]);
	}
	else if (!strcmpi(command, "groundfriction"))
	{
		m_param.groundFriction = atof(args[0]);
	}
	else if (!strcmpi(command, "gravity"))
	{
		m_param.gravity = WVector(atof(args[0]), atof(args[1]), atof(args[2]));
	}
	else if (!strcmpi(command, "add_gravitypoint"))
	{
		sGravity gravity;
		gravity.pos.x = atof(args[0]);
		gravity.pos.y = atof(args[1]);
		gravity.pos.z = atof(args[2]);
		gravity.power = atof(args[3]);
		gravity.radius = atof(args[4]);
		gravity.outerPower = atof(args[5]);
		m_param.gravityPoint.push_back(gravity);
	}
	else if (!strcmpi(command, "ground"))
	{
		m_param.groundHeight = atof(args[0]);
		m_param.groundBounce = atof(args[1]);
		m_param.groundRadius = atof(args[2]);
	}
	else if (!strcmpi(command, "blend"))
	{
		if (!strcmpi(args[0], "normal"))
			m_param.blend[0] = 0;
		else if (!strcmpi(args[0], "add"))
			m_param.blend[0] = 0x800000;
		else if (!strcmpi(args[0], "multiply"))
			m_param.blend[0] = 0x1000000;
		else if (!strcmpi(args[0], "invmultiply"))
			m_param.blend[0] = 0x1800000;
		else
			return false;

		if (!strcmpi(args[1], "normal"))
			m_param.blend[1] = 0;
		else if (!strcmpi(args[1], "add"))
			m_param.blend[1] = 0x800000;
		else if (!strcmpi(args[1], "multiply"))
			m_param.blend[1] = 0x1000000;
		else if (!strcmpi(args[1], "invmultiply"))
			m_param.blend[1] = 0x1800000;
		else
			return false;
	}
	else if (!strcmpi(command, "lighting"))
	{
		if (!strcmpi(args[0], "off"))
			m_param.lighting = 0;
		else if (!strcmpi(args[0], "diffuse"))
			m_param.lighting = 1;
		else if (!strcmpi(args[0], "ambient"))
			m_param.lighting = 2;
		else if (!strcmpi(args[0], "ambient2"))
			m_param.lighting = 3;
		else
			return false;
	}
	else if (!strcmpi(command, "add_fade"))
	{
		sFade fade;
		fade.start = atof(args[0]);
		fade.startColor[0] = atoi(args[1]);
		fade.startColor[1] = atoi(args[2]);
		fade.startColor[2] = atoi(args[3]);
		fade.startColor[3] = atoi(args[4]);
		fade.end = atof(args[5]);
		fade.endColor[0] = atoi(args[6]);
		fade.endColor[1] = atoi(args[7]);
		fade.endColor[2] = atoi(args[8]);
		fade.endColor[3] = atoi(args[9]);
		if (m_param.lifeTime[0] == 0 && fade.start == 0.0f)
			fade.startColor[0] = fade.endColor[0] = 255;
		m_param.fade.push_back(fade);
	}
	else if (!strcmpi(command, "init_size"))
	{
		sscanf(args[0], "%f", &m_param.initSize[0]);
		sscanf(args[1], "%f", &m_param.initSize[1]);
		sscanf(args[2], "%f", &m_param.initRatio);
	}
	else if (!strcmpi(command, "add_size"))
	{
		sSize size;
		sscanf(args[0], "%f", &size.start);
		sscanf(args[1], "%f", &size.startSize);
		sscanf(args[2], "%f", &size.end);
		sscanf(args[3], "%f", &size.endSize);
		m_param.size.push_back(size);
	}
	else if (!strcmpi(command, "init_frame"))
	{
		sscanf(args[0], "%d", &m_param.initFrame[0]);
		sscanf(args[1], "%d", &m_param.initFrame[1]);
	}
	else if (!strcmpi(command, "add_frame"))
	{
		sFrame frame;
		sscanf(args[0], "%f", &frame.start);
		sscanf(args[1], "%f", &frame.end);
		sscanf(args[2], "%d", &frame.startFrame);
		sscanf(args[3], "%d", &frame.endFrame);
		frame.interval = (int)m_fxBox.ParseNumber(args[4]);
		m_param.frame.push_back(frame);
	}
	else if (!strcmpi(command, "add_quake"))
	{
		sQuake quake;
		quake.pos.x = atof(args[0]);
		quake.pos.y = atof(args[1]);
		quake.pos.z = atof(args[2]);
		quake.start = (int)m_fxBox.ParseNumber(args[3]);
		quake.startPower.x = atof(args[4]);
		quake.startPower.y = atof(args[5]);
		quake.startPower.z = atof(args[6]);
		quake.end = (int)m_fxBox.ParseNumber(args[7]);
		quake.endPower.x = atof(args[8]);
		quake.endPower.y = atof(args[9]);
		quake.endPower.z = atof(args[10]);
		quake.interval = (int)m_fxBox.ParseNumber(args[11]);
		quake.time = 0.0f;
		m_param.quake.push_back(quake);
	}
	else if (!strcmpi(command, "add_flash"))
	{
		sFlash flash;
		flash.start = (int)m_fxBox.ParseNumber(args[0]);
		flash.peak = (int)m_fxBox.ParseNumber(args[1]);
		flash.end = (int)m_fxBox.ParseNumber(args[2]);
		m_param.flash.push_back(flash);
	}
	else if (!strcmpi(command, "dist_limit"))
	{
		m_param.distLimit[0] = Max(0.0f, (float)atof(args[0]));
		m_param.distLimit[1] = Max(0.0f, (float)atof(args[1]));
		if (m_param.distLimit[1] < m_param.distLimit[0])
			m_param.distLimit[1] = m_param.distLimit[0];
	}
	else if (!strcmpi(command, "add_bubble"))
	{
		sBubble bubble;
		bubble.amplitude.x = atof(args[0]);
		bubble.frequency.x = atof(args[1]);
		bubble.amplitude.y = atof(args[2]);
		bubble.frequency.y = atof(args[3]);
		bubble.amplitude.z = atof(args[4]);
		bubble.frequency.z = atof(args[5]);
		m_param.bubble.push_back(bubble);
	}
	else if (!strcmpi(command, "sound"))
	{
		sSound sound;
		sound.range = -1.0f;
		strcpy(sound.name, args[0]);
		char* ext = strstr(sound.name, ".wav");
		if (ext)
			*ext = 0;
		sscanf(args[1], "%f", &sound.range);
		m_param.sound.push_back(sound);
	}
	else
	{
		return false;
	}

	return true;
}

bool CFxSpray::Process(float elapsed)
{
	const float scale = 0.06f;
	const float msec = 0.001f;
	const float quakeScale = 0.00005f;
	const int quakeRange = 20001;
	const int quakeHalf = 10000;
	const int flashMax = 255;
	const float percent = 100.0f;
	const float minSq = 0.5f;
	const float bounce = -0.01f;
	const float groundGap = 0.03f;
	const float minSpeed = 0.01f;
	const float rateScale = 0.01f;
	const float minMove = 0.0001f;
	const int phaseRange = 1001;
	const int allAxes = 7;
	float dt = elapsed * scale;

	m_pos += m_vel * dt;

	if (m_param.bAttachedToParent && m_parent)
	{
		WMatrix mat;
		m_parent->GetBoneMatrix(&mat);
		mat.GetRotMatrix(&m_rot);

		if (m_param.attachedAxis && m_param.attachedAxis != allAxes)
		{
			WVector axis;
			WVector dir;
			WVector angle;
			angle.Reset();

			if (m_param.attachedAxis & 1)
			{
				axis = WVector::UNIT_POS_Y;
				dir = axis * m_rot;
				dir.x = 0.0f;
				angle.x = GetAngle(axis, dir);
				if (!WisZero(dir.z, g_EPSILON))
					angle.x *= dir.z < 0.0f ? -1.0f : 1.0f;
			}
			if (m_param.attachedAxis & 2)
			{
				axis = WVector::UNIT_POS_Z;
				dir = axis * m_rot;
				dir.y = 0.0f;
				angle.y = GetAngle(axis, dir);
				if (!WisZero(dir.x, g_EPSILON))
					angle.y *= dir.x < 0.0f ? -1.0f : 1.0f;
			}
			if (m_param.attachedAxis & 4)
			{
				axis = WVector::UNIT_POS_X;
				dir = axis * m_rot;
				dir.z = 0.0f;
				angle.z = GetAngle(axis, dir);
				if (!WisZero(dir.y, g_EPSILON))
					angle.z *= dir.y > 0.0f ? -1.0f : 1.0f;
			}

			m_rot = ~RotMat(angle);
		}
	}

	WMatrix nozzleRot = RotateNozzle(m_time, dt);
	m_nozzleRot = m_nozzleRot * nozzleRot;
	WMatrix rot = Rotate(m_time, dt);
	Revolute(m_time, dt);
	RevoluteGrand(m_time, dt);

	m_quakePos.Reset();
	bool bQuake = false;
	std::list<sQuake>::iterator q = m_param.quake.begin();
	WVector powerSum;
	powerSum.Reset();
	while (q != m_param.quake.end())
	{
		sQuake& quake = *q;
		if (m_time < quake.start)
		{
			quake.time = m_time;
		}
		else
		{
			if (m_time <= quake.end)
			{
				bQuake = true;
				if (m_time - quake.time >= quake.interval)
				{
					quake.time = m_time;

					WVector power = quake.end - quake.start > 0
						? quake.startPower +
							(quake.endPower - quake.startPower) *
								(m_time - quake.start) /
								(quake.end - quake.start)
						: quake.startPower;

					m_quake.Reset();
					int rx = rand() % quakeRange - quakeHalf;
					m_quake.x += rx * power.x * quakeScale;
					int ry = rand() % quakeRange - quakeHalf;
					m_quake.y += ry * power.y * quakeScale;
					int rz = rand() % quakeRange - quakeHalf;
					m_quake.z += rz * power.z * quakeScale;

					m_quakePos.x += power.x * quake.pos.x;
					m_quakePos.y += power.y * quake.pos.y;
					m_quakePos.z += power.z * quake.pos.z;

					powerSum += power;
				}
			}
		}
		++q;
	}

	if (!bQuake)
	{
		m_quake.Reset();
	}
	else
	{
		if (!WisZero(powerSum.x, g_EPSILON))
			m_quakePos.x /= powerSum.x;
		if (!WisZero(powerSum.y, g_EPSILON))
			m_quakePos.y /= powerSum.y;
		if (!WisZero(powerSum.z, g_EPSILON))
			m_quakePos.z /= powerSum.z;
	}

	m_flash = 0;
	for (std::list<sFlash>::iterator f = m_param.flash.begin();
		f != m_param.flash.end(); ++f)
	{
		sFlash& flash = *f;
		if (m_time >= flash.start)
		{
			if (m_time <= flash.peak)
			{
				int value = flash.peak - flash.start > 0 ? flashMax *
						(int)(m_time - flash.start) / (flash.peak - flash.start)
														 : 0;
				value += m_flash;
				if (value > flashMax)
					value = flashMax;
				m_flash = value;
			}
			else if (m_time <= flash.end)
			{
				int value = flash.end - flash.peak > 0 ? flashMax -
						flashMax * (int)(m_time - flash.peak) /
							(flash.end - flash.peak)
													   : 0;
				value += m_flash;
				if (value > flashMax)
					value = flashMax;
				m_flash = value;
			}
		}
	}

	bool bGenerated = Generate(m_time);

	Waabb core;
	bool bRegen = false;
	GetCoreBox(core, m_param.genPos, m_param.bSphere, m_param.volCore[0],
		m_param.volCore[1]);

	std::list<CFxParticle*>::iterator it = m_particle.begin();
	while (it != m_particle.end())
	{
		CFxParticle* p = *it;

		WVector pos = p->pos * GetBgRot(p) + GetBgPos(p);
		float age;
		if (p->lifeTime > 0.0f)
			age = (p->lifeTime - p->life) / p->lifeTime * percent;
		else
			age = percent;

		MakeTail(m_time, p, pos + p->bubble);

		if (!p->bStop)
		{
			WVector prev = pos;
			WVector wind = m_param.extWind * p->wind;
			pos += dt * (p->vel * GetBgRot(p) + wind);

			WVector friction = p->vel;
			float rate = p->bGround ? m_param.groundFriction : m_param.friction;
			friction = friction.Normalize() * dt * rate;
			if (p->vel.x > 0.0f && p->vel.x < friction.x ||
				p->vel.x < 0.0f && p->vel.x > friction.x)
				p->vel.x = 0.0f;
			else
				p->vel.x -= friction.x;
			if (p->vel.y > 0.0f && p->vel.y < friction.y ||
				p->vel.y < 0.0f && p->vel.y > friction.y)
				p->vel.y = 0.0f;
			else
				p->vel.y -= friction.y;
			if (p->vel.z > 0.0f && p->vel.z < friction.z ||
				p->vel.z < 0.0f && p->vel.z > friction.z)
				p->vel.z = 0.0f;
			else
				p->vel.z -= friction.z;

			if (!p->bGround)
			{
				for (std::list<sGravity>::iterator g =
						 m_param.gravityPoint.begin();
					g != m_param.gravityPoint.end(); ++g)
				{
					sGravity& gravity = *g;
					WVector dir = GetBgPos() + (*g).pos - pos;
					float dist = dir.Magnitude();
					if (dist < gravity.radius)
					{
						float sq = dist * dist;
						if (sq < minSq)
							sq = minSq;
						p->vel += dir.Normalize() * dt * gravity.power / sq;
					}
					else
						p->vel += dir.Normalize() * dt * gravity.outerPower;
				}

				WVector bgPos = GetBgPos(p);
				float height = bgPos.y + m_param.groundHeight;
				if (m_fxBox.CheckGroundCollision(prev, pos, height,
						m_param.groundRadius))
				{
					float y = pos.y - m_param.groundRadius;
					float gap = height - y > 0.0f ? height - y : y - height;
					if (prev.y < pos.y)
						pos.y = height - m_param.groundRadius;
					else
						pos.y = height + m_param.groundRadius;

					p->vel.y = m_param.groundBounce * p->vel.y * bounce;
					float speed = p->vel.y > 0.0f ? p->vel.y : -p->vel.y;
					if (gap < groundGap && speed < gap || speed < minSpeed ||
						(p->vel + m_param.gravity) * p->vel < 0.0f)
					{
						p->vel.y = 0.0f;
						p->bGround = true;
					}

					for (std::list<sSpawn>::iterator s = m_param.spawn.begin();
						s != m_param.spawn.end(); ++s)
					{
						sSpawn& spawn = *s;
						if (spawn.type == 3)
							Spawn(*s, p);
					}
				}
				else
				{
					p->vel += m_param.gravity * dt;
				}

				if (m_param.sprayRotation.size())
					pos = GetBgPos() + (pos - GetBgPos()) * rot;
			}
		}

		for (std::list<sFade>::iterator f = m_param.fade.begin();
			f != m_param.fade.end(); ++f)
		{
			sFade& fade = *f;
			if (age >= fade.start && age <= fade.end)
			{
				for (int i = 0; i < 4; i++)
				{
					p->bgra[3 - i] = fade.end - fade.start > 0.0f
						? (unsigned char)((fade.endColor[i] -
											  fade.startColor[i]) *
								  (age - fade.start) / (fade.end - fade.start) +
							  fade.startColor[i])
						: fade.startColor[i];
				}
			}
		}

		if (p->sprite || p->pet)
		{
			for (std::list<sRot>::iterator r = m_param.rotation.begin();
				r != m_param.rotation.end(); r++)
			{
				sRot& rotation = *r;
				if (age >= rotation.start && age <= rotation.end)
				{
					float rate;
					if (rotation.end - rotation.start > 0.0f)
						rate = (rotation.endRate - rotation.startRate) *
								(age - rotation.start) /
								(rotation.end - rotation.start) +
							rotation.startRate;
					else
						rate = rotation.startRate;
					p->rot += p->rotVel * dt * rate * rateScale;
				}
			}

			for (std::list<sSize>::iterator s = m_param.size.begin();
				s != m_param.size.end(); s++)
			{
				sSize& size = *s;
				if (age >= size.start && age <= size.end)
				{
					float rate;
					if (size.end - size.start > 0.0f)
						rate = (size.endSize - size.startSize) *
								(age - size.start) / (size.end - size.start) +
							size.startSize;
					else
						rate = size.startSize;
					p->size = rate * p->initSize * 0.01f;
				}
			}

			if (!p->bGround)
			{
				for (std::list<sFrame>::iterator f = m_param.frame.begin();
					f != m_param.frame.end(); f++)
				{
					if (age >= (*f).start && age <= (*f).end &&
						m_time - p->frameTime >= (*f).interval)
					{
						p->frameTime = m_time;
						if ((*f).startFrame == -1)
						{
							int frame = rand() % (*f).endFrame;
							p->frame = frame;
						}
						else if (++p->frame > (*f).endFrame)
							p->frame = (*f).startFrame;
					}
				}
			}

			if (!m_param.lifeTime[0] && elapsed != 0.0f)
			{
				WMatrix mat;
				GetBoneMatrix(&mat);
				WMatrix rm;
				mat.GetRotMatrix(&rm);

				WVector bgPos = GetBgPos(p);
				WVector local;
				local = (pos - bgPos) * ~rm;

				bool bIn;
				if (!m_param.bSphere)
				{
					WVector lo = m_param.genPos.min -
						WVector(g_EPSILON, g_EPSILON, g_EPSILON);
					WVector hi = m_param.genPos.max +
						WVector(g_EPSILON, g_EPSILON, g_EPSILON);
					Waabb bound(lo, hi);
					bIn = bound.min.x <= local.x && local.x <= bound.max.x &&
						bound.min.y <= local.y && local.y <= bound.max.y &&
						bound.min.z <= local.z && local.z <= bound.max.z;
				}
				else
				{
					WVector d = local - m_param.genPos.min;
					float dist = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
					bIn = dist < m_param.genPos.max.y + minMove;
				}

				if (bIn)
				{
					float opacity = GetVolumeOpacity(local, core,
						m_param.genPos, m_param.volCore[0], m_param.volCore[1],
						m_param.bSphere);
					p->volOpacity = opacity;
				}
				else
				{
					InitParticle(m_param.bRegenAtSamePos, p, percent, 0.0f,
						core, true);
					bRegen = false;
					++it;
					continue;
				}
			}

			if (p->pet)
			{
				unsigned char alpha =
					(unsigned char)((p->bgra[3] * m_param.volMaxOpacity) *
						p->volOpacity);
				p->pet->SetAlpha(alpha);
				p->pet->SetLight();
				p->pet->SetPos(pos);
				p->pet->SetRotate(p->rot);
				p->pet->MultRotate(&m_rot);
				p->pet->SetScale(p->size);
				p->pet->Process(elapsed * msec);
			}
		}

		for (std::list<sSpawn>::iterator s = m_param.spawn.begin();
			s != m_param.spawn.end(); ++s)
		{
			sSpawn& spawn = *s;
			if (spawn.type == 1 && !p->spawnNum && age + 1.0f > spawn.time)
				Spawn(spawn, p);
			if (spawn.type == 0 && m_time - p->spawnTime > spawn.time &&
				spawn.count > p->spawnNum)
			{
				p->spawnTime = m_time;
				Spawn(spawn, p);
			}
		}

		if (p->life > 0.0f && !bRegen)
		{
			p->life -= elapsed;
			if (p->life <= 0.0f)
			{
				for (std::list<sSpawn>::iterator s = m_param.spawn.begin();
					s != m_param.spawn.end(); ++s)
				{
					sSpawn& spawn = *s;
					if (spawn.type == 2)
						Spawn(*s, p);
				}

				if (m_param.bRegenAtSamePos)
				{
					bRegen = true;
					InitParticle(m_param.bRegenAtSamePos, p, percent, 0.0f,
						core, true);
				}
				else
				{
					it = m_particle.erase(it);
					delete p;
				}
				continue;
			}
		}

		if (m_param.bubble.size())
		{
			WVector amp;
			amp.Reset();
			std::list<WVector>::iterator phase = p->bubblePhase.begin();
			for (std::list<sBubble>::iterator b = m_param.bubble.begin();
				b != m_param.bubble.end(); ++b)
			{
				if (phase == p->bubblePhase.end())
				{
					float z = (rand() % phaseRange) * (g_PI * 2.0f * 0.001f);
					float y = (rand() % phaseRange) * (g_PI * 2.0f * 0.001f);
					float x = (rand() % phaseRange) * (g_PI * 2.0f * 0.001f);
					p->bubblePhase.push_back(WVector(x, y, z));
					--phase;
				}
				WVector& ph = *phase++;
				amp.x +=
					cosf((*b).frequency.x * m_time * (g_PI * 2.0f * 0.001f) +
						ph.x) *
					(*b).amplitude.x;
				amp.y +=
					cosf((*b).frequency.y * m_time * (g_PI * 2.0f * 0.001f) +
						ph.y) *
					(*b).amplitude.y;
				amp.z +=
					cosf((*b).frequency.z * m_time * (g_PI * 2.0f * 0.001f) +
						ph.z) *
					(*b).amplitude.z;
			}

			WVector forward = p->vel;
			WVector side = !IsEqual(p->vel.x, 0.0f) || !IsEqual(p->vel.z, 0.0f)
				? WCrossProduct(WVector::UNIT_POS_Y, p->vel)
				: WVector::UNIT_POS_Z;
			WVector up = WCrossProduct(forward, side);
			p->bubble += (forward.Normalize() * amp.x + up.Normalize() * amp.y +
							 side.Normalize() * amp.z) *
				dt;
		}

		if (m_param.sound.size())
		{
			for (std::list<sSound>::iterator s = m_param.sound.begin();
				s != m_param.sound.end(); ++s)
			{
				sSound& sound = *s;
				if (sound.range < 0.0f)
					g_audio->PlaySfx(sound.name);
				else
				{
					WVector relative = p->pos - m_fxBox.m_camera->pivot;
					g_audio->PlaySfx(sound.name, &relative, 5, NULL, NULL, 0,
						sound.range);
				}
			}
			m_param.sound.clear();
		}

		p->pos = (pos - GetBgPos(p)) * ~GetBgRot(p);

		bRegen = false;
		++it;
	}

	m_time += elapsed;

	return m_particle.size() > 0 || bGenerated;
}

void CFxSpray::Display(WView* view)
{
	if (!m_bActive)
		return;

	view->SetScreenCenter(m_centerX == g_HUGE ? view->GetWidth() * 0.5f
											  : m_centerX,
		m_centerY == g_HUGE ? view->GetHeight() * 0.5f : m_centerY);
	view->UpdateCamera();

	m_particleNum = 0;
	m_tailNum = 0;

	if (m_fxBox.m_bShowBound)
	{
		if (!m_param.bSphere)
		{
			view->DrawAABB(m_param.genPos + GetBgPos(), 0xffff0000, false, 0);
			WVector center = (m_param.genPos.min + m_param.genPos.max) * 0.5f;
			view->DrawSphere(GetBgPos() + center, 0.5f, 0xffff0000, 10, 0);
		}
		else
		{
			WSphere sphere(m_param.genPos.min, m_param.genPos.max.x);
			view->DrawSphere(sphere, 0xffff0000, 64, 0);
			sphere = WSphere(m_param.genPos.min, m_param.genPos.max.y);
			view->DrawSphere(sphere, 0xffff0000, 64, 0);
		}

		for (std::list<sGravity>::iterator it = m_param.gravityPoint.begin();
			it != m_param.gravityPoint.end(); ++it)
			view->DrawSphere((*it).pos + GetBgPos(), 0.3f, 0xffffff00, 10, 0);
	}

	for (std::list<CFxParticle*>::iterator it = m_particle.begin();
		it != m_particle.end(); ++it)
	{
		m_particleNum++;

		CFxParticle* p = *it;
		WVector pos;
		pos = (p->pos + p->bubble) * GetBgRot(p) + GetBgPos(p);
		m_bgPos = GetBgPos(p);

		DrawSprite(view, p, pos);
		DrawTail(view, p, pos);
		DrawBandana(view, p, pos);
	}

	view->ResetScreenCenter();
	view->UpdateCamera();
}

WMatrix CFxSpray::Rotate(float time, float elapsed)
{
	WMatrix mat;
	mat.Reset();

	std::list<sNozzleRot>::iterator end = m_param.sprayRotation.end();
	for (std::list<sNozzleRot>::iterator it = m_param.sprayRotation.begin();
		it != end; ++it)
	{
		sNozzleRot& rot = *it;
		if (time >= rot.start && rot.end >= time)
		{
			float speed;
			if (time - rot.start > 0.0f)
				speed = (time - rot.start) * (rot.endSpeed - rot.startSpeed) /
						(rot.end - rot.start) +
					rot.startSpeed;
			else
				speed = rot.startSpeed;
			mat = RotMat(rot.axis, speed * elapsed);
		}
	}

	return mat;
}

WMatrix CFxSpray::RotateNozzle(float time, float elapsed)
{
	WMatrix mat;
	mat.Reset();

	std::list<sNozzleRot>::iterator end = m_param.nozzleRotation.end();
	for (std::list<sNozzleRot>::iterator it = m_param.nozzleRotation.begin();
		it != end; ++it)
	{
		sNozzleRot& rot = *it;
		if (time >= rot.start && rot.end >= time)
		{
			float speed;
			if (time - rot.start > 0.0f)
				speed = (time - rot.start) * (rot.endSpeed - rot.startSpeed) /
						(rot.end - rot.start) +
					rot.startSpeed;
			else
				speed = rot.startSpeed;
			mat = RotMat(rot.axis, speed * elapsed);
		}
	}

	return mat;
}

void CFxSpray::Revolute(float time, float elapsed)
{
	for (std::list<sRevol>::iterator it = m_param.revolution.begin();
		it != m_param.revolution.end(); ++it)
	{
		sRevol& revol = *it;
		if (revol.start <= time && revol.end >= time)
		{
			float speed;
			if (revol.end - revol.start > 0)
				speed = (time - revol.start) *
						(revol.endSpeed - revol.startSpeed) /
						(revol.end - revol.start) +
					revol.startSpeed;
			else
				speed = revol.startSpeed;
			WMatrix mat = RotMat(revol.axis, speed * elapsed);

			WVector pos = m_pos;
			m_pos = (m_pos - revol.center) * mat + revol.center;
			m_vel += pos;
			m_vel = (m_vel - revol.center) * mat + revol.center;
			m_vel -= m_pos;

			for (std::list<sNozzleRot>::iterator r =
					 m_param.sprayRotation.begin();
				r != m_param.sprayRotation.end(); ++r)
			{
				sNozzleRot& rot = *r;
				rot.axis += pos;
				rot.axis = (rot.axis - revol.center) * mat + revol.center;
				rot.axis -= m_pos;
			}
		}
	}
}

void CFxSpray::RevoluteGrand(float time, float elapsed)
{
	for (std::list<sRevol>::iterator it = m_param.preRevolution.begin();
		it != m_param.preRevolution.end(); ++it)
	{
		sRevol& revol = *it;
		if (revol.start <= time && revol.end >= time)
		{
			float speed;
			if (revol.end - revol.start > 0)
				speed = (time - revol.start) *
						(revol.endSpeed - revol.startSpeed) /
						(revol.end - revol.start) +
					revol.startSpeed;
			else
				speed = revol.startSpeed;
			WMatrix mat = RotMat(revol.axis, speed * elapsed);

			WVector pos = m_pos;
			m_pos = (m_pos - revol.center) * mat + revol.center;
			m_vel += pos;
			m_vel = (m_vel - revol.center) * mat + revol.center;
			m_vel -= m_pos;

			for (std::list<sNozzleRot>::iterator r =
					 m_param.nozzleRotation.begin();
				r != m_param.nozzleRotation.end(); ++r)
			{
				sNozzleRot& rot = *r;
				rot.axis += pos;
				rot.axis = (rot.axis - revol.center) * mat + revol.center;
				rot.axis -= m_pos;
			}

			for (std::list<sRevol>::iterator r = m_param.revolution.begin();
				r != m_param.revolution.end(); ++r)
			{
				sRevol& rv = *r;
				rv.axis += rv.center;
				rv.axis = (rv.axis - revol.center) * mat + revol.center;
				rv.center = (rv.center - revol.center) * mat + revol.center;
				rv.axis -= rv.center;
			}
		}
	}
}

bool CFxSpray::Generate(float time)
{
	float velRate = 100.0f;
	for (std::list<sVel>::iterator it = m_param.velocity.begin();
		it != m_param.velocity.end(); ++it)
	{
		sVel& vel = *it;
		if (time >= vel.start && time <= vel.end)
		{
			if (vel.end - vel.start > 0)
				velRate = (vel.endRate - vel.startRate) * (time - vel.start) /
						(vel.end - vel.start) +
					vel.startRate;
			else
				velRate = vel.startRate;
		}
	}

	bool bGenerated = false;

	Waabb core;
	if (m_param.lifeTime[0] == 0)
		core = m_param.genPos;
	else
	{
		const Waabb& genPos = m_param.genPos;
		GetCoreBox(core, genPos, m_param.bSphere, m_param.volCore[0],
			m_param.volCore[1]);
	}

	std::list<sGen>::iterator it;
	for (it = m_param.generation.begin(); it != m_param.generation.end(); ++it)
	{
		sGen& gen = *it;
		if (time <= gen.end || gen.count < gen.num)
		{
			bGenerated = true;
			if (time < gen.start)
				gen.time = time;
			else
			{
				int num;
				if (time < gen.end && gen.end - gen.start > 0)
					num = (int)((time - gen.start) * gen.num /
								  (gen.end - gen.start) +
							  0.0001f) -
						gen.count + 1;
				else
					num = gen.num - gen.count;

				if (num > 0)
					gen.time = time;
				if (num + gen.count > gen.num)
					num = gen.num - gen.count;

				for (int i = 0; i < num; i++)
				{
					CFxParticle* p = new CFxParticle;
					InitParticle(false, p, velRate, gen.spread, core, false);
					m_particle.push_back(p);
					gen.count++;
				}
			}
		}
	}

	return bGenerated;
}

bool CFxSpray::InitParticle(bool bRegen, CFxParticle* p, float velRate,
	float spread, Waabb& core, bool bFirst)
{
	if (m_param.petName[0] && m_fxBox.m_resrcMng)
	{
		if (p->pet)
		{
			delete p->pet;
			p->pet = NULL;
		}
		p->pet = new CPetBody(m_param.petName, true);
		if (p->pet)
		{
			if (p->pet->GetPet())
			{
				w_motion_data* motion = p->pet->GetPet()->GetFirstMotionData();
				if (motion)
					p->pet->SetMotion(motion->name, false, 0.0f, false);
			}
			p->pet->ActiveGlobalLight(m_param.bPetLight);
			WPuppet* puppet = p->pet->GetPet();
			if (puppet)
				puppet->CalcBound(NULL, true);
		}
	}

	p->bStop = false;
	p->sprite = m_param.sprite;

	if (!bRegen)
	{
		if (!m_param.bSphere)
			p->pos = m_fxBox.Random(m_param.genPos.min, m_param.genPos.max);
		else if (m_param.bEvenSphere)
		{
			float angle = m_sphereAngle;
			float radius = m_param.genPos.max.x;
			int total = 0;
			for (std::list<sGen>::iterator it = m_param.generation.begin();
				it != m_param.generation.end(); it++)
				total += (*it).num;

			float x = m_fxBox.Random(0.0f, m_param.genPos.max.z);
			p->pos.x = x - m_param.genPos.max.z * 0.5f;
			p->pos.y = sinf(angle * g_DEGTORAD) * radius;
			p->pos.z = cosf(angle * g_DEGTORAD) * radius;
			m_sphereAngle += 360.0f / total;
		}
		else
			p->pos = m_fxBox.Random(m_param.genPos.min, m_param.genPos.max.x,
				m_param.genPos.max.y, m_param.genPos.max.z);
	}

	p->bubble.Reset();
	p->bgPos = GetBgPos();
	p->bgRot = m_rot;
	p->bGround = false;

	{
		WVector vel =
			m_fxBox.Random(m_param.initVelocity.min, m_param.initVelocity.max);
		vel *= velRate * 0.01f;
		p->vel = vel;

		const WVector& axis = vel.x == 0.0f && vel.z == 0.0f
			? WVector::UNIT_POS_Z
			: WCrossProduct(vel, WVector::UNIT_POS_Y);
		float angle = m_fxBox.Random(0.0f, spread);
		p->vel = p->vel * RotMat(axis - p->pos, angle);
		p->vel = p->vel * RotMat(vel, m_fxBox.Random(0.0f, g_PI * 2.0f));
		p->vel = p->vel * m_nozzleRot;
	}

	p->wind = m_fxBox.Random(m_param.addWind[0], m_param.addWind[1]);
	p->reserved = WVector(0.0f, 0.0f, 0.0f);

	WMatrix mat;
	GetBoneMatrix(&mat);
	WMatrix rm;
	mat.GetRotMatrix(&rm);

	WVector local;
	if (m_param.extOrientation)
	{
		if (m_param.extOrientation == 7)
		{
			WVector wind = m_param.extWind * p->wind;
			local = p->vel + wind * ~rm;
			p->vel = p->vel * rm;
		}
		else
		{
			WVector up;
			WVector angle;
			angle.Reset();
			if (m_param.extOrientation & 1)
			{
				up = WVector::UNIT_POS_Y;
				local = up * rm;
				local.x = 0.0f;
				angle.x = GetAngle(up, local);
				if (!WisZero(local.z, g_EPSILON))
					angle.x *= local.z < 0.0f ? -1.0f : 1.0f;
			}
			if (m_param.extOrientation & 2)
			{
				up = WVector::UNIT_POS_Z;
				local = up * rm;
				local.y = 0.0f;
				angle.y = GetAngle(up, local);
				if (!WisZero(local.x, g_EPSILON))
					angle.y *= local.x < 0.0f ? -1.0f : 1.0f;
			}
			if (m_param.extOrientation & 4)
			{
				up = WVector::UNIT_POS_X;
				local = up * rm;
				local.z = 0.0f;
				angle.z = GetAngle(up, local);
				if (!WisZero(local.y, g_EPSILON))
					angle.z *= local.y > 0.0f ? -1.0f : 1.0f;
			}

			WMatrix orient;
			orient = ~RotMat(angle);
			local = (p->vel * orient + m_param.extWind * p->wind) * ~rm;
			p->vel = p->vel * orient;
		}
	}
	else
	{
		local = (m_param.extWind * p->wind + p->vel) * ~rm;
	}

	if (bFirst &&
		(!WisZero(local.x, g_EPSILON) || !WisZero(local.y, g_EPSILON) ||
			!WisZero(local.z, g_EPSILON) ||
			!WisZero(m_param.gravity.x, g_EPSILON) ||
			!WisZero(m_param.gravity.y, g_EPSILON) ||
			!WisZero(m_param.gravity.z, g_EPSILON)))
	{
		if (m_param.bSphere)
		{
			WVector dir = p->pos - m_param.genPos.min;
			dir.Normalize();
			p->pos = dir * m_param.genPos.max.y * -0.999f + m_param.genPos.min;
		}
		else
		{
			WVector dir;
			if (!WisZero(local.x, g_EPSILON) || !WisZero(local.y, g_EPSILON) ||
				!WisZero(local.z, g_EPSILON))
				dir = local;
			else
				dir = m_param.gravity;

			const WVector& lower = m_param.genPos.min;
			const WVector& upper = m_param.genPos.max;
			WPlane plane[6] = {
				WPlane(0.0f, 0.0f, 1.0f, -lower.z),
				WPlane(0.0f, 0.0f, -1.0f, upper.z),
				WPlane(0.0f, 1.0f, 0.0f, -lower.y),
				WPlane(0.0f, -1.0f, 0.0f, upper.y),
				WPlane(1.0f, 0.0f, 0.0f, -lower.x),
				WPlane(-1.0f, 0.0f, 0.0f, upper.x),
			};

			WVector point[6] = {
				lower,
				WVector(upper.x, lower.y, upper.z),
				WVector(upper.x, lower.y, upper.z),
				WVector(lower.x, upper.y, lower.z),
				WVector(lower.x, lower.y, upper.z),
				WVector(upper.x, lower.y, lower.z),
			};
			bool bFirstPlane = true;
			int side = 0;
			if (dir.x > 0.0001f)
				side = 16;
			else if (dir.x < -0.0001f)
				side = 32;
			if (dir.z > 0.0001f)
				side |= 1;
			else if (dir.z < -0.0001f)
				side |= 2;
			if (dir.y > 0.0001f)
				side |= 4;
			else if (dir.y < -0.0001f)
				side |= 8;

			for (int i = 0; i < 6; i++)
			{
				if (!(side & (1 << i)))
					continue;
				if (!bFirstPlane &&
					(p->pos - point[i]) * plane[i].normal > 0.0f)
					continue;
				float dist = plane[i] * p->pos;
				p->pos = p->pos + dir * dist / (plane[i].normal * dir) * -1.0f;
				bFirstPlane = false;
			}
		}
	}

	p->rotVel =
		m_fxBox.Random(m_param.initRotation.min, m_param.initRotation.max) *
		m_nozzleRot;
	p->rot = m_fxBox.Random(m_param.initAngle.min, m_param.initAngle.max) *
		m_nozzleRot;

	float size = m_fxBox.Random(m_param.initSize[0], m_param.initSize[1]);
	p->initSize = p->size = size;
	float ratio = m_param.initRatio;
	p->ratio = ratio;

	// HACK!
	if (false)
		GetBgPos();
	unsigned long color = 0xffffffff;
	p->color = color;
	for (std::list<sFade>::iterator it = m_param.fade.begin();
		it != m_param.fade.end(); ++it)
	{
		sFade& fade = *it;
		if (fade.start == 0.0f)
		{
			p->bgra[3] = fade.startColor[0];
			p->bgra[2] = fade.startColor[1];
			p->bgra[1] = fade.startColor[2];
			p->bgra[0] = fade.startColor[3];
			break;
		}
	}

	float opacity = GetVolumeOpacity(p->pos, core, m_param.genPos,
		m_param.volCore[0], m_param.volCore[1], m_param.bSphere);
	p->volOpacity = opacity;

	float life =
		m_fxBox.Random((float)m_param.lifeTime[0], (float)m_param.lifeTime[1]);
	p->lifeTime = life;
	p->life = life;
	float frame = m_fxBox.Random((float)m_param.initFrame[0],
		(float)m_param.initFrame[1]);
	p->frame = (int)frame;
	p->frameTime = 0.0f;
	p->tailTime = 0.0f;

	if (m_param.tailNum > 0)
	{
		p->tail.Init(m_param.tailNum + 1);
		p->bandana.Init(m_param.tailNum + 1);
	}

	p->spawnNum = 0;
	p->spawnTime = m_time;

	return true;
}

WVector CFxSpray::GetBgPos()
{
	if (m_spawnParent)
		return m_pos;

	if (m_parent)
	{
		if (m_param.bAttachedToParent)
			return m_pos * m_rot + m_parent->m_pos;
		else
			return m_parent->m_pos + m_pos;
	}

	return m_pos;
}

void CFxSpray::MakeTail(float time, CFxParticle* p, const WVector& pos)
{
	if (m_param.tailNum == 0 || time - p->tailTime < m_param.tailInterval)
		return;

	p->tailTime = time;

	if (m_param.tailTexHandle)
	{
		CFxParticle::sBandana* bandana = p->bandana.GetTail(0);
		CFxParticle::sBandana* prev = p->bandana.GetTail(1);

		WVector side;
		if (p->vel.Magnitude() < 0.0001f && p->bandana.GetHead(0) != bandana)
			side = (prev->pos[0] - prev->pos[1]) / 2.0f;
		else
		{
			if (m_fxBox.m_camera && m_param.bCameraSide)
				side = WCrossProduct(m_fxBox.m_camera->za, p->vel);
			else
				side = WCrossProduct(WVector::UNIT_POS_Y, p->vel);

			side = side.Normalize() * p->size * m_param.tailTexScale * 0.005f;
		}

		bandana->pos[0] = pos + side;
		bandana->pos[1] = pos - side;

		if (p->bandana.GetHead(0) == bandana)
		{
			bandana->length = 0.0f;
		}
		else
		{
			if ((bandana->pos[0] - bandana->pos[1]) *
					(prev->pos[0] - prev->pos[1]) <
				0.0f)
			{
				WVector temp = bandana->pos[0];
				bandana->pos[0] = bandana->pos[1];
				bandana->pos[1] = temp;
			}

			bandana->length =
				(pos - (prev->pos[0] + prev->pos[1]) * 0.5f).Magnitude();
		}

		if (p->bandana.IsFull())
			p->bandana.StepHead(1);
		p->bandana.StepTail(1);
	}
	else
	{
		CFxParticle::sTail* tail = p->tail.GetTail(0);
		tail->pos = pos;
		tail->frame = p->frame;

		if (p->tail.IsFull())
			p->tail.StepHead(1);
		p->tail.StepTail(1);
	}
}

void CFxSpray::DrawSprite(WView* view, CFxParticle* p, const WVector& pos)
{
	if (p->sprite)
	{
		unsigned long color = GetParticleColor(view, pos, p, p->color);
		if (!(color & 0xff000000))
			return;

		p->sprite->SetColor(color);
		float width = p->ratio * p->size;
		p->sprite->SetRect(width, p->size, width * 0.5f, p->size * 0.5f);

		if (m_param.camOffset != 0.0f)
		{
			WVector dir = pos - view->camera.pivot;
			float dist = dir.SquareMagnitude();
			dir.Normalize();
			float depth = dir * view->camera.za;
			if (depth <= 0.0f)
				return;

			float nearDist = view->GetClipNearValue() / depth;
			if (sqrtf(dist) < nearDist + m_param.camOffset)
				dir = view->camera.pivot + dir * (nearDist + 0.01f);
			else
				dir = pos - dir * m_param.camOffset;
			p->sprite->SetPos(dir);
		}
		else
		{
			p->sprite->SetPos(pos);
		}

		if (m_param.bBillboard)
			p->sprite->Render(view, p->rot.x,
				m_param.blend[0] | m_fxBox.m_drawFlag | 0x20400000, p->frame);
		else
			p->sprite->Render(view, RotMat(p->rot) * GetBgRot(p),
				m_param.blend[0] | m_fxBox.m_drawFlag | 0x20000000, p->frame);
	}
	else if (p->pet)
	{
		p->pet->Display();
	}
	else
	{
		unsigned long color = GetParticleColor(view, pos, p, p->color);
		if (color & 0xff000000)
		{
			WVector dir = p->vel;
			dir = dir.Normalize() * 0.03f;
			view->DrawLine(pos, color, pos + dir, color, 0);
		}
	}
}

void CFxSpray::DrawTail(WView* view, CFxParticle* p, const WVector& pos)
{
	if (p->tail.IsEmpty())
		return;

	m_tailNum++;

	unsigned long color = 0xffffffff;
	int len = p->tail.GetLen();
	CFxParticle::sTail* tail = p->tail.GetTail(1);

	if (p->sprite)
	{
		unsigned char c[4];
		c[0] = p->bgra[0] * (len - 1) / len + m_param.tailColor[3] / len;
		c[1] = p->bgra[1] * (len - 1) / len + m_param.tailColor[2] / len;
		c[2] = p->bgra[2] * (len - 1) / len + m_param.tailColor[1] / len;
		c[3] = p->bgra[3] * (len - 1) / len + m_param.tailColor[0] / len;

		unsigned long result =
			GetParticleColor(view, tail->pos, p, *(unsigned long*)c);
		if (result & 0xff000000)
		{
			p->sprite->SetColor(result);
			p->sprite->SetPos(tail->pos);
			p->sprite->Render(view, p->rot.x,
				m_param.blend[1] | m_fxBox.m_drawFlag | 0x400000, tail->frame);
		}
	}
	else
	{
		unsigned char* c = (unsigned char*)&color;
		c[0] = p->bgra[0] * (len - 1) / len + m_param.tailColor[3] / len;
		c[1] = p->bgra[1] * (len - 1) / len + m_param.tailColor[2] / len;
		c[2] = p->bgra[2] * (len - 1) / len + m_param.tailColor[1] / len;
		c[3] = p->bgra[3] * (len - 1) / len + m_param.tailColor[0] / len;

		view->DrawLine(tail->pos, GetParticleColor(view, tail->pos, p, color),
			pos, GetParticleColor(view, tail->pos, p, p->color), 0);
	}

	for (int n = len - 2; n >= 0; n--)
	{
		m_tailNum++;

		CFxParticle::sTail* cur = p->tail.GetHead(n);
		CFxParticle::sTail* next = p->tail.GetHead(n + 1);

		unsigned char c[4];
		c[0] = m_param.tailColor[3] * (len - n) / len + p->bgra[0] * n / len;
		c[1] = m_param.tailColor[2] * (len - n) / len + p->bgra[1] * n / len;
		c[2] = m_param.tailColor[1] * (len - n) / len + p->bgra[2] * n / len;
		c[3] = m_param.tailColor[0] * (len - n) / len + p->bgra[3] * n / len;

		if (p->sprite)
		{
			unsigned long result =
				GetParticleColor(view, cur->pos, p, *(unsigned long*)c);
			if (!(result & 0xff000000))
				continue;
			p->sprite->SetColor(result);
			p->sprite->SetPos(cur->pos);
			p->sprite->Render(view, p->rot.x,
				m_param.blend[1] | m_fxBox.m_drawFlag | 0x400000, cur->frame);
		}
		else
		{
			view->DrawLine(cur->pos, *(unsigned long*)c, next->pos, color, 0);
		}

		color = *(unsigned long*)c;
	}
}

void CFxSpray::DrawBandana(WView* view, CFxParticle* p, const WVector& pos)
{
	if (p->bandana.IsEmpty())
		return;

	float total = 0.0f;
	int len = p->bandana.GetLen();
	for (int i = 1; i < len; i++)
	{
		CFxParticle::sBandana* bandana = p->bandana.GetHead(i);
		if (bandana)
			total += bandana->length;
	}

	unsigned long color = p->color;

	m_tailNum++;

	const WVector* tail = p->bandana.GetTail(1)->pos;
	float length =
		(float)((double)((pos - (tail[0] + tail[1]) * 0.5f).Magnitude()));
	float sum = length + total;

	CFxParticle::sBandana edge;
	edge.pos[0] = (tail[0] - tail[1]) * 0.5f + pos;
	edge.pos[1] = pos - (tail[0] - tail[1]) * 0.5f;

	unsigned char c[4];
	int len1 = len - 1;
	c[0] = p->bgra[0] * len1 / len + m_param.tailColor[3] / len;
	c[1] = p->bgra[1] * len1 / len + m_param.tailColor[2] / len;
	c[2] = p->bgra[2] * len1 / len + m_param.tailColor[1] / len;
	c[3] = p->bgra[3] < m_param.tailColor[0] / len + p->bgra[3] * len1 / len
		? p->bgra[3]
		: (unsigned char)(m_param.tailColor[0] / len + p->bgra[3] * len1 / len);

	for (int i = 0; i < 4; i++)
	{
		const WVector* pair;
		if (i & 2)
		{
			pair = tail;
			m_fxBox.m_vtx[i].tu =
				!m_param.bMonolith || sum == 0.0f ? 1.0f : (length / sum);
			m_fxBox.m_vtx[i].diffuse = GetParticleColor(view,
				(i & 1) == 1 ? pair[0] : pair[1], p, *(unsigned long*)c);
		}
		else
		{
			pair = edge.pos;
			m_fxBox.m_vtx[i].tu = 0.0f;
			m_fxBox.m_vtx[i].diffuse =
				GetParticleColor(view, i & 1 ? pair[0] : pair[1], p, p->color);
		}
		m_fxBox.m_vtx[i].SetPosition(i & 1 ? pair[0] : pair[1]);
	}

	if (m_fxBox.m_vtx[0].diffuse & 0xff000000 ||
		m_fxBox.m_vtx[1].diffuse & 0xff000000 ||
		m_fxBox.m_vtx[2].diffuse & 0xff000000 ||
		m_fxBox.m_vtx[3].diffuse & 0xff000000)
		view->DrawPolygonFan(m_fxBox.m_quad,
			m_param.tailTexHandle & 0x7ff | m_fxBox.m_drawFlag |
				m_param.blend[1] | 0x400000,
			0x400, false);

	for (int n = len - 2; n >= 0; n--)
	{
		m_tailNum++;

		CFxParticle::sBandana* cur = p->bandana.GetHead(n);
		CFxParticle::sBandana* next = p->bandana.GetHead(n + 1);

		unsigned char nc[4];
		int lenN = len - n;
		nc[0] = p->bgra[0] * n / len + m_param.tailColor[3] * lenN / len;
		nc[1] = p->bgra[1] * n / len + m_param.tailColor[2] * lenN / len;
		nc[2] = p->bgra[2] * n / len + m_param.tailColor[1] * lenN / len;
		nc[3] = p->bgra[3] <
				m_param.tailColor[0] * lenN / len + p->bgra[3] * n / len
			? p->bgra[3]
			: (unsigned char)(m_param.tailColor[0] * lenN / len +
				  p->bgra[3] * n / len);

		for (int i = 0; i < 4; i++)
		{
			const WVector* pair;
			if (i & 2)
			{
				pair = cur->pos;
				m_fxBox.m_vtx[i].tu = !m_param.bMonolith || sum == 0.0f
					? 1.0f
					: (length + next->length) / sum;
				m_fxBox.m_vtx[i].diffuse = GetParticleColor(view,
					i & 1 ? pair[0] : pair[1], p, *(unsigned long*)nc);
			}
			else
			{
				pair = next->pos;
				m_fxBox.m_vtx[i].tu =
					!m_param.bMonolith || sum == 0.0f ? 0.0f : length / sum;
				m_fxBox.m_vtx[i].diffuse =
					GetParticleColor(view, i & 1 ? pair[0] : pair[1], p, color);
			}
			m_fxBox.m_vtx[i].SetPosition(i & 1 ? pair[0] : pair[1]);
		}

		if (m_fxBox.m_vtx[0].diffuse & 0xff000000 ||
			m_fxBox.m_vtx[1].diffuse & 0xff000000 ||
			m_fxBox.m_vtx[2].diffuse & 0xff000000 ||
			m_fxBox.m_vtx[3].diffuse & 0xff000000)
			view->DrawPolygonFan(m_fxBox.m_quad,
				m_param.tailTexHandle & 0x7ff | m_fxBox.m_drawFlag |
					m_param.blend[1] | 0x400000,
				0x400, false);

		color = *(unsigned long*)nc;
		length += next->length;
	}
}

unsigned long CFxSpray::GetParticleColor(const WView* view, const WVector& pos,
	const CFxParticle* p, unsigned long color)
{
	if (!p)
		return 0xffffffff;

	float rate;
	if (m_param.distLimit[1] > 0.0f)
	{
		float dist = (view->camera.pivot - pos).Magnitude();
		if (dist < m_param.distLimit[0])
		{
			rate = 1.0f;
		}
		else if (dist > m_param.distLimit[1])
		{
			return 0;
		}
		else
		{
			rate = 1.0f -
				Between(0.0f,
					(dist - m_param.distLimit[0]) /
						(m_param.distLimit[1] - m_param.distLimit[0]),
					1.0f);
		}
	}
	else
	{
		rate = 1.0f;
	}

	unsigned long result = color;
	if (m_param.lighting)
		result = color & 0xff000000 | MulColor(color, m_param.lightColor);

	return ScaleAlpha(result, m_param.volMaxOpacity * p->volOpacity * rate);
}

void CFxSpray::SetLight(const LightSet& light)
{
	switch (m_param.lighting)
	{
	case 0:
		m_param.lightColor = 0xffffff;
		break;
	case 1:
		m_param.lightColor = light.diffuse;
		break;
	case 2:
		m_param.lightColor = light.ambient;
		break;
	case 3:
		m_param.lightColor = light.ambient2;
		break;
	}
}

void CFxSpray::SetWind(const WVector& wind)
{
	if (m_param.addWind[0] != 0.0f || m_param.addWind[1] != 0.0f)
		m_param.extWind = wind;
}

void CFxSpray::SetDistLimit(float nearDist, float farDist)
{
	m_param.distLimit[0] = nearDist;
	m_param.distLimit[1] = farDist;
}

void CFxSpray::Spawn(const sSpawn& spawn, CFxParticle* particle)
{
	particle->spawnNum++;

	CFxSpray* spray = m_fxBox.LoadSpray(spawn.name, NULL);
	if (!spray)
		return;

	spray->m_param.bSpawnAttached = spawn.attached;
	spray->m_spawnParent = spray->m_param.bSpawnAttached ? this : NULL;
	spray->m_parent = m_parent;
	spray->m_centerX = m_centerX;
	spray->m_centerY = m_centerY;

	if (m_parent)
	{
		spray->SetWind(m_parent->m_wind);
		m_parent->m_spray[spray] = spray;
	}

	spray->Pos() = particle->pos + particle->bgPos;
	spray->Vel() = particle->vel;
	spray->m_bActive = m_bActive;
	m_fxBox.AddSpray(spray);
}
