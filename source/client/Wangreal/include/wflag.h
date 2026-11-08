#pragma once

class WFlags
{
public:
	WFlags()
		: m_flag(0)
	{
	}
	WFlags(const unsigned long& flag)
		: m_flag(flag)
	{
	}
	void Set(unsigned long value) { this->m_flag = value; }
	void Enable(unsigned long flag) { this->m_flag |= flag; }
	void Disable(unsigned long flag) { this->m_flag &= ~flag; }
	void Turn(unsigned long flag, bool on)
	{
		if (on == true)
			this->m_flag |= flag;
		else
			this->m_flag &= ~flag;
	}
	void Reset() { this->m_flag = 0; }
	bool GetFlag(unsigned long flag) const
	{
		return (this->m_flag & flag) ? true : false;
	}
	unsigned long Get() const { return m_flag; }
	operator unsigned long() const { return m_flag; }

protected:
	unsigned long m_flag;
};
