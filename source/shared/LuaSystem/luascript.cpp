#include "luascript.h"
#include "luascriptmanager.h"
#include "luatablehelper.h"
#include "wresrcmng.h"
#include "westpak.h"

namespace lua_system
{
	lua_script::lua_script(lua_script_manger* owner, bool option)
	{
		manager = owner;
		flag = option;
		state = 3;
		elapsed = 0;
		lua_State* master = manager->GetMasterLuaState();
		thread = lua_newthread(master);
		reference = luaL_ref(master, LUA_REGISTRYINDEX);
		lua_pushlightuserdata(master, thread);
		lua_pushlightuserdata(master, this);
		lua_settable(master, LUA_GLOBALSINDEX);
	}

	lua_script::~lua_script()
	{
		luaL_unref(manager->GetMasterLuaState(), LUA_REGISTRYINDEX, reference);
	}

	int lua_script::Update(float delta)
	{
		float next = delta + elapsed;
		elapsed = next;
		switch (state)
		{
		case 0:
			if (--wait_frames <= 0)
				ResumeScript(0);
			break;
		case 1:
			if (next >= wait_time)
				ResumeScript(0);
			break;
		case 2:
			if (flag && next >= 5.0f)
				state = 4;
			break;
		}
		return state;
	}

	void lua_script::WaitTimeScript(float seconds)
	{
		wait_time = seconds;
		elapsed = 0;
		state = 1;
	}

	void lua_script::WaitFrameScript(int frames)
	{
		wait_frames = frames;
		state = 0;
	}

	bool lua_script::RunFile(const char* name)
	{
		bool success = false;
		cFile* file = g_resrcmng->GetCFile(name, 65535);
		if (file)
		{
			char* buffer = new char[file->Length() + 1];
			file->Read(buffer, file->Length());
			buffer[file->Length()] = 0;
			CloseCFile(file);
			if (luaL_loadbuffer(thread, buffer, strlen(buffer), name) == 0)
			{
				ResumeScript(0);
				success = true;
				filename = name;
			}
			else
				FormatError();
			delete[] buffer;
		}
		return success;
	}

	bool lua_script::RunString(const char* text)
	{
		bool success = false;
		script = text;
		if (luaL_loadbuffer(thread, text, strlen(text),
				"lua_script::RunString") == 0)
		{
			ResumeScript(0);
			success = true;
		}
		else
			FormatError();
		return success;
	}

	void lua_script::Reload()
	{
		if (filename.size() > 0)
			RunFile(filename.c_str());
		if (script.size() > 0)
			RunString(script.c_str());
	}

	void lua_script::ResumeScript(float value)
	{
		state = 2;
		elapsed = 0;
		lua_pushnumber(thread, value);
		if (lua_resume(thread, 1))
			FormatError();
	}

	void lua_script::AbortWait()
	{
		state = 2;
		elapsed = 0;
		lua_pushnumber(thread, 1.0);
		if (lua_resume(thread, 1))
			FormatError();
	}

	void lua_script::FormatError()
	{
		const char* text = lua_tolstring(thread, -1, 0);
		lua_settop(thread, -2);
		if (!text)
		{
			const char* msg = "(error with no message)";
			strcpy(error, msg);
		}
		else
			strcpy(error, text);
	}

	void lua_script::OutputError(char* prefix)
	{
		printf("%s %s \n", prefix, error);
	}

	lua_table_helper lua_script::GetTable(const char* name)
	{
		lua_tinker::table value =
			lua_tinker::get<lua_tinker::table>(thread, name);
		return lua_table_helper(value);
	}
}
