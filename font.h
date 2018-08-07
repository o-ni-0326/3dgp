#pragma once
#include "sprite.h"

class Font
{
private:

	static sprite* img;
	static float r, g, b, a;

public:

	void Text_Initialize() {};

	static void RenderText(ID3D11DeviceContext*Context,
		const char*text,float x,float y,float w,float h){}
};
