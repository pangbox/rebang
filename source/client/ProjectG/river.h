#pragma once

#include <vector>

class ntRiver
{
public:
	virtual ~ntRiver() { }

	virtual void Setup(const std::vector<WVector>& points,
		const char* name) = 0;
	virtual void Process(float dt) = 0;
	virtual void Render() = 0;
};

struct sRiverSide
{
	float scroll;
	float scrollVel;
	int texHandle;
	float tileHead;
	float tileSide;
	float depth;
	bool bActive;
};

class CRiver : public ntRiver
{
public:
	struct sRiver
	{
		sRiver() { }

		WVector pos[2];
		float u[2];
	};

	CRiver();
	virtual ~CRiver();

	virtual void Setup(const std::vector<WVector>& points, const char* name);
	virtual void Process(float dt);
	virtual void Render();

protected:
	void ReadAttribute(const char* name);

	float m_step;
	std::vector<sRiver> m_river;
	sRiverSide m_side[2];
	std::vector<Waabb> m_water;

private:
	static WTVertex vtr[4];
	static WTVertex* vtx[5];
};

class ntRiver_Lava : public ntRiver
{
public:
	struct sRiver
	{
		sRiver() { }

		WVector pos[2];
		float u[2];
	};

	ntRiver_Lava();
	virtual ~ntRiver_Lava();

	virtual void Setup(const std::vector<WVector>& points, const char* name);
	virtual void Process(float dt);
	virtual void Render();

protected:
	void ReadAttribute(const char* name);

	float m_unused4;
	sRiverSide m_side[2];
	std::vector<sRiver> m_river;
	std::vector<Waabb> m_water;

private:
	static WTVertex vtr[4];
	static WTVertex* vtx[5];
};

class CRiver_ex : public ntRiver
{
public:
	struct sRiver
	{
		sRiver() { }

		WVector pos[2];
		float u[2];
	};

	CRiver_ex();
	virtual ~CRiver_ex();

	virtual void Setup(const std::vector<WVector>& points, const char* name);
	virtual void Process(float dt);
	virtual void Render();

protected:
	void ReadAttribute(const char* name);

	std::vector<sRiver> m_river;
	sRiverSide m_side[2];
	std::vector<Waabb> m_water;

private:
	static WTVertex vtr[4];
	static WTVertex* vtx[5];
};
