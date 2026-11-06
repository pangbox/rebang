#pragma once

class DescBlock
{
public:
	DescBlock() { memset(m_desc, 0, sizeof(m_desc)); }
	virtual ~DescBlock() { }

	char* GetDesc() { return m_desc; }

protected:
	virtual void BuildDesc(unsigned long param1, unsigned long param2,
		const char* name)
	{
	}

	char m_desc[128];
};

class CApproachDesc
{
public:
	~CApproachDesc()
	{
		if (m_desc)
			delete m_desc;
		m_desc = NULL;
	}

	char* GetDesc() { return m_desc ? m_desc->GetDesc() : NULL; }
	CApproachDesc(int mission, unsigned long param1, unsigned long param2,
		const char* name);

private:
	DescBlock* m_desc;
};
