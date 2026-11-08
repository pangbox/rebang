template <class T>
inline _Coordinates<T>::_Coordinates()
{
}

template <class T>
inline _Coordinates<T>::_Coordinates(T x, T y)
{
	this->x = x;
	this->y = y;
}

template <class T>
inline _Coordinates<T>::_Coordinates(const _Coordinates<T>& c)
{
	x = c.x;
	y = c.y;
}

template <class T>
inline _Coordinates<T> _Coordinates<T>::operator+(
	const _Coordinates<T>& c) const
{
	return _Coordinates<T>(x + c.x, y + c.y);
}

template <class T>
inline _Coordinates<T> _Coordinates<T>::operator+(const T v) const
{
	return _Coordinates<T>(x + v, y + v);
}

template <class T>
inline _Coordinates<T> _Coordinates<T>::operator-(
	const _Coordinates<T>& c) const
{
	return _Coordinates<T>(x - c.x, y - c.y);
}

template <class T>
inline _Coordinates<T> _Coordinates<T>::operator-(const T v) const
{
	return _Coordinates<T>(x - v, y - v);
}

template <class T>
inline T _Coordinates<T>::SquareMagnitude() const
{
	return x * x + y * y;
}

template <class T>
inline _Rectangle<T>::_Rectangle()
{
}

template <class T>
inline _Rectangle<T>::_Rectangle(const _Rectangle<T>& rhs)
{
	left = rhs.left;
	top = rhs.top;
	right = rhs.right;
	bottom = rhs.bottom;
}

template <class T>
inline _Rectangle<T>::_Rectangle(const _Coordinates<T>& tl,
	const _Coordinates<T>& br)
{
	left = tl.x;
	top = tl.y;
	right = br.x;
	bottom = br.y;
}

template <class T>
inline _Rectangle<T>::_Rectangle(const T l, const T t, const T r, const T b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

template <class T>
inline void _Rectangle<T>::Nomalize()
{
	if (left > right)
	{
		T v = left;
		left = right;
		right = v;
	}
	if (top > bottom)
	{
		T v = top;
		top = bottom;
		bottom = v;
	}
}

template <class T>
inline T _Rectangle<T>::Height() const
{
	return bottom - top;
}

template <class T>
inline T _Rectangle<T>::Width() const
{
	return right - left;
}

template <class T>
inline _Coordinates<T> _Rectangle<T>::GetSize() const
{
	return _Coordinates<T>(Width(), Height());
}

template <class T>
inline _Rectangle<T>& _Rectangle<T>::operator=(const _Rectangle<T>& rhs)
{
	left = rhs.left;
	top = rhs.top;
	right = rhs.right;
	bottom = rhs.bottom;
	return *this;
}
