#pragma once

struct tagRECT;

template <class T>
struct _Coordinates
{
	T x;
	T y;

	_Coordinates(T x, T y);
	_Coordinates(const _Coordinates<T>& c);
	_Coordinates();

	void Reset() { x = y = 0; }
	T SquareMagnitude() const;
	T Magnitude() const;

	void operator+=(const _Coordinates<T>& c);
	void operator+=(const T v);
	void operator-=(const _Coordinates<T>& c);
	void operator-=(const T v);
	void operator*=(const _Coordinates<T>& c);
	void operator*=(const T v);
	void operator/=(const _Coordinates<T>& c);
	void operator/=(const T v);
	void operator%=(const _Coordinates<T>& c);
	void operator%=(const T v);

	bool operator==(const _Coordinates<T>& c) const;
	bool operator!=(const _Coordinates<T>& c) const;

	_Coordinates<T> operator+(const _Coordinates<T>& c) const;
	_Coordinates<T> operator+(const T v) const;
	_Coordinates<T> operator-(const _Coordinates<T>& c) const;
	_Coordinates<T> operator-(const T v) const;
	_Coordinates<T> operator*(const _Coordinates<T>& c) const;
	_Coordinates<T> operator*(const T v) const;
	_Coordinates<T> operator/(const _Coordinates<T>& c) const;
	_Coordinates<T> operator/(const T v) const;
};

struct _CoordinatesSHORT : public _Coordinates<short>
{
	_CoordinatesSHORT() { }
	_CoordinatesSHORT(const short x, const short y);
	_CoordinatesSHORT(const _Coordinates<short>& c);
	_CoordinatesSHORT(const _Coordinates<float>& c);
	_CoordinatesSHORT(const _CoordinatesSHORT& c);

	_CoordinatesSHORT& operator=(const _Coordinates<short>& c);
	_CoordinatesSHORT& operator=(const _Coordinates<float>& c);
};

struct _CoordinatesFLOAT : public _Coordinates<float>
{
	_CoordinatesFLOAT() { }
	_CoordinatesFLOAT(const float x, const float y);
	_CoordinatesFLOAT(const _Coordinates<float>& c);
	_CoordinatesFLOAT(const _Coordinates<short>& c);
	_CoordinatesFLOAT(const _CoordinatesFLOAT& c);

	_CoordinatesFLOAT& operator=(const _Coordinates<float>& c);
	_CoordinatesFLOAT& operator=(const _Coordinates<short>& c);
};

template <class T>
struct _Rectangle
{
	union
	{
		struct
		{
			_Coordinates<T> tl;
			_Coordinates<T> br;
		};
		struct
		{
			T left;
			T top;
			T right;
			T bottom;
		};
	};

	void Reset() { left = top = right = bottom = 0; }
	void Nomalize();
	bool IsIncluded(const _Coordinates<T>& c) const;
	bool IsIntersect(const _Rectangle<T>& rc) const;
	T Height() const;
	T Width() const;
	_Coordinates<T> GetSize() const;
	_Coordinates<T> GetCenterCoord() const;

	void InflateRect(T l, T t, T r, T b);
	void InflateRect(const _Rectangle<T>& rc);
	void InflateRect(const _Coordinates<T>& c);
	void InflateRect(T x, T y);
	void DeflateRect(T l, T t, T r, T b);
	void DeflateRect(const _Rectangle<T>& rc);
	void DeflateRect(const _Coordinates<T>& c);
	void DeflateRect(T x, T y);

	_Rectangle(const _Coordinates<T>& tl, const _Coordinates<T>& br);
	_Rectangle(const T l, const T t, const T r, const T b);
	_Rectangle(const _Rectangle<T>& rhs);
	_Rectangle();

	_Rectangle<T>& operator=(const _Rectangle<T>& rhs);
	_Rectangle<T>& operator=(const tagRECT* rc);
	_Rectangle<T>& operator=(const tagRECT& rc);

	void operator+=(const _Coordinates<T>& c);
	void operator+=(const T v);
	void operator-=(const _Coordinates<T>& c);
	void operator-=(const T v);

	bool operator==(const _Rectangle<T>& rc);
	bool operator!=(const _Rectangle<T>& rc);

	void operator&=(const _Rectangle<T>& rc);
	void operator|=(const _Rectangle<T>& rc);
	_Rectangle<T> operator&(const _Rectangle<T>& rc) const;
	_Rectangle<T> operator|(const _Rectangle<T>& rc) const;

	_Rectangle<T> operator+(const _Coordinates<T>& c) const;
	_Rectangle<T> operator+(const T v) const;
	_Rectangle<T> operator-(const _Coordinates<T>& c) const;
	_Rectangle<T> operator-(const T v) const;
};

struct _RectangleSHORT : public _Rectangle<short>
{
	_RectangleSHORT() { }
	_RectangleSHORT(const _Rectangle<short>& rc);
	_RectangleSHORT(const short l, const short t, const short r, const short b);
	_RectangleSHORT(const _Coordinates<short>& tl,
		const _Coordinates<short>& br);

	_RectangleSHORT& operator=(const _Rectangle<short>& rc);
	_RectangleSHORT& operator=(const _Rectangle<float>& rc);
	_RectangleSHORT& operator=(const tagRECT& rc);
	_RectangleSHORT& operator=(const tagRECT* rc);
};

struct _RectangleFLOAT : public _Rectangle<float>
{
	_RectangleFLOAT() { }
	_RectangleFLOAT(const _Rectangle<float>& rc);
	_RectangleFLOAT(const float l, const float t, const float r, const float b);
	_RectangleFLOAT(const _Coordinates<float>& tl,
		const _Coordinates<float>& br);
	_RectangleFLOAT(const tagRECT& rc);
	_RectangleFLOAT(const _RectangleFLOAT& rc);

	_RectangleFLOAT& operator=(const _Rectangle<short>& rc);
	_RectangleFLOAT& operator=(const _Rectangle<float>& rc);
	_RectangleFLOAT& operator=(const tagRECT& rc);
	_RectangleFLOAT& operator=(const tagRECT* rc);
};

#include "coord.inl"
