#ifndef SHADERSUTLITY_H
#define SHADERSUTLITY_H
#include<istream>
#include <glew.h>
#include<iostream>
#include<stdlib.h>
#include<fstream>
#include<stdio.h>
#include <string>
#include<vector>
#include <boost/filesystem.hpp>
#include<sstream>
#include <Logger\Logger.h>
#include <StringUtils.h>
#define CHECK_GL_ERROR CheckError(__FUNCTION__,StringUtils::get_string(__LINE__));

inline bool CheckError(const std::string &_func_name,
	const std::string &_line){
	switch (glGetError())
	{
	case GL_NO_ERROR:
		return true;
	case GL_INVALID_ENUM:
		AceLogger::Log("An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag. Function:" + _func_name + " line : " + _line, AceLogger::LOG_ERROR);
		return false;
	case GL_INVALID_VALUE:
		AceLogger::Log("ERROR: A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag. Function:" + _func_name + " line : " + _line, AceLogger::LOG_ERROR);
		return false;
	case GL_INVALID_OPERATION:
		AceLogger::Log("The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag. Function:" + _func_name + " line : " + _line, AceLogger::LOG_ERROR);
		return false;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		AceLogger::Log("The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag. Function:" + _func_name + " line : " + _line, AceLogger::LOG_ERROR);
		return false;
	case GL_OUT_OF_MEMORY:
		AceLogger::Log("There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. Function:" + _func_name + " line : " + _line, AceLogger::LOG_ERROR);
		return false;
	case GL_STACK_UNDERFLOW:
		AceLogger::Log("An attempt has been made to perform an operation that would cause an internal stack to underflow. Function:" + _func_name + " line : " + _line, AceLogger::LOG_ERROR);
		return false;
	case GL_STACK_OVERFLOW:
		AceLogger::Log("An attempt has been made to perform an operation that would cause an internal stack to overflow. Function:" + _func_name + " line : " + _line, AceLogger::LOG_ERROR);
		return false;
	default:
		break;
	}
	return true;
}

inline GLuint LoadShader(std::istream &_istr, const GLenum & _shader){
	// Create the shaders
	GLuint shaderID = glCreateShader(_shader);
	std::string shaderCode;
	std::string line = "";
	while (std::getline(_istr, line))
		shaderCode += "\n" + line;
	GLint result = GL_FALSE;
	int infoLogLength;

	// Compile Shader
	AceLogger::Log("Compiling shader...");
	char const * sourceShaderPointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourceShaderPointer, NULL);
	glCompileShader(shaderID);

	// Check Shader
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0){
		std::vector<char> shaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]);
		printf("%s\n", &shaderErrorMessage[0]);
		//shaderID = 0;
	}
	return shaderID;
};

inline GLuint GetProgram(const std::vector<GLuint> &_shaders){
	// Link the program
	AceLogger::Log("Linking program");
	GLuint programID = glCreateProgram();
	for (auto shaderID:_shaders)
		glAttachShader(programID, shaderID);
	glLinkProgram(programID);

	// Check the program
	GLint result = GL_FALSE;
	int infoLogLength;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0){
		std::vector<char> ProgramErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	for (auto shaderID : _shaders)
		glDeleteShader(shaderID);

	return programID;

};

inline GLuint LoadShaderFromFile(const std::string &_filePath,
							     GLenum _shader){

	boost::filesystem::path somepath(_filePath); 
	if (!boost::filesystem::exists(somepath))
	{
		AceLogger::Log("Can't find my file!", AceLogger::LOG_ERROR);
	}
	std::ifstream ifstr(_filePath);
	return LoadShader(ifstr, _shader);
};

inline GLuint LoadShader(const std::string &_shaderCode, 
						GLenum _shader){

	std::stringstream istr;
	istr << _shaderCode;
	return LoadShader(istr, _shader);
};

inline GLuint GetProgramID_StrShaders(std::vector<std::pair< std::string,GLenum>>&_shaders){
	std::vector<GLuint> shaders;
	for (auto shader : _shaders){
		shaders.push_back(LoadShader(shader.first, shader.second));
	}
	return GetProgram(shaders);
};

inline GLuint GetProgramID_FileShaders(std::vector<std::pair< std::string, GLenum>>&_shaders){
	std::vector<GLuint> shaders;
	for (auto shader : _shaders){
		shaders.push_back(LoadShaderFromFile(shader.first, shader.second));
	}
	return GetProgram(shaders);
}

