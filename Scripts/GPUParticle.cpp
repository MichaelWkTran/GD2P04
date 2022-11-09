#include "GPUParticle.h"
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include "Main.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

CTexture* CGPUParticleSystem::m_particleTexture = nullptr;

CGPUParticleSystem::CGPUParticleSystem()
{
	if (m_particleTexture == nullptr) m_particleTexture = new CTexture("SmokeParticle.png", GL_RGBA);

	//Set up Shaders
	auto lamCreateShaders = [&](unsigned int& _program, std::string _shaderDirectory, int _shaderType, std::string _shaderTypeName)
	{
		//Create Program
		_program = glCreateProgram();

		//Read shader
		std::string shaderCode = CShader::GetFileContents(CShader::m_directive + _shaderDirectory); const char* pVertexSource = shaderCode.c_str();

		//Create and Compile Shader
		unsigned int shaderID = glCreateShader(_shaderType);
		glShaderSource(shaderID, 1, &pVertexSource, NULL);
		glCompileShader(shaderID);

		//Print any errors in creating and compiling a shader
		CShader::CompileErrors(shaderID, _shaderTypeName);

		//Attach shader to program
		glAttachShader(_program, shaderID);

		//Link shaders to program
		glLinkProgram(_program);
		CShader::CompileErrors(shaderID, "PROGRAM");

		//Delete Shaders
		glDeleteShader(shaderID);
	};

	lamCreateShaders(computeProgram, "GPUParticle.comp", GL_COMPUTE_SHADER, "COMPUTE");
	renderProgram = *(new CShader("Particle.vert", "Particle.geom", "Particle.frag"));

	//------------------------------------------------------------------------------------------------------------------------------
	initialposition.resize(numParticles);
	initialvelocity.resize(numParticles);
	for (int i = 0; i < numParticles; i++)
	{
		initialposition[i] = glm::vec4(0.0f, 0.0f, 0.0f, ((float)rand()/(double)RAND_MAX) + 0.125);

		initialvelocity[i] = glm::vec4
		(
			0.25f * cos(i * 0.0167f * 0.5) + 0.25f * ((float)rand()/(double)RAND_MAX) - 0.125f,
			2.0f + 0.25f * ((float)rand()/(double)RAND_MAX) - 0.125f, 0.25 * sin(i * 0.0167 * 0.5f) + 0.25f * ((float)rand()/(double)RAND_MAX) - 0.125f,
			((float)rand()/(double)RAND_MAX) + 0.125f
		);
	}

	//------------------------------------------------------------------------------------------------------------------------------
	glGenBuffers(1, &posVbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posVbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, initialposition.size() * sizeof(glm::vec4), &initialposition[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posVbo);

	glGenBuffers(1, &velVbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velVbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, initialvelocity.size() * sizeof(glm::vec4), &initialvelocity[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velVbo);

	glGenBuffers(1, &initVelVbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, initVelVbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, initialvelocity.size() * sizeof(glm::vec4), &initialvelocity[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, initVelVbo);

	// a useless vao, but we need it bound or we get errors.
	glGenVertexArrays(1, &particleVao);
	glBindVertexArray(particleVao);
	
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, NULL, 0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CGPUParticleSystem::Draw()
{
	glUseProgram(computeProgram);
	glDispatchCompute(numParticles / 128, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	glUseProgram(renderProgram);
	
	glUniformMatrix4fv(glGetUniformLocation(renderProgram, "uni_mat4CameraMatrix"), 1, GL_FALSE, glm::value_ptr(GetMainCamera().GetCameraMatrix()));
	
	glUniform3f(glGetUniformLocation(renderProgram, "uni_cameraUp"),
		GetMainCamera().m_transform.Up().x, GetMainCamera().m_transform.Up().y, GetMainCamera().m_transform.Up().z);

	glUniform3f(glGetUniformLocation(renderProgram, "uni_cameraRight"),
		GetMainCamera().m_transform.Right().x, GetMainCamera().m_transform.Right().y, GetMainCamera().m_transform.Right().z);

	glActiveTexture(GL_TEXTURE0);
	m_particleTexture->Bind();
	glUniform1i(glGetUniformLocation(renderProgram, "uni_samp2DDiffuse0"), 0);

	// Bind position buffer as GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, NULL, 0);
	glEnableVertexAttribArray(0);
	
	// Render
	glBindVertexArray(particleVao);
	glDrawArrays(GL_POINTS, 0, numParticles);
	glBindVertexArray(0);
	
	// Tidy up
	m_particleTexture->Unbind();
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
}
	