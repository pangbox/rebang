#include "stdafx.h"
#include "miles.h"
#include "mss.h"
#include "wevent.h"

cFile* WMilesSoundSystem::g_files[16];
int WMilesSoundSystem::g_offset[16];

static WSyncEvent soundEvent;

bool WMilesSoundSystem::Init(HWND hwnd, int rate, int bits)
{
	m_hDriver = AIL_open_digital_driver(rate, bits, 16, 1);
	if (!m_hDriver)
	{
		char buffer[256];
		sprintf(buffer,
			"\xbb\xe7\xbf\xee\xb5\xe5 \xb5\xe5\xb6\xf3\xc0\xcc\xb9\xf6\xb8\xa6 \xbf\xad \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9: %s",
			AIL_last_error());
		MessageBox(NULL, buffer, "Miles Sound System", 0);
		return false;
	}
	AIL_set_DirectSound_HWND(m_hDriver, hwnd);
	InitMixer();
	return true;
}

WMilesSoundSystem::WMilesSoundSystem(HWND hwnd, const char* redistDir,
	bool option, int param1, int param2, int param3)
	: m_hDriver(NULL),
	  m_unknown20(0),
	  m_sample2DList(16, 16),
	  m_sample3DList(16, 16)
{
	m_unknown04 = 1.0f;
	m_volume2D = 1.0f;
	m_volume3D = 1.0f;
	m_lastHandle = 0;
	m_hMixer = NULL;
	m_nearDistance = 0;
	m_farDistance = 100;
	m_bFlipX = false;
	m_bUnknown19 = false;
	m_bAllowOverlap = false;
	for (int i = 0; i < 1024; ++i)
	{
		m_sound[i].sample2d = NULL;
		m_sound[i].data2d = NULL;
		m_sound[i].sample3d = NULL;
		m_sound[i].data3d = NULL;
		m_sound[i].stream = NULL;
		m_sound[i].streamPos = 0;
		m_sound[i].type = 3;
	}
	for (int i = 0; i < 8; ++i)
		m_stream[i] = 0;
	for (int i = 0; i < 8; ++i)
		m_streamOwner[i] = 0;
	for (int i = 0; i < 16; ++i)
		m_sample2DOwner[i] = 0;
	for (int i = 0; i < 16; ++i)
		m_sample2D[i] = 0;
	for (int i = 0; i < 16; ++i)
		m_sample3DOwner[i] = 0;
	for (int i = 0; i < 16; ++i)
		m_sample3D[i] = 0;
	if (redistDir)
		AIL_set_redist_directory(redistDir);
	for (int i = 0; i < 16; ++i)
		g_files[i] = 0;
	AIL_set_file_callbacks(Open_callback, Close_callback, Seek_callback,
		Read_callback);
	if (!AIL_startup())
	{
		char buffer[256];
		sprintf(buffer,
			"\xbb\xe7\xbf\xee\xb5\xe5\xb8\xa6 \xc3\xca\xb1\xe2\xc8\xad\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9: %s",
			AIL_last_error());
		MessageBox(NULL, buffer, "Miles Sound System", 0);
	}
}

void WMilesSoundSystem::SetReverb()
{
	AIL_set_digital_master_reverb(m_hDriver, 1.0f, 0.0f, 0.0f);
}

bool WMilesSoundSystem::InitMixer()
{
	unsigned int n = mixerGetNumDevs();
	m_hMixer = NULL;
	if (n)
	{
		if (!mixerOpen(&m_hMixer, 0, 0, 0, 0))
		{
			memset(&m_mixerLine, 0, sizeof(m_mixerLine));
			m_mixerLine.cbStruct = sizeof(m_mixerLine);
			m_mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
			if (!mixerGetLineInfo((HMIXEROBJ)m_hMixer, &m_mixerLine,
					MIXER_GETLINEINFOF_COMPONENTTYPE))
			{
				MIXERLINECONTROLS controls;
				controls.dwLineID = m_mixerLine.dwLineID;
				memset(&m_mixerControl, 0, sizeof(m_mixerControl));
				controls.pamxctrl = &m_mixerControl;
				m_mixerControl.cbStruct = sizeof(m_mixerControl);
				controls.cbStruct = sizeof(controls);
				controls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
				controls.cControls = 1;
				controls.cbmxctrl = sizeof(m_mixerControl);
				if (!mixerGetLineControls((HMIXEROBJ)m_hMixer, &controls,
						MIXER_GETLINECONTROLSF_ONEBYTYPE |
							MIXER_OBJECTF_HMIXER))
					return true;
			}
		}
	}
	return false;
}