struct vao_state{
	GLuint p_vertexbuffer;
	GLuint p_colours_vbo;
	GLuint p_vao;
	vao_state(){
		p_vertexbuffer = 0;
		p_colours_vbo = 0;
		p_vao = 0;
	}
	
}typedef vao_state;

struct vao_state_normals{
	GLuint p_vertexbuffer;
	GLuint p_colours_vbo;
	GLuint p_normals_vbo;
	GLuint p_vao;
	vao_state_normals(){
		p_vertexbuffer = 0;
		p_colours_vbo = 0;
		p_normals_vbo = 0;
		p_vao = 0;
	}
	
}typedef vao_state_normals;

inline void generate_gl_buffers(vao_state &_state,
	int _total_count_vertices,
	int _total_count_colours,
	float *_points,
	float *_colors,
	bool _alpha_included = false){

	glGenBuffers(1, &_state.p_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*_total_count_vertices, _points, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &_state.p_colours_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*_total_count_colours, _colors, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &_state.p_vao);
	glBindVertexArray(_state.p_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_vertexbuffer);
	glVertexAttribPointer(0,  // attribute 0. (id)
		3,   // size
		GL_FLOAT,  // type
		GL_FALSE,  // normalized?
		0,        // stride
		NULL);     // array buffer offset
	int number_of_color_data_elements = _alpha_included ? 4 : 3;
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_colours_vbo);
	glVertexAttribPointer(1, number_of_color_data_elements, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

}

template<typename uniform>
inline void generate_gl_uniform_buffer(GLuint &_uniform){
	glGenBuffers(1, &_uniform);
	glBindBuffer(GL_UNIFORM_BUFFER, _uniform);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniform), NULL, GL_DYNAMIC_DRAW);
}

inline void generate_gl_buffers(vao_state &_state,
	bool _alpha_included = false){

	glGenBuffers(1, &_state.p_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_vertexbuffer);

	glGenBuffers(1, &_state.p_colours_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_colours_vbo);

	glGenVertexArrays(1, &_state.p_vao);
	glBindVertexArray(_state.p_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_vertexbuffer);
	glVertexAttribPointer(0,  // attribute 0. (id)
		3,   // size
		GL_FLOAT,  // type
		GL_FALSE,  // normalized?
		0,        // stride
		NULL);     // array buffer offset
	int number_of_color_data_elements = _alpha_included ? 4 : 3;
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_colours_vbo);
	glVertexAttribPointer(1, number_of_color_data_elements, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

}

inline void generate_gl_buffers(vao_state_normals &_state,
	bool _alpha_included = false){

	glGenBuffers(1, &_state.p_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_vertexbuffer);

	glGenBuffers(1, &_state.p_colours_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_colours_vbo);

	glGenBuffers(1, &_state.p_normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_normals_vbo);

	glGenVertexArrays(1, &_state.p_vao);
	glBindVertexArray(_state.p_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_vertexbuffer);
	glVertexAttribPointer(0,  // attribute 0. (id)
		3,   // size
		GL_FLOAT,  // type
		GL_FALSE,  // normalized?
		0,        // stride
		NULL);     // array buffer offset

	int number_of_color_data_elements = _alpha_included ? 4 : 3;
	glBindBuffer(GL_ARRAY_BUFFER, _state.p_colours_vbo);
	glVertexAttribPointer(1, number_of_color_data_elements, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, _state.p_normals_vbo);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

}

inline static void delete_vao_state(vao_state &_state){
	if (_state.p_vertexbuffer)
		glDeleteBuffers(1, &_state.p_vertexbuffer);
	if (_state.p_colours_vbo)
		glDeleteBuffers(1, &_state.p_colours_vbo);
	if (_state.p_vao)
		glDeleteVertexArrays(1, &_state.p_vao);
}

inline static void delete_vao_state(vao_state_normals &_state){
	if (_state.p_vertexbuffer)
		glDeleteBuffers(1, &_state.p_vertexbuffer);
	if (_state.p_colours_vbo)
		glDeleteBuffers(1, &_state.p_colours_vbo);
	if (_state.p_normals_vbo)
		glDeleteBuffers(1, &_state.p_normals_vbo);
	if (_state.p_vao)
		glDeleteVertexArrays(1, &_state.p_vao);
}

#endif