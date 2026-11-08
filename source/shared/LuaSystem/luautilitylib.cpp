#include "luautilitylib.h"
#include "luascript.h"
#include "luascriptmanager.h"
#include "wresrcmng.h"
#include "westpak.h"
#include <windows.h>
#include <time.h>
#include <stdlib.h>

namespace lua_system
{
	static bool debug_line_trace = false;

	int lua_dofile_for_projectG(lua_State* state);
	static int WaitFrame(lua_State* state);
	static int WaitSec(lua_State* state);
	static lua_script* get_script(lua_State* state);
	static bool is_debug_mode();
	static const char* get_my_country();
	static const char* get_current_dir(const char* suffix);
	static void print_client(const char* text);
	static int debug_line_trace_start(lua_State* state);
	static int debug_line_trace_end(lua_State* state);
	static int debug_local_value(lua_State* state);
	static int print_value_info(lua_State* state);
	static int random(int low, int high);

	static const luaL_Reg script_lib[] = {
		{ "WaitFrame", WaitFrame },
        { "WaitSec",   WaitSec   },
        { 0,           0         }
	};

	void LuaOpenUtilityLib(lua_State* state)
	{
		luaL_openlib(state, "Script", script_lib, 0);
		lua_register(state, "dofile_ex", lua_dofile_for_projectG);
		lua_register(state, "debug_line_trace_start", debug_line_trace_start);
		lua_register(state, "debug_line_trace_end", debug_line_trace_end);
		lua_register(state, "debug_local_value", debug_local_value);
		lua_register(state, "print_value_info", print_value_info);
		lua_tinker::def(state, "is_debug_mode", is_debug_mode);
		lua_tinker::def(state, "get_my_country", get_my_country);
		lua_tinker::def(state, "get_current_dir", get_current_dir);
		lua_tinker::def(state, "print_client", print_client);
		lua_tinker::def(state, "random", random);
	}

	void print_debug_local_value(lua_State* state, const lua_Debug* debug)
	{
		char text[256];
		if (debug->currentline > 0 && strcmp(debug->source, "?"))
		{
			int index = 1;
			sprintf(text, "\t\t\t---- Debug Line : %s [%d] -----",
				debug->source, debug->currentline);
			print_client(text);
			const char* name = lua_getlocal(state, debug, index);
			while (name && strcmp(name, "(*temporary)"))
			{
				const char* value = lua_tostring(state, -1);
				++index;
				sprintf(text, "\t\t\t\t%s = %s", name, value);
				print_client(text);
				name = lua_getlocal(state, debug, index);
			}
		}
	}

	const char* lua_file_line_text_manager::get_line_text(const char* file,
		int line)
	{
		if (!file)
			return 0;
		std::map<std::string, std::vector<std::string> >::iterator it;
		it = lines_.find(file);
		if (it == lines_.end())
			return 0;
		std::vector<std::string>& values = it->second;
		if (values.size() >= line)
			return values[line - 1].c_str();
		return 0;
	}

	void lua_file_line_text_manager::road_lua_file(const char* file)
	{
		if (!file)
			return;
		cFile* input = g_resrcmng->GetCFile(file, 65535);
		if (!input)
			return;
		char* buffer = new char[input->Length() + 1];
		input->Read(buffer, input->Length());
		buffer[input->Length()] = 0;
		CloseCFile(input);
		std::vector<std::string> values;
		std::map<std::string, std::vector<std::string> >::iterator it;
		it = lines_.find(file);
		if (it != lines_.end())
			lines_.erase(it);
		const char* cursor = buffer;
		std::string line;
		while (*cursor)
		{
			if (*cursor == '\n')
			{
				values.push_back(line);
				line.clear();
				++cursor;
				if (!*cursor)
					break;
			}
			line += *cursor++;
		}
		delete[] buffer;
		if (line.size())
			values.push_back(line);
		lines_[file] = values;
	}

	void lua_file_line_text_manager::road_lua_buffer(const char* file,
		const char* buffer)
	{
		if (!file || !buffer)
			return;
		std::vector<std::string> values;
		std::map<std::string, std::vector<std::string> >::iterator it;
		it = lines_.find(file);
		if (it != lines_.end())
			lines_.erase(it);
		const char* cursor = buffer;
		std::string line;
		while (*cursor)
		{
			if (*cursor == '\n')
			{
				values.push_back(line);
				line.clear();
				++cursor;
				if (!*cursor)
					break;
			}
			line += *cursor++;
		}
		if (line.size())
			values.push_back(line);
		lines_[file] = values;
	}

	bool lua_file_line_text_manager::is_loaded_lua_file(const char* file)
	{
		if (!file)
			return false;
		std::map<std::string, std::vector<std::string> >::iterator it;
		it = lines_.find(file);
		if (it == lines_.end())
			return false;
		return true;
	}

	lua_file_line_text_manager* lua_file_line_text_manager::get_instance()
	{
		static lua_file_line_text_manager instance;
		return &instance;
	}

	int lua_dofile_for_projectG(lua_State* state)
	{
		const char* file = lua_tostring(state, -1);
		lua_script* script =
			lua_script_manger::GetInstance()->CreateScript(true);
		script->RunFile(file);
		return 0;
	}

	static int WaitFrame(lua_State* state)
	{
		lua_script* script = get_script(state);
		script->WaitFrameScript((int)luaL_checknumber(state, 1));
		return lua_yield(state, 1);
	}

	static int WaitSec(lua_State* state)
	{
		lua_script* script = get_script(state);
		float seconds = (float)luaL_checknumber(state, 1);
		script->WaitTimeScript(seconds);
		return lua_yield(state, 1);
	}

	static lua_script* get_script(lua_State* state)
	{
		lua_pushlightuserdata(state, state);
		lua_gettable(state, LUA_GLOBALSINDEX);
		return (lua_script*)lua_touserdata(state, -1);
	}

	static bool is_debug_mode()
	{
		return false;
	}

	static const char* get_my_country()
	{
		return "KOREA";
	}

	static const char* get_current_dir(const char* suffix)
	{
		static char result[1024];
		memset(result, 0, sizeof(result));
		char current[1024] = { 0 };
		GetCurrentDirectory(sizeof(current), current);
		std::string path(current);
		if (suffix && strcmp(suffix, "?"))
		{
			path += "\\";
			path += suffix;
		}
		strncpy(result, path.c_str(), sizeof(result) - 1);
		return result;
	}

	static void print_client(const char* text)
	{
	}

	static int debug_line_trace_start(lua_State* state)
	{
		return 0;
	}

	static int debug_line_trace_end(lua_State* state)
	{
		debug_line_trace = false;
		return 0;
	}

	static int debug_local_value(lua_State* state)
	{
		return 0;
	}

	static int print_value_info(lua_State* state)
	{
		return 0;
	}

	static int random(int low, int high)
	{
		if (low == high)
			return high;
		srand((unsigned)time(0));
		if (low > high)
		{
			int temp = high;
			high = low;
			low = temp;
		}
		return rand() % (high - low) + low;
	}
}