void WMilesSoundSystem::SetMixer(float volume)
{
	MIXERCONTROLDETAILS details;
	MIXERCONTROLDETAILS_UNSIGNED value;
	value.dwValue =
		(unsigned long)((m_mixerControl.Bounds.dwMaximum - 1) * volume);
	details.cbStruct = sizeof(details);
	details.cChannels = 1;
	details.cMultipleItems = 0;
	details.cbDetails = sizeof(value);
	details.dwControlID = m_mixerControl.dwControlID;
	details.paDetails = &value;
	mixerSetControlDetails((HMIXEROBJ)m_hMixer, &details, MIXER_OBJECTF_HMIXER);
}

void WMilesSoundSystem::CloseMixer()
{
	if (m_hMixer)
		mixerClose(m_hMixer);
}

WMilesSoundSystem::~WMilesSoundSystem()
{
	CloseMixer();
	ClearAllSamples();
	ClearAllSounds();
	ClearAllStreams();
	for (unsigned int i = 0; i < 16; ++i)
		if (g_files[i])
		{
			CloseCFile(g_files[i]);
			g_files[i] = NULL;
		}
	if (m_hDriver)
		AIL_digital_handle_release(m_hDriver);
	if (m_hDriver)
		AIL_close_digital_driver(m_hDriver);
	AIL_shutdown();
}

void WMilesSoundSystem::ClearAllStreams()
{
	for (int i = 0; i < 8; ++i)
	{
		if (m_stream[i])
		{
			AIL_close_stream(m_stream[i]);
			m_stream[i] = NULL;
			if (m_streamOwner[i])
			{
				m_sound[m_streamOwner[i]].stream = NULL;
				m_sound[m_streamOwner[i]].streamPos = 0;
				m_streamOwner[i] = 0;
			}
		}
	}
}

void WMilesSoundSystem::ClearAllSamples()
{
	for (int i = 0; i < 16; ++i)
	{
		if (m_sample2D[i])
		{
			AIL_stop_sample(m_sample2D[i]);
			AIL_release_sample_handle(m_sample2D[i]);
			m_sample2D[i] = NULL;
			if (m_sample2DOwner[i] > 0)
			{
				m_sound[m_sample2DOwner[i]].sample2d = NULL;
				m_sample2DOwner[i] = 0;
			}
		}
	}
	memset(m_sample2D, 0, sizeof(m_sample2D));
	for (int i = 0; i < 16; ++i)
	{
		if (m_sample3D[i])
		{
			AIL_stop_sample(m_sample3D[i]);
			AIL_release_sample_handle(m_sample3D[i]);
			m_sample3D[i] = NULL;
			if (m_sample3DOwner[i] > 0)
			{
				m_sound[m_sample3DOwner[i]].sample3d = NULL;
				m_sample3DOwner[i] = 0;
			}
		}
	}
	memset(m_sample3D, 0, sizeof(m_sample3D));
}

void WMilesSoundSystem::ClearAllSounds()
{
	for (msample2d* sample = m_sample2DList.Start(); sample;
		sample = m_sample2DList.Next())
	{
		AIL_mem_free_lock(sample->data);
		delete[] (char*)sample;
	}
	m_sample2DList.Reset();
	for (msample3d* sample = m_sample3DList.Start(); sample;
		sample = m_sample3DList.Next())
	{
		AIL_mem_free_lock(sample->data);
		delete[] (char*)sample;
	}
	m_sample3DList.Reset();
}

int WMilesSoundSystem::GetCPUpercent()
{
	return m_hDriver ? AIL_digital_CPU_percent(m_hDriver) : 0;
}

