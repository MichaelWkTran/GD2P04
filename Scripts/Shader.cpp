/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Shader.cpp																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <glm/gtc/type_ptr.hpp>

const char* CShader::m_directive = "Resources/Shaders/";

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CShader()
//	 Purpose: Initalises a shader. _pName is the name of the shader which can be used to find it again from the map it is stored in. 
//			  _vertexFile is the file directory to the vertex shader file.
//			  _strTessControlFile is the file directory to the tessellation control shader file.		  
//			  _strTessEvaluationFile is the file directory to the tessellation evaluation shader file. 
//			  _strGeometryFile is the file directory to the geometry shader file.
//			  _fragmentFile is the file directory to the fragment shader file.
//			  _defaultUniform is a function pointer used to store the default values of the shader so that it can be used later. 

CShader::CShader(std::string _vertexFile, std::string _strTessControlFile, std::string _strTessEvaluationFile,  std::string _strGeometryFile, std::string _fragmentFile, void(*_defaultUniform)(CShader& _shader))
{
	m_defaultUniform = _defaultUniform;

	//Create Program
	m_ID = glCreateProgram();

	//Set up Shaders
	auto lamCreateShaders = [&](std::string _strShaderDirectory, int _iShaderType, std::string _strShaderType)->const int
	{
		if (_strShaderDirectory == "") return -1;

		//Read shader
		std::string strShaderCode = GetFileContents(m_directive + _strShaderDirectory); const char* pVertexSource = strShaderCode.c_str();
		
		//Create and Compile Shader
		unsigned int uiShaderID = glCreateShader(_iShaderType);
		glShaderSource(uiShaderID, 1, &pVertexSource, NULL);
		glCompileShader(uiShaderID);
		
		//Print any errors in creating and compiling a shader
		CompileErrors(uiShaderID, _strShaderType);

		//Attach shader to program
		glAttachShader(m_ID, uiShaderID);

		return (int)uiShaderID;
	};

	int iVertexShader = lamCreateShaders(_vertexFile, GL_VERTEX_SHADER, "VERTEX");
	int iTessControlShader = lamCreateShaders(_strTessControlFile, GL_TESS_CONTROL_SHADER, "TESSELLATION_CONTROL_SHADER");
	int iTessEvaluationShader = lamCreateShaders(_strTessEvaluationFile, GL_TESS_EVALUATION_SHADER, "TESSELLATION_EVALUATION_SHADER");
	int iGeometryShader = lamCreateShaders(_strGeometryFile, GL_GEOMETRY_SHADER, "GEOMETRY");
	int iFragmentShader = lamCreateShaders(_fragmentFile, GL_FRAGMENT_SHADER, "FRAGMENT");

	//Link shaders to program
	glLinkProgram(m_ID);
	CompileErrors(m_ID, "PROGRAM");

	//Delete Shaders
	if (iVertexShader		  >= 0)	glDeleteShader(iVertexShader);
	if (iTessControlShader	  >= 0)	glDeleteShader(iTessControlShader);
	if (iTessEvaluationShader >= 0) glDeleteShader(iTessEvaluationShader);
	if (iGeometryShader		  >= 0)	glDeleteShader(iGeometryShader);
	if (iFragmentShader		  >= 0)	glDeleteShader(iFragmentShader);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: ~CShader()
//	 Purpose: Deletes the shader.

CShader::~CShader()
{
	glDeleteProgram(m_ID);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetFileContents()
//	 Purpose: Reads the contents of a shader file.
//	 Returns: The contents of a shader file.

std::string CShader::GetFileContents(std::string _pFileName)
{
	std::ifstream In(_pFileName, std::ios::binary);
	if (In)
	{
		std::string strContents;
		In.seekg(0, std::ios::end);
		strContents.resize((unsigned int)In.tellg());
		In.seekg(0, std::ios::beg);
		In.read(&strContents[0], strContents.size());
		In.close();
		return strContents;
	}
	throw errno;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CompileErrors()
//	 Purpose: To print an error when the shader file can not be read

void CShader::CompileErrors(unsigned int _uShader, std::string _pType)
{
	GLint GLiHasCompiled;
	char cInfoLog[1024];

	if (_pType != "PROGRAM")
	{
		glGetShaderiv(_uShader, GL_COMPILE_STATUS, &GLiHasCompiled);
		if (GLiHasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(_uShader, 1024, NULL, cInfoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << _pType << "\n" << cInfoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(_uShader, GL_LINK_STATUS, &GLiHasCompiled);
		if (GLiHasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(_uShader, 1024, NULL, cInfoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << _pType << "\n" << cInfoLog << std::endl;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: operator int()
//	 Purpose: Gets the ID of the shader.
//	 Returns: The ID of the shader.

CShader::operator int() const
{
	return m_ID;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: operator int*()
//	 Purpose: Prevents the shader from being cast to an integer pointer
//	 Returns: nullptr

CShader::operator int* ()
{
	return nullptr;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetID()
//	 Purpose: Gets the shader ID by casting the shader as an integer.
//	 Returns: The Shader ID.

const unsigned int& CShader::GetID()
{
	return m_ID;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Activate()
//	 Purpose: Use the Shader program.

void CShader::Activate()
{
	glUseProgram(m_ID);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Activate()
//	 Purpose: Use a shader program by passing in the shader ID via _uiID.

void CShader::Activate(unsigned int _uiID)
{
	glUseProgram(_uiID);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Deactivate()
//	 Purpose: Deactivates the shader

void CShader::Deactivate()
{
	glUseProgram(0);
}

#pragma region Uniform Functions

//------------------------------------------------------------------------------------------------------------------------
// Procedure: ResetUniforms()
//	 Purpose: Resets the shader uniforms to default uniforms using m_defaultUniform

void CShader::ResetUniforms()
{
	if (m_defaultUniform != nullptr)
	{
		m_defaultUniform(*this);
	}
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Uniform1f() Uniform2f() Uniform3f() Uniform4f() Uniform1i() Uniform2i() Uniform3i() Uniform4i() Uniform1ui() Uniform2ui() Uniform3ui() Uniform4ui() UniformMatrix4fv()
//	 Purpose: Assigns the shader uniform. _pUniform is the name of the uniform and value the rest of the variables are the given value

void CShader::Uniform1f(std::string _pUniform, float _v0)
{
	Activate();
	glUniform1f(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0);
	Deactivate();
}

void CShader::Uniform2f(std::string _pUniform, float _v0, float _v1)
{
	Activate();
	glUniform2f(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0, _v1);
	Deactivate();
}

void CShader::Uniform3f(std::string _pUniform, float _v0, float _v1, float _v2)
{
	Activate();
	glUniform3f(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0, _v1, _v2);
	Deactivate();
}

void CShader::Uniform3f(std::string _pUniform, glm::vec3 _v0)
{
	Activate();
	glUniform3f(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0.x, _v0.y, _v0.z);
	Deactivate();
}

void CShader::Uniform4f(std::string _pUniform, float _v0, float _v1, float _v2, float _v3)
{
	Activate();
	glUniform4f(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0, _v1, _v2, _v3);
	Deactivate();
}

void CShader::Uniform4f(std::string _pUniform, glm::vec4 _v0)
{
	Activate();
	glUniform4f(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0.x, _v0.y, _v0.z, _v0.w);
	Deactivate();
}

void CShader::Uniform1i(std::string _pUniform, int _v0)
{
	Activate();
	glUniform1i(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0);
	Deactivate();
}

void CShader::Uniform2i(std::string _pUniform, int _v0, int _v1)
{
	Activate();
	glUniform2i(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0, _v1);
	Deactivate();
}

void CShader::Uniform3i(std::string _pUniform, int _v0, int _v1, int _v2)
{
	Activate();
	glUniform3i(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0, _v1, _v2);
}

void CShader::Uniform4i(std::string _pUniform, int _v0, int _v1, int _v2, int _v3)
{
	Activate();
	glUniform4i(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0, _v1, _v2, _v3);
	Deactivate();
}

void CShader::Uniform1ui(std::string _pUniform, unsigned int _v0)
{
	Activate();
	glUniform1ui(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0);
	Deactivate();
}

void CShader::Uniform2ui(std::string _pUniform, unsigned int _v0, unsigned int _v1)
{
	Activate();
	glUniform2ui(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0, _v1);
	Deactivate();
}

void CShader::Uniform3ui(std::string _pUniform, unsigned int _v0, unsigned int _v1, unsigned int _v2)
{
	Activate();
	glUniform3ui(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0, _v1, _v2);
	Deactivate();
}

void CShader::Uniform4ui(std::string _pUniform, unsigned int _v0, unsigned int _v1, unsigned int _v2, unsigned int _v3)
{
	Activate();
	glUniform4ui(glGetUniformLocation(m_ID, _pUniform.c_str()), _v0, _v1, _v2, _v3);
	Deactivate();
}

void CShader::UniformMatrix4fv(std::string _pUniform, int _iCount, bool _bTranspose, glm::mat4 _v)
{
	Activate();
	glUniformMatrix4fv(glGetUniformLocation(m_ID, _pUniform.c_str()), _iCount, _bTranspose, glm::value_ptr(_v));
	Deactivate();
}

#pragma endregion