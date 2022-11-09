	#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "UpdatedObject.h"
#include "Texture.h"

class CGPUParticleSystem : public CUpdatedObject
{
public:
	static const unsigned int numParticles = 128 * 20000;
	unsigned int posVbo, velVbo, initVelVbo, particleVao;
	unsigned int computeProgram, renderProgram;
	std::vector<glm::vec4> initialposition;
	std::vector<glm::vec4> initialvelocity;
	static CTexture* m_particleTexture;

	CGPUParticleSystem();
	virtual void Draw();
};