void WMilesSoundSystem::SpeakerType(w_speaker_type type)
{
	int channels, physical, falloff = 0, config;
	switch (type)
	{
	case W_SPEAKER_2CH:
		physical = channels = 2;
		config = 2;
		break;
	case W_SPEAKER_HEADPHONE:
		physical = channels = 2;
		config = 32;
		break;
	case W_SPEAKER_SURROUND:
		channels = 2;
		physical = 3;
		config = 48;
		break;
	case W_SPEAKER_4CH:
		physical = channels = 4;
		config = 80;
		break;
	case W_SPEAKER_51CH:
		physical = channels = 6;
		config = 96;
		break;
	case W_SPEAKER_71CH:
		physical = channels = 8;
		config = 128;
		break;
	}
	const void* speakers = AIL_speaker_configuration(m_hDriver, &channels,
		&physical, &falloff, &config);
	AIL_set_speaker_configuration(m_hDriver, speakers, 0, 0);
}

void WMilesSoundSystem::SetBalance(int balance)
{
	if (balance < 128)
	{
		m_volume2D = 1.0f;
		m_volume3D = (balance < 0 ? 0.0f : balance / 128.0f);
	}
	else
	{
		m_volume2D = (balance > 256 ? 1.0f : (256 - balance) / 128.0f);
		m_volume3D = 1.0f;
	}
}

void WMilesSoundSystem::Activate(bool active)
{
}

void WMilesSoundSystem::ClearIdleSound(int age)
{
	WList<msample2d*> idle2d(16, 16);
	WList<msample3d*> idle3d(16, 16);
	for (msample2d* sample = m_sample2DList.Start(); sample;
		sample = m_sample2DList.Next())
	{
		if (!sample->refCount && ++sample->idleCount >= age + 1)
			idle2d += sample;
	}
	for (msample2d* sample = idle2d.Start(); sample; sample = idle2d.Next())
	{
		m_sample2DList -= sample;
		AIL_mem_free_lock(sample->data);
		delete[] (char*)sample;
	}
	for (msample3d* sample = m_sample3DList.Start(); sample;
		sample = m_sample3DList.Next())
	{
		if (!sample->refCount && ++sample->idleCount >= age + 1)
			idle3d += sample;
	}
	for (msample3d* sample = idle3d.Start(); sample; sample = idle3d.Next())
	{
		m_sample3DList -= sample;
		AIL_mem_free_lock(sample->data);
		delete[] (char*)sample;
	}
}

int WMilesSoundSystem::Load(char* filename, int type)
{
	int handle = GetEmptyHandle();
	if (handle > 0)
	{
		if (type == 2)
		{
			unsigned int i;
			for (i = 0; m_stream[i] && i < 8; ++i)
			{
				unsigned int status = AIL_stream_status(m_stream[i]);
				if (status == 2 || status == 8)
					break;
			}
			if (i >= 8)
				return 0;
			if (m_stream[i])
			{
				if (m_sound[m_streamOwner[i]].stream == m_stream[i])
					m_sound[m_streamOwner[i]].stream = NULL;
				AIL_close_stream(m_stream[i]);
				m_stream[i] = NULL;
			}
			HSTREAM stream = AIL_open_stream(m_hDriver, filename, 0);
			if (stream)
			{
				m_stream[i] = stream;
				m_streamOwner[i] = handle;
				AIL_set_stream_user_data(stream, i,
					AIL_sample_playback_rate(AIL_stream_sample_handle(stream)));
				m_sound[handle].stream = stream;
				m_sound[handle].type = 2;
				SetVolume(handle, 1);
				return handle;
			}
		}
		else if (type == 1)
		{
			msample3d* sample = LoadSample3D(filename);
			if (sample)
			{
				m_sound[handle].data3d = sample;
				m_sound[handle].sample3d = NULL;
				m_sound[handle].nearDistance = m_nearDistance;
				m_sound[handle].farDistance = m_farDistance;
				m_sound[handle].type = 1;
				SetVolume(handle, 1);
				return handle;
			}
		}
		else
		{
			msample2d* sample = LoadSample2D(filename);
			if (sample)
			{
				m_sound[handle].data2d = sample;
				m_sound[handle].sample2d = NULL;
				m_sound[handle].type = 0;
				SetVolume(handle, 1);
				return handle;
			}
		}
	}
	return 0;
}

