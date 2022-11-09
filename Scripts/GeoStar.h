#pragma once
#include "GameObject.h"

class CShader;
class CTexture;

class CGeoStar : public CGameObject
{
public:
	CGeoStar();
	void Draw() override;
};