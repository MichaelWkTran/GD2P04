#include "GPUParticle.h"
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include "Main.h"
#include <Camera.h>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include <MathUtils.h>

CGPUParticleSystem::CGPUParticleSystem()
{
	m_particleTexture = new CTexture("WaterDrop.png", GL_RGBA);
	m_visible = true;
	renderProgram = *(new CShader("Particle.vert", "Particle.geom", "Particle.frag"));

	//Create compute shader
	{
		//Create Program
		computeProgram = glCreateProgram();
	
		//Read shader
		std::string shaderCode = CShader::GetFileContents(CShader::m_directive + std::string("GPUParticle.comp")); const char* pVertexSource = shaderCode.c_str();
	
		//Create and Compile Shader
		unsigned int shaderID = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(shaderID, 1, &pVertexSource, NULL);
		glCompileShader(shaderID);
	
		//Print any errors in creating and compiling a shader
		CShader::CompileErrors(shaderID, "COMPUTE");
	
		//Attach shader to program
		glAttachShader(computeProgram, shaderID);
	
		//Link shaders to program
		glLinkProgram(computeProgram);
		CShader::CompileErrors(shaderID, "PROGRAM");
	
		//Delete Shaders
		glDeleteShader(shaderID);
	}
	
	//Set particle Data
	initialposition.resize(numParticles);
	initialvelocity.resize(numParticles);
	for (int i = 0; i < numParticles; i++)
	{
		float minSpreadSpeed = 2.0f; float maxSpreadSpeed = 3.0f;
		float minRiseSpeed = 2.0f; float maxRiseSpeed = 6.0f;
		
		float spreadSpeed = glm::Lerp(minSpreadSpeed, maxSpreadSpeed, (float)rand() / RAND_MAX);
		float spreadAngle = 2.0f * glm::pi<float>() * ((float)rand() / RAND_MAX);

		initialposition[i] = glm::vec4(0.0f, 0.0f, 0.0f, ((float)rand()/RAND_MAX));
		initialvelocity[i] = glm::vec3
		(
			cosf(spreadAngle) * spreadSpeed,
			glm::Lerp(minRiseSpeed, maxRiseSpeed, (float)rand()/RAND_MAX),
			sinf(spreadAngle) * spreadSpeed
		);
	}

	//Create buffers
	glGenBuffers(1, &posVbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posVbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, initialposition.size() * sizeof(glm::vec4), &initialposition[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posVbo);

	glGenBuffers(1, &velVbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velVbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, initialvelocity.size() * sizeof(glm::vec3), &initialvelocity[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velVbo);

	glGenBuffers(1, &initVelVbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, initVelVbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, initialvelocity.size() * sizeof(glm::vec3), &initialvelocity[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, initVelVbo);

	//Create vertex array
	glGenVertexArrays(1, &particleVao);
	glBindVertexArray(particleVao);
	
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, NULL, 0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CGPUParticleSystem::Update()
{
	if (!m_visible) return;

	//Update particles with compute shader
	glUseProgram(computeProgram);
	glUniform1f(glGetUniformLocation(computeProgram, "uni_deltaTime"), e_deltaTime * 100.0f);
	glDispatchCompute(numParticles / 128, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void CGPUParticleSystem::Draw()
{
	if (!m_visible) return;

	//Bind shader and set blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	
	glUseProgram(renderProgram);
	
	//Set render shader uniforms
	glUniformMatrix4fv(glGetUniformLocation(renderProgram, "uni_cameraMatrix"), 1, GL_FALSE, glm::value_ptr(GetMainCamera().GetCameraMatrix()));
	
	glUniform3f(glGetUniformLocation(renderProgram, "uni_cameraUp"),
		GetMainCamera().m_transform.Up().x, GetMainCamera().m_transform.Up().y, GetMainCamera().m_transform.Up().z);

	glUniform3f(glGetUniformLocation(renderProgram, "uni_cameraRight"),
		GetMainCamera().m_transform.Right().x, GetMainCamera().m_transform.Right().y, GetMainCamera().m_transform.Right().z);

	glActiveTexture(GL_TEXTURE0);
	m_particleTexture->Bind();
	glUniform1i(glGetUniformLocation(renderProgram, "uni_texture"), 0);

	//Bind position buffer as GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, NULL, 0);
	glEnableVertexAttribArray(0);
	
	//Render
	glBindVertexArray(particleVao);
	glDrawArrays(GL_POINTS, 0, numParticles);
	glBindVertexArray(0);
	
	//Tidy up
	m_particleTexture->Unbind();
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}
	