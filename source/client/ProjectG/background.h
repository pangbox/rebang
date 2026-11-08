#pragma once

class Bitmap;
class WPoint;
class WRect;
class WSplash;

class CBackGround
{
public:
	CBackGround(const char* name, bool screenSized);
	CBackGround(bool screenSized, const Bitmap* bitmap);
	virtual ~CBackGround();
	virtual void Load(const char* name, bool screenSized);
	virtual void Process(const float deltaTime);
	virtual void Draw(unsigned long color);
	virtual void Draw(const WPoint* pos, const WRect* rect,
		unsigned long color);
	virtual void ResetScreenSize();
	virtual int GetWidth();
	virtual int GetHeight();
	void SetDUV(float du, float dv)
	{
		m_du = du;
		m_dv = dv;
	}
	bool IsLoaded() { return m_pSplash != NULL; }

protected:
	WSplash* m_pSplash;
	float m_du;
	float m_dv;
	float m_u;
	float m_v;
};
