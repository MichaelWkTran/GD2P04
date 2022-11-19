	#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <UpdatedObject.h>
#include <Texture.h>

class CGPUParticleSystem : public CUpdatedObject
{
public:
	bool m_visible;
	static const unsigned int numParticles = 128 * 100;
	unsigned int posVbo, velVbo, initVelVbo, particleVao;
	unsigned int computeProgram, renderProgram;
	std::vector<glm::vec4> initialposition;
	std::vector<glm::vec3> initialvelocity;
	CTexture* m_particleTexture;

	CGPUParticleSystem();
	virtual void Update();
	virtual void Draw();
};