void WMilesSoundSystem::DestroySoundBuffer(int handle)
{
	Stop(handle);
	switch (m_sound[handle].type)
	{
	case 0:
		Release2D(m_sound[handle].data2d);
		m_sound[handle].data2d = NULL;
		break;
	case 1:
		Release3D(m_sound[handle].data3d);
		m_sound[handle].data3d = NULL;
		break;
	case 2:
		if (m_sound[handle].stream)
		{
			AIL_close_stream(m_sound[handle].stream);
			for (unsigned int i = 0; i < 8; ++i)
			{
				if (m_stream[i] == m_sound[handle].stream)
				{
					m_stream[i] = NULL;
					if (m_streamOwner[i])
					{
						m_sound[m_streamOwner[i]].stream = NULL;
						m_streamOwner[i] = 0;
					}
					break;
				}
				if (m_streamOwner[i] == handle)
				{
					m_streamOwner[i] = 0;
					AIL_close_stream(m_stream[i]);
					m_stream[i] = NULL;
				}
			}
			m_sound[handle].stream = NULL;
		}
		break;
	}
	m_sound[handle].type = 3;
}

void WMilesSoundSystem::Volume(float volume)
{
	SetMixer(volume);
}

void WMilesSoundSystem::Play(int handle, int loop, bool replace)
{
	switch (m_sound[handle].type)
	{
	case 0:
		if (m_sound[handle].data2d->data)
		{
			int last = m_sound[handle].data2d->lastHandle;
			m_sound[handle].data2d->lastHandle = handle;
			if (!m_bAllowOverlap && replace && m_sound[last].type == 0 &&
				m_sound[last].data2d == m_sound[handle].data2d)
				Stop(last);
			HSAMPLE sample = GetAllocateSample(handle, loop != 0);
			if (sample)
			{
				AIL_set_named_sample_file(sample, m_sound[handle].data2d->name,
					m_sound[handle].data2d->data, m_sound[handle].data2d->size,
					0);
				AIL_set_sample_loop_count(sample, !loop);
				AIL_set_sample_volume_pan(sample, m_sound[handle].volume, 0.5f);
				AIL_start_sample(sample);
			}
		}
		break;
	case 1:
	{
		int last = m_sound[handle].data3d->lastHandle;
		m_sound[handle].data3d->lastHandle = handle;
		if (!m_bAllowOverlap && replace && m_sound[last].type == 1 &&
			m_sound[last].data3d == m_sound[handle].data3d)
			Stop(last);
		HSAMPLE sample = GetAllocateSample3D(handle, loop != 0);
		if (sample)
		{
			AIL_set_sample_file(sample, m_sound[handle].data3d->data, 0);
			AIL_set_sample_loop_count(sample, !loop);
			AIL_set_sample_3D_distances(sample, m_sound[handle].nearDistance,
				m_sound[handle].farDistance, 0);
			AIL_set_sample_volume_pan(sample, m_sound[handle].volume, 0.5f);
			AIL_set_sample_3D_position(m_sound[handle].sample3d,
				m_sound[handle].pos[0], m_sound[handle].pos[1],
				m_sound[handle].pos[2]);
			AIL_start_sample(sample);
		}
	}
	break;
	case 2:
		if (m_sound[handle].stream)
		{
			AIL_set_stream_loop_count(m_sound[handle].stream, !loop);
			HSAMPLE sample = AIL_stream_sample_handle(m_sound[handle].stream);
			AIL_set_sample_volume_pan(sample, m_sound[handle].volume, 0.5f);
			AIL_start_stream(m_sound[handle].stream);
		}
		break;
	}
}

void WMilesSoundSystem::Resume(int handle, int loop)
{
	switch (m_sound[handle].type)
	{
	case 2:
		if (m_sound[handle].stream)
		{
			AIL_set_stream_loop_count(m_sound[handle].stream, !loop);
			HSAMPLE sample = AIL_stream_sample_handle(m_sound[handle].stream);
			AIL_set_sample_volume_pan(sample, m_sound[handle].volume, 0.5f);
			AIL_set_stream_position(m_sound[handle].stream,
				m_sound[handle].streamPos);
			AIL_start_stream(m_sound[handle].stream);
		}
	}
}

