#pragma once
#include "../Scripts/GameObject.h"

class CShader;
class CTexture;

class CGeoStar : public CGameObject
{
public:
	CGeoStar();
	void Draw() override;
};