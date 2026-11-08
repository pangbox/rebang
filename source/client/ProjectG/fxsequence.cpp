#include "stdafx.h"
#include "fxbox.h"
#include "binstr.h"
#include "soundmanager.h"
#include "mathconsts.h"

CFxSequence::CFxSequence(CFxBox& box, bool temporary)
	: m_fxBox(box)
{
	m_bMark = true;
	m_wind.Reset();
	m_bActive = true;
	m_pos = m_vel = WVector(0, 0, 0);
	m_bDelete = temporary;
	m_time = 0;
	m_delay = 0;
	m_centerX = g_HUGE;
	m_centerY = g_HUGE;
	m_rot.Reset();
	memset(m_attachMat, 0, sizeof(m_attachMat));
	memset(m_attachPos, 0, sizeof(m_attachPos));
	Clear();
}

CFxSequence::~CFxSequence()
{
}

void CFxSequence::Clear()
{
	m_attacher = NULL;
	m_lines.clear();
	m_current = m_lines.end();
	m_lastError.text[0] = 0;
	m_lastSpray = NULL;
	memset(&m_defines, 0, sizeof(m_defines));
}

bool CFxSequence::LoadFile(const char* filename)
{
	Clear();
	std::map<std::string, CFxSequence*>::iterator i =
		m_fxBox.m_sequenceCache.find(filename);
	if (i == m_fxBox.m_sequenceCache.end())
	{
		cFile* file = m_fxBox.m_resrcMng->GetCFile(filename, 0x2000);
		if (!file)
			return false;
		char line[1024];
		int number = 0;
		while (file->Scan("%n", line))
			AddLine(line, ++number);
		CloseCFile(file);
		CFxSequence* cache = new CFxSequence(m_fxBox, true);
		cache->m_lines = m_lines;
		m_fxBox.m_sequenceCache[filename] = cache;
	}
	else
	{
		m_lines = (*i).second->m_lines;
		m_current = m_lines.begin();
	}
	SetLineTime();
	return true;
}

void CFxSequence::AddLine(const char* text, int number)
{
	if (!*text || *text == ';' || m_fxBox.PreprocessDefines(m_defines, text))
		return;
	cTokenV tokens;
	tokens.Init(text, strlen(text));
	char start[260], end[260], command[260];
	tokens.GetToken(start, ":", 1);
	tokens.GetToken(command, "\r\n", 2);
	cTokenS times;
	times.Init(start, strlen(start));
	times.GetToken(start, "-", 1);
	times.GetToken(end, "-", 1);
	TrimA(start, " \t\n\r\0");
	TrimA(end, " \t\n\r\0");
	TrimA(command, " \t\n\r\0");
	sLine line;
	line.number = number;
	if (isalpha(start[0]))
	{
		line.time = -1;
		strcpy(line.text, start);
	}
	else if (!start[0])
	{
		line.time = -2;
		strcpy(line.text, command);
	}
	else
	{
		line.start = atoi(start);
		line.end = end[0] ? atoi(end) : line.start;
		line.time = line.start;
		strcpy(line.text, command);
	}
	m_lines.push_back(line);
	if (m_current == m_lines.end())
		m_current = m_lines.begin();
}

const CFxSequence::sLine& CFxSequence::GetLastError()
{
	return m_lastError;
}

bool CFxSequence::ParseLine(const char* text, bool& error)
{
	error = false;
	if (!*text || *text == ';')
		return true;
	cTokenV tokens;
	tokens.Init(text, strlen(text));
	char command[260], args[16][260];
	tokens.GetToken(command, "=", 1);
	TrimA(command, " \t\n\r\0");
	for (int i = 0; i < 16; ++i)
	{
		tokens.GetToken(args[i], ",", 1);
		TrimA(args[i], " \t()%");
		m_fxBox.ProcessDefines(m_defines, args[i]);
	}
	if (!command[0])
		return true;
	if (!strcmpi(command, "spray"))
	{
		if (m_bActive)
		{
			CFxSpray* spray = m_fxBox.LoadSpray(args[0]);
			if (!spray)
			{
				error = true;
				return false;
			}
			spray->m_param.bStrongBond = !strcmpi(args[7], "strongbond");
			spray->m_parent = this;
			spray->m_centerX = m_centerX;
			spray->m_centerY = m_centerY;
			spray->m_bMark = m_bMark;
			spray->SetWind(m_wind);
			spray->Pos() = WVector(atof(args[1]), atof(args[2]), atof(args[3]));
			spray->Vel() = RotVec(
				WVector(atof(args[4]), atof(args[5]), atof(args[6])), m_rot);
			spray->SetActive(m_bActive);
			m_spray[spray] = spray;
			m_fxBox.AddSpray(spray);
			m_lastSpray = spray;
		}
	}
	else if (!strcmpi(command, "add_spawn"))
	{
		if (m_lastSpray)
		{
			CFxSpray::sSpawn spawn;
			strcpy(spawn.name, args[0]);
			spawn.attached = !strcmpi(args[1], "attached");
			if (!strcmpi(args[2], "interval"))
			{
				spawn.type = 0;
				spawn.time = m_fxBox.ParseNumber(args[3]);
				spawn.count = (int)m_fxBox.ParseNumber(args[4]);
			}
			else if (!strcmpi(args[2], "age"))
			{
				spawn.type = 1;
				spawn.time = m_fxBox.ParseNumber(args[3]);
			}
			else if (!strcmpi(args[2], "ondeath"))
				spawn.type = 2;
			else if (!strcmpi(args[2], "onground"))
				spawn.type = 3;
			m_lastSpray->m_param.spawn.push_back(spawn);
		}
	}
	else if (!strcmpi(command, "add_revolution"))
	{
		if (m_lastSpray)
		{
			CFxSpray::sRevol revol;
			sscanf(args[0], "%f", &revol.center.x);
			sscanf(args[1], "%f", &revol.center.y);
			sscanf(args[2], "%f", &revol.center.z);
			sscanf(args[3], "%f", &revol.axis.x);
			sscanf(args[4], "%f", &revol.axis.y);
			sscanf(args[5], "%f", &revol.axis.z);
			revol.start = (int)m_fxBox.ParseNumber(args[6]);
			revol.startSpeed = m_fxBox.ParseNumber(args[7]);
			revol.end = (int)m_fxBox.ParseNumber(args[8]);
			revol.endSpeed = m_fxBox.ParseNumber(args[9]);
			m_lastSpray->m_param.revolution.push_back(revol);
		}
	}
	else if (!strcmpi(command, "add_rotation"))
	{
		if (m_lastSpray)
		{
			CFxSpray::sNozzleRot rot;
			sscanf(args[0], "%f", &rot.axis.x);
			sscanf(args[1], "%f", &rot.axis.y);
			sscanf(args[2], "%f", &rot.axis.z);
			rot.start = (int)m_fxBox.ParseNumber(args[3]);
			rot.startSpeed = m_fxBox.ParseNumber(args[4]);
			rot.end = (int)m_fxBox.ParseNumber(args[5]);
			rot.endSpeed = m_fxBox.ParseNumber(args[6]);
			m_lastSpray->m_param.sprayRotation.push_back(rot);
		}
	}
	else if (!strcmpi(command, "add_nrotation"))
	{
		if (m_lastSpray)
		{
			CFxSpray::sNozzleRot rot;
			sscanf(args[0], "%f", &rot.axis.x);
			sscanf(args[1], "%f", &rot.axis.y);
			sscanf(args[2], "%f", &rot.axis.z);
			rot.start = (int)m_fxBox.ParseNumber(args[3]);
			rot.startSpeed = m_fxBox.ParseNumber(args[4]);
			rot.end = (int)m_fxBox.ParseNumber(args[5]);
			rot.endSpeed = m_fxBox.ParseNumber(args[6]);
			m_lastSpray->m_param.nozzleRotation.push_back(rot);
		}
	}
	else if (!strcmpi(command, "add_prevolution"))
	{
		if (m_lastSpray)
		{
			CFxSpray::sRevol revol;
			sscanf(args[0], "%f", &revol.center.x);
			sscanf(args[1], "%f", &revol.center.y);
			sscanf(args[2], "%f", &revol.center.z);
			sscanf(args[3], "%f", &revol.axis.x);
			sscanf(args[4], "%f", &revol.axis.y);
			sscanf(args[5], "%f", &revol.axis.z);
			revol.start = (int)m_fxBox.ParseNumber(args[6]);
			revol.startSpeed = m_fxBox.ParseNumber(args[7]);
			revol.end = (int)m_fxBox.ParseNumber(args[8]);
			revol.endSpeed = m_fxBox.ParseNumber(args[9]);
			m_lastSpray->m_param.preRevolution.push_back(revol);
		}
	}
	else if (!stricmp(command, "AttachedToParent"))
	{
		if (m_lastSpray)
		{
			m_lastSpray->m_param.bAttachedToParent = false;
			if (!stricmp(args[0], "on"))
				m_lastSpray->m_param.bAttachedToParent = true;
			int x = 0, y = 0, z = 0;
			sscanf(args[1], "%d", &x);
			sscanf(args[2], "%d", &y);
			sscanf(args[3], "%d", &z);
			m_lastSpray->m_param.attachedAxis = 0;
			if (x)
				m_lastSpray->m_param.attachedAxis |= 1;
			if (y)
				m_lastSpray->m_param.attachedAxis |= 2;
			if (z)
				m_lastSpray->m_param.attachedAxis |= 4;
		}
	}
	else if (!strcmpi(command, "sound"))
	{
		float distance = -1;
		char name[260];
		strcpy(name, args[0]);
		char* ext = strstr(name, ".wav");
		if (ext)
			*ext = 0;
		sscanf(args[1], "%f", &distance);
		if (distance < 0)
		{
			g_audio->PlaySfx(name);
		}
		else
		{
			WVector relative = m_pos - g_camera.pivot;
			g_audio->PlaySfx(name, &relative, 5, NULL, NULL, 0, distance);
		}
	}
	else if (!strcmpi(command, "goto"))
	{
		std::list<sLine>::iterator i;
		for (i = m_lines.begin(); i != m_lines.end(); ++i)
		{
			if (!strcmpi((*i).text, args[0]))
			{
				SetLineTime();
				m_current = ++i;
				float time = (*m_current).start;
				float diff = m_time - time;
				if (diff < m_fxBox.m_frameTime * 2 &&
					diff > m_fxBox.m_frameTime * -2)
					return false;
				m_time = time;
				break;
			}
		}
	}
	else if (!strcmpi(command, "stop"))
	{
		m_current = m_lines.end();
	}
	else
	{
		error = true;
		return false;
	}
	return true;
}