void WMilesSoundSystem::Stop(int handle)
{
	switch (m_sound[handle].type)
	{
	case 0:
		if (m_sound[handle].sample2d)
		{
			for (int i = 0; i < 16; ++i)
				if (m_sample2DOwner[i] == handle)
				{
					m_sample2DOwner[i] = 0;
					AIL_stop_sample(m_sample2D[i]);
				}
			m_sound[handle].sample2d = NULL;
		}
		break;
	case 1:
		if (m_sound[handle].sample3d)
		{
			for (int i = 0; i < 16; ++i)
				if (m_sample3DOwner[i] == handle)
				{
					m_sample3DOwner[i] = 0;
					AIL_stop_sample(m_sample3D[i]);
				}
			m_sound[handle].sample3d = NULL;
		}
		break;
	case 2:
		AIL_pause_stream(m_sound[handle].stream, 1);
		m_sound[handle].streamPos = AIL_stream_position(m_sound[handle].stream);
		break;
	}
}

void WMilesSoundSystem::SetPitch(int handle, int rate)
{
	AIL_set_sample_playback_rate(m_sound[handle].sample2d, rate);
}

int WMilesSoundSystem::GetPitch(int handle)
{
	return AIL_sample_playback_rate(m_sound[handle].sample2d);
}

void WMilesSoundSystem::SetVolume(int handle, float volume)
{
	switch (m_sound[handle].type)
	{
	case 0:
		volume *= m_volume2D;
		m_sound[handle].volume = volume;
		if (m_sound[handle].sample2d)
			AIL_set_sample_volume_pan(m_sound[handle].sample2d, volume, 0.5f);
		break;
	case 1:
		volume *= m_volume3D;
		m_sound[handle].volume = volume;
		if (m_sound[handle].sample3d)
			AIL_set_sample_volume_pan(m_sound[handle].sample3d, volume, 0.5f);
		break;
	case 2:
		m_sound[handle].volume = volume * m_volume2D;
		if (m_sound[handle].stream)
		{
			HSAMPLE sample = AIL_stream_sample_handle(m_sound[handle].stream);
			AIL_set_sample_volume_pan(sample, m_sound[handle].volume, 0.5f);
		}
		break;
	}
}

void WMilesSoundSystem::SetNearFar(int handle, float nearDist, float farDist)
{
	m_sound[handle].nearDistance = nearDist;
	m_sound[handle].farDistance = farDist;
	if (m_sound[handle].sample3d)
		AIL_set_sample_3D_distances(m_sound[handle].sample3d, nearDist, farDist,
			0);
}

void WMilesSoundSystem::SetPosition(int handle, float x, float y, float z)
{
	m_sound[handle].pos[0] = m_bFlipX ? -x : x;
	m_sound[handle].pos[1] = y;
	m_sound[handle].pos[2] = z;
	if (m_sound[handle].sample3d)
		AIL_set_sample_3D_position(m_sound[handle].sample3d, x, y, z);
}

bool WMilesSoundSystem::IsPlaying(int handle)
{
	switch (m_sound[handle].type)
	{
	case 0:
		if (m_sound[handle].sample2d &&
			AIL_sample_status(m_sound[handle].sample2d) == 4)
			return true;
		return false;
	case 1:
		if (m_sound[handle].sample3d &&
			AIL_sample_status(m_sound[handle].sample3d) == 4)
			return true;
		return false;
	case 2:
		if (m_sound[handle].stream)
		{
			unsigned int status = AIL_stream_status(m_sound[handle].stream);
			if (status != 2 && status != 8)
				return true;
			return false;
		}
		break;
	}
	return false;
}

int WMilesSoundSystem::GetPosition(int handle)
{
	switch (m_sound[handle].type)
	{
	case 0:
		if (m_sound[handle].sample2d &&
			AIL_sample_status(m_sound[handle].sample2d) == 4)
			return AIL_sample_position(m_sound[handle].sample2d);
		break;
	case 1:
		return AIL_sample_status(m_sound[handle].sample3d) == 4
			? AIL_sample_position(m_sound[handle].sample3d)
			: -1;
	case 2:
		if (AIL_stream_status(m_sound[handle].stream) == 4)
		{
			int position;
			AIL_stream_ms_position(m_sound[handle].stream, NULL, &position);
			return position / 1000;
		}
		break;
	}
	return -1;
}

