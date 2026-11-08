#include "stdafx.h"
#include "tweaker.h"
#include <stdarg.h>

namespace
{
	bool tokenize(const std::string& source, const char* separators,
		std::vector<std::string>& tokens)
	{
		std::string token;
		unsigned int count = strlen(separators);
		for (std::string::const_iterator it = source.begin();
			it != source.end(); ++it)
		{
			unsigned int i;
			for (i = 0; i < count; ++i)
			{
				if (separators[i] == *it && token.length() > 0)
				{
					tokens.push_back(token);
					token.clear();
					if (++it == source.end())
						return true;
					break;
				}
			}
			bool separator = false;
			for (i = 0; i < count; ++i)
				if (separators[i] == *it)
				{
					separator = true;
					break;
				}
			if (!separator)
				token += *it;
		}
		if (token.length() > 0)
			tokens.push_back(token);
		return tokens.size() == 0;
	}

	void __cdecl output_trace(const char* format, ...)
	{
		char buffer[512] = { 0 };
		va_list args;
		va_start(args, format);
		int result = _vsnprintf(buffer, 511, format, args);
		va_end(args);
		if (result != -1)
		{
			int length = strlen(buffer);
			buffer[length] = '\n';
			buffer[length + 1] = 0;
		}
	}
}

tweaker_cmd_target::~tweaker_cmd_target()
{
	tweaker_database::instance()->_unregist_tweaker(this);
}

tweaker_param::tweaker_param()
{
}

tweaker_param::tweaker_param(std::vector<std::string>& params)
{
	if (params.size())
		m_params.assign(params.begin(), params.end());
}

tweaker_token tweaker_param::operator[](unsigned int index) const
{
	if (m_params.size() > index)
		return tweaker_token(m_params[index]);
	output_trace("tweaker_token index overrun!");
	return tweaker_token(std::string(""));
}

tweaker::tweaker(const char* name)
	: m_name(name)
{
}

void tweaker::_regist_command(const char* name, const char* command,
	tweaker_cmd_info& info)
{
	if (m_name != name)
		return;
	if (m_command.find(command) == m_command.end())
	{
		output_trace("succeed regist tweaker command : &%s::%s", name, command);
		m_command.insert(
			std::map<std::string, tweaker_cmd_info>::value_type(command, info));
	}
}

std::string tweaker::_run_command(std::string& command, tweaker_param& param)
{
	std::map<std::string, tweaker_cmd_info>::iterator found =
		m_command.find(command);
	if (found == m_command.end())
		return "unknown command";
	std::string result;
	tweaker_cmd_func function = found->second.function;
	for (std::set<tweaker_cmd_target*>::iterator it = m_alloc.begin();
		it != m_alloc.end(); ++it)
		result = ((*it)->*function)(param);
	return result;
}

std::string tweaker::run_command(const char* param)
{
	if (!param)
		return std::string("warning : parameter null");
	std::vector<std::string> params;
	tokenize(std::string(param), " ,", params);
	if (!params.size())
		return std::string("warning : parameter empty");
	std::string command = params[0];
	params.erase(params.begin());
	return _run_command(command, tweaker_param(params));
}

bool tweaker::is_exist_command(const std::string& command)
{
	std::vector<std::string> params;
	tokenize(command, " ,", params);
	if (!params.size())
		return false;
	std::map<std::string, tweaker_cmd_info>::iterator it =
		m_command.find(params[0]);
	return it != m_command.end();
}

void tweaker::get_command_list(std::vector<std::string>& list,
	bool withName) const
{
	for (std::map<std::string, tweaker_cmd_info>::const_iterator it =
			 m_command.begin();
		it != m_command.end(); ++it)
	{
		std::string line;
		if (withName)
		{
			line = m_name;
			line += " ";
		}
		const tweaker_cmd_info& info = it->second;
		line += it->first;
		if (!info.help.empty())
		{
			line += " (";
			line += info.help;
			line += ")";
		}
		list.push_back(line);
	}
}

void tweaker::_insert_alloc(tweaker_cmd_target* target)
{
	if (m_alloc.find(target) == m_alloc.end())
	{
		m_alloc.insert(target);
		output_trace("succeed tweaker insert alloc : 0x%p", target);
	}
}

void tweaker::_erase_alloc(tweaker_cmd_target* target)
{
	std::set<tweaker_cmd_target*>::iterator it = m_alloc.find(target);
	if (it != m_alloc.end())
		m_alloc.erase(it);
}

bool tweaker::_is_exist_alloc(tweaker_cmd_target* target)
{
	std::set<tweaker_cmd_target*>::iterator it = m_alloc.find(target);
	return it != m_alloc.end();
}

tweaker_database::~tweaker_database()
{
	for (std::map<std::string, tweaker*>::iterator it = m_tweaker.begin();
		it != m_tweaker.end(); ++it)
		delete it->second;
	m_tweaker.clear();
}

std::string tweaker_database::send_command(const char* command)
{
	std::string str(command);
	std::string name;
	if (str.empty())
		return std::string("> command string too short.");
	{
		std::vector<std::string> params;
		tokenize(str, " ,", params);
		name = params[0];
		params.erase(params.begin());
		if (!params.size())
			return std::string("> command string too short.");
		{
			if (name == "class" || name == "struct")
			{
				name += " ";
				name += params[0];
				params.erase(params.begin());
			}
			tweaker* t = get_tweaker(name);
			if (t && !params.empty())
			{
				str = params[0];
				params.erase(params.begin());
				std::string result("> ");
				result += t->_run_command(str, tweaker_param(params));
				return result;
			}
			return std::string("> tweaker not found.");
		}
	}
}

bool tweaker_database::is_exist_command(const char* command)
{
	std::string str(command);
	std::string name;
	if (str.empty())
		return false;
	std::vector<std::string> params;
	tokenize(str, " ,", params);
	name = params[0];
	params.erase(params.begin());
	if (params.size())
	{
		if (name == "class" || name == "struct")
		{
			name += " ";
			name += params[0];
			params.erase(params.begin());
		}
		tweaker* t = get_tweaker(name);
		if (t && !params.empty())
		{
			str = params[0];
			return t->is_exist_command(str);
		}
	}
	return false;
}

bool tweaker_database::is_exist_tweaker(const tweaker* t)
{
	for (std::map<std::string, tweaker*>::iterator it = m_tweaker.begin();
		it != m_tweaker.end(); ++it)
		if (it->second == t)
			return true;
	return false;
}

void tweaker_database::get_tweaker_command_all(std::vector<std::string>& list)
{
	list.clear();
	unsigned int size = 0;
	for (std::map<std::string, tweaker*>::iterator it = m_tweaker.begin();
		it != m_tweaker.end(); ++it)
		size += it->second->_size_command();
	list.reserve(size);
	for (std::map<std::string, tweaker*>::iterator it = m_tweaker.begin();
		it != m_tweaker.end(); ++it)
		it->second->get_command_list(list, true);
}

void tweaker_database::get_tweaker_names(std::vector<std::string>& list)
{
	list.clear();
	list.reserve(m_tweaker.size());
	std::map<std::string, tweaker*>::const_iterator it;
	for (it = m_tweaker.begin(); it != m_tweaker.end(); ++it)
		list.push_back(it->first);
}

tweaker* tweaker_database::get_tweaker(std::string& name)
{
	std::map<std::string, tweaker*>::iterator it = m_tweaker.find(name);
	if (it != m_tweaker.end())
		return it->second;
	return NULL;
}

tweaker* tweaker_database::_regist_tweaker(const char* name,
	tweaker_cmd_target* target)
{
	std::map<std::string, tweaker*>::iterator it = m_tweaker.find(name);
	if (it != m_tweaker.end())
	{
		it->second->_insert_alloc(target);
		return it->second;
	}
	tweaker* t = new tweaker(name);
	t->_insert_alloc(target);
	m_tweaker.insert(std::map<std::string, tweaker*>::value_type(name, t));
	output_trace("succeed tweaker regist : 0x%p", target);
	return t;
}

void tweaker_database::_unregist_tweaker(tweaker_cmd_target* target)
{
	for (std::map<std::string, tweaker*>::iterator it = m_tweaker.begin();
		it != m_tweaker.end(); ++it)
	{
		tweaker* t = it->second;
		if (t->_is_exist_alloc(target))
		{
			t->_erase_alloc(target);
			if (t->_size_alloc() == 0)
			{
				delete t;
				m_tweaker.erase(it);
				output_trace("succeed tweaker unregist : 0x%p", target);
				return;
			}
		}
	}
}