void CFxSequence::SetLineTime()
{
	for (std::list<sLine>::iterator i = m_lines.begin(); i != m_lines.end();
		++i)
	{
		sLine& line = *i;
		if (line.time == -1 || line.time == -2)
			continue;
		if (line.end != line.start)
			line.time = line.start + rand() % (line.end - line.start);
		std::list<sLine>::iterator first = i, prev = i;
		for (; first != m_lines.begin(); --first)
		{
			--prev;
			if ((*prev).time == -1 || (*prev).time == -2)
				break;
		}
		const sLine& moving = *i;
		if (first != i)
		{
			while (first != i)
			{
				if ((*first).time > line.time)
				{
					m_lines.insert(first, moving);
					i = m_lines.erase(i);
					--i;
					break;
				}
				++first;
			}
		}
	}
}

bool CFxSequence::Process(float elapsed, bool& error)
{
	error = false;
	if (!m_bActive)
	{
		if (m_delay > 0)
		{
			m_delay -= elapsed;
			if (m_delay < 0)
				m_bActive = true;
			else
				return true;
		}
		else
			return true;
	}
	bool result;
	for (;;)
	{
		if (m_current == m_lines.end())
		{
			result = false;
			break;
		}
		const sLine& line = *m_current;
		if (line.time == -1)
		{
			++m_current;
			continue;
		}
		if (line.time != -2 && line.time > (unsigned int)(int)(m_time + .5f))
		{
			result = true;
			break;
		}
		++m_current;
		if (line.text[0])
		{
			if (!ParseLine(line.text, error))
			{
				--m_current;
				result = true;
				break;
			}
			if (error)
			{
				m_lastError = line;
				result = true;
				break;
			}
		}
	}
	m_time += elapsed;
	return result;
}

void CFxSequence::DeleteChildSpray(CFxSpray* spray)
{
	std::map<CFxSpray*, CFxSpray*>::iterator i = m_spray.find(spray);
	if (i != m_spray.end())
	{
		if ((*i).second == m_lastSpray)
			m_lastSpray = NULL;
		m_spray.erase(i);
	}
}

void CFxSequence::SetLight(const LightSet& light)
{
	for (std::map<CFxSpray*, CFxSpray*>::iterator i = m_spray.begin();
		i != m_spray.end(); ++i)
		(*i).second->SetLight(light);
}

void CFxSequence::SetWind(const WVector& wind)
{
	for (std::map<CFxSpray*, CFxSpray*>::iterator i = m_spray.begin();
		i != m_spray.end(); ++i)
		(*i).second->SetWind(wind);
}

void CFxSequence::SetDistLimit(float nearLimit, float farLimit)
{
	for (std::map<CFxSpray*, CFxSpray*>::iterator i = m_spray.begin();
		i != m_spray.end(); ++i)
		(*i).second->SetDistLimit(nearLimit, farLimit);
}