int WMilesSoundSystem::GetLength(int handle)
{
	switch (m_sound[handle].type)
	{
	case 0:
	case 1:
		return 0;
	case 2:
	{
		int rate, type, length, memory;
		AIL_stream_info(m_sound[handle].stream, &rate, &type, &length, &memory);
		return length / rate;
	}
	}
	return -1;
}

WMilesSoundSystem::msample2d* WMilesSoundSystem::LoadSample2D(char* filename)
{
	msample2d* sample = m_sample2DList.Find(filename);
	if (!sample)
	{
		sample = (msample2d*)new char[sizeof(msample2d) + strlen(filename)];
		sample->size = AIL_file_size(filename);
		sample->data = sample->size > 0 ? AIL_file_read(filename, NULL) : NULL;
		sample->refCount = 0;
		sample->lastHandle = 0;
		strcpy(sample->name, filename);
		m_sample2DList.AddItem(sample, sample->name, false);
	}
	++sample->refCount;
	return sample;
}

void WMilesSoundSystem::Release2D(msample2d* sample)
{
	--sample->refCount;
	sample->idleCount = 0;
}

void WMilesSoundSystem::LoadSample3D(msample3d* sample)
{
	int size = AIL_file_size(sample->name);
	sample->data = AIL_file_read(sample->name, NULL);
	void* decompressed;
	switch (AIL_file_type(sample->data, size))
	{
	case 2:
	{
		AILSOUNDINFO info;
		AIL_WAV_info(sample->data, &info);
		AIL_decompress_ADPCM(&info, &decompressed, NULL);
		AIL_mem_free_lock(sample->data);
		sample->data = decompressed;
		break;
	}
	case 13:
		AIL_decompress_ASI(sample->data, size, sample->name, &decompressed,
			NULL, NULL);
		AIL_mem_free_lock(sample->data);
		sample->data = decompressed;
		break;
	}
}

WMilesSoundSystem::msample3d* WMilesSoundSystem::LoadSample3D(char* filename)
{
	msample3d* sample = m_sample3DList.Find(filename);
	if (!sample)
	{
		sample = (msample3d*)new char[sizeof(msample3d) + strlen(filename)];
		sample->refCount = 0;
		sample->idleCount = 0;
		sample->lastHandle = 0;
		strcpy(sample->name, filename);
		LoadSample3D(sample);
		m_sample3DList.AddItem(sample, sample->name, false);
	}
	++sample->refCount;
	return sample;
}

void WMilesSoundSystem::Release3D(msample3d* sample)
{
	--sample->refCount;
	sample->idleCount = 0;
}

void WMilesSoundSystem::Process()
{
}

int WMilesSoundSystem::GetEmptyHandle()
{
	for (int i = 0; i < 1024; ++i)
	{
		m_lastHandle = (m_lastHandle + 1) & 1023;
		if (m_lastHandle > 0 && m_sound[m_lastHandle].type == 3)
			return m_lastHandle;
	}
	return 0;
}

void WMilesSoundSystem::ClearIdleSample()
{
	for (int i = 0; i < 16; ++i)
	{
		if (m_sample2D[i])
		{
			unsigned int status = AIL_sample_status(m_sample2D[i]);
			if (status == 2 || status == 8)
			{
				if (m_sample2DOwner[i] > 0)
				{
					if (m_sound[m_sample2DOwner[i]].sample2d == m_sample2D[i])
						m_sound[m_sample2DOwner[i]].sample2d = NULL;
					m_sample2DOwner[i] = 0;
				}
				AIL_stop_sample(m_sample2D[i]);
				AIL_release_sample_handle(m_sample2D[i]);
				m_sample2D[i] = NULL;
			}
		}
	}
	for (int i = 0; i < 16; ++i)
	{
		if (m_sample3D[i])
		{
			unsigned int status = AIL_sample_status(m_sample3D[i]);
			if (status == 2 || status == 8)
			{
				if (m_sample3DOwner[i] > 0)
				{
					if (m_sound[m_sample3DOwner[i]].sample3d == m_sample3D[i])
						m_sound[m_sample3DOwner[i]].sample3d = NULL;
					m_sample3DOwner[i] = 0;
				}
				AIL_stop_sample(m_sample3D[i]);
				AIL_release_sample_handle(m_sample3D[i]);
				m_sample3D[i] = NULL;
			}
		}
	}
}

