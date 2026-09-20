#pragma once

class WMemFillBlock
{
public:
	void* Alloc(int size);
	void Free(void* data);
};

extern WMemFillBlock g_mem;
