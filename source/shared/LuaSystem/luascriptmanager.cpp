#include "luascriptmanager.h"
#include "luascript.h"
#include "luatablehelper.h"
#include "wresrcmng.h"
#include "westpak.h"
#include <algorithm>
extern "C"
{
#include "Lua/src/lualib.h"
}

namespace lua_system
{
	void LuaOpenUtilityLib(lua_State* state);

	lua_script_manger* lua_script_manger::GetInstance()
	{
		static lua_script_manger manager;
		return &manager;
	}

	lua_script_manger::lua_script_manger()
	{
		master = luaL_newstate();
		if (master)
		{
			luaL_openlibs(master);
			LuaOpenUtilityLib(master);
			cFile* file = g_resrcmng->GetCFile("root.lua", 65535);
			if (file)
			{
				char* buffer = new char[file->Length() + 1];
				file->Read(buffer, file->Length());
				buffer[file->Length()] = 0;
				CloseCFile(file);
				lua_tinker::dostring(master, buffer, "root.lua");
				delete[] buffer;
			}
		}
	}

	lua_script_manger::~lua_script_manger()
	{
		for (std::list<lua_script*>::iterator i = scripts.begin();
			i != scripts.end(); ++i)
		{
			delete *i;
			*i = 0;
		}
		scripts.clear();
		lua_close(master);
	}

	lua_script* lua_script_manger::CreateScript(bool option)
	{
		lua_script* value = new lua_script(this, option);
		if (!value)
			return 0;
		scripts.push_back(value);
		return value;
	}

	void lua_script_manger::ReleaseScript(lua_script* value)
	{
		value->ReserveDelete();
	}

	void lua_script_manger::DeleteScript(lua_script* value)
	{
		std::list<lua_script*>::iterator i;
		i = std::find(scripts.begin(), scripts.end(), value);
		if (i != scripts.end())
		{
			delete *i;
			*i = 0;
			scripts.erase(i);
		}
	}

	void lua_script_manger::ReloadAll()
	{
		for (std::list<lua_script*>::iterator i = scripts.begin();
			i != scripts.end(); ++i)
			(*i)->Reload();
	}

	void lua_script_manger::Update(float delta)
	{
		for (std::list<lua_script*>::iterator i = scripts.begin();
			i != scripts.end(); ++i)
		{
			if ((*i)->Update(delta) == 4)
			{
				std::list<lua_script*>::iterator next = i;
				++next;
				DeleteScript(*i);
				i = next;
			}
		}
	}

	bool lua_script_manger::RunString(const char* text)
	{
		return lua_tinker::dostring(master, text, 0);
	}

	lua_table_helper lua_script_manger::GetTable(const char* name)
	{
		lua_tinker::table value =
			lua_tinker::get<lua_tinker::table>(master, name);
		return lua_table_helper(value);
	}
}