HSAMPLE WMilesSoundSystem::GetAllocateSample(int handle, bool loop)
{
	unsigned int i;
	for (i = 0; i < 16; ++i)
	{
		if (!m_sample2D[i])
		{
			m_sample2D[i] = AIL_allocate_sample_handle(m_hDriver);
			break;
		}
		unsigned int status = AIL_sample_status(m_sample2D[i]);
		if (status == 2 || status == 8)
			break;
	}
	if (i == 16 || !m_sample2D[i])
	{
		for (i = 0; i < 16; ++i)
			if (m_sample2D[i] && !m_bSample2DLoop[i])
				break;
		if (i == 16)
			return NULL;
	}
	if (m_sample2DOwner[i] > 0)
	{
		if (m_sound[m_sample2DOwner[i]].sample2d == m_sample2D[i])
			m_sound[m_sample2DOwner[i]].sample2d = NULL;
	}
	m_sample2DOwner[i] = handle;
	m_bSample2DLoop[i] = loop;
	m_sound[handle].sample2d = m_sample2D[i];
	AIL_init_sample(m_sample3D[i], 2);
	return m_sample2D[i];
}

HSAMPLE WMilesSoundSystem::GetAllocateSample3D(int handle, bool loop)
{
	unsigned int i;
	for (i = 0; i < 16; ++i)
	{
		if (!m_sample3D[i])
		{
			m_sample3D[i] = AIL_allocate_sample_handle(m_hDriver);
			break;
		}
		unsigned int status = AIL_sample_status(m_sample3D[i]);
		if (status == 2 || status == 8)
			break;
	}
	if (i == 16 || !m_sample3D[i])
	{
		for (i = 0; i < 16; ++i)
			if (m_sample3D[i] && !m_bSample3DLoop[i])
				break;
		if (i == 16)
			return NULL;
	}
	if (m_sample3DOwner[i] > 0)
	{
		if (m_sound[m_sample3DOwner[i]].sample3d == m_sample3D[i])
			m_sound[m_sample3DOwner[i]].sample3d = NULL;
	}
	m_sample3DOwner[i] = handle;
	m_bSample3DLoop[i] = loop;
	m_sound[handle].sample3d = m_sample3D[i];

	return m_sample3D[i];
}

unsigned int __stdcall WMilesSoundSystem::Open_callback(const char* filename,
	unsigned int* handle)
{
	for (unsigned int i = 1; i < 16; ++i)
	{
		if (!g_files[i])
		{
			cFile* file = g_resrcmng->GetCFile(filename, 0xffff);
			if (!file)
				return 0;
			g_files[i] = file;
			g_offset[i] = -1;
			*handle = i;
			return 1;
		}
	}
	return 0;
}

void __stdcall WMilesSoundSystem::Close_callback(unsigned int handle)
{
	CloseCFile(g_files[handle]);
	g_files[handle] = NULL;
}

int __stdcall WMilesSoundSystem::Seek_callback(unsigned int handle, int offset,
	unsigned int type)
{
	switch (type)
	{
	case 1:
		offset += g_files[handle]->Tell();
		break;
	case 2:
		offset += g_files[handle]->Length();
		break;
	}
	g_offset[handle] = offset;
	return offset;
}

unsigned int __stdcall WMilesSoundSystem::Read_callback(unsigned int handle,
	void* buffer, unsigned int bytes)
{
	if (g_offset[handle] != -1)
	{
		g_files[handle]->Seek(g_offset[handle], 0);
		g_offset[handle] = -1;
	}
	return g_files[handle]->Read(buffer, bytes);
}
