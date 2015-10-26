#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H
#include <glew.h>
#include <glfw3.h>
#include <stdio.h>
#include <tchar.h>
#include <string>

struct ViewPort{
	int p_start_x;
	int p_start_y;
	int p_width;
	int p_height;

	ViewPort(){
		p_start_x = 0;
		p_start_y = 0;
		p_width = 640;
		p_height = 480;

	};

	ViewPort(const int _start_x,
		const int _start_y,
		const int _width,
		const int _height){

		p_start_x = _start_x;
		p_start_y = _start_y;
		p_width = _width;
		p_height = _height;
	};
};

inline static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

inline static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		if (window == glfwGetCurrentContext())
			glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

static double lastTime = 0;
static unsigned int nbFrames = 0;

inline static void FPSStatus(GLFWwindow *_window){
	// Measure speed
	double currentTime = glfwGetTime();
	nbFrames++;

	if (currentTime - lastTime >= 1.0){ // If last cout was more than 1 sec ago
		char title[256];
		title[255] = '\0';
		double fps = nbFrames / (currentTime - lastTime);

		int viewPortWidth, viewPortHeight;
		glfwGetFramebufferSize(_window, &viewPortWidth, &viewPortHeight);
		sprintf_s(title, 255,
			"widthxHeight(%dx%d)- [FPS: %3.2f]",
			 viewPortWidth, viewPortHeight, fps);

		glfwSetWindowTitle(_window, title);

		nbFrames = 0;
		lastTime = glfwGetTime();
	}
}

inline static void FPSStatus(GLFWwindow *_window,const char *_app_title){
	// Measure speed
	double currentTime = glfwGetTime();
	nbFrames++;

	if (currentTime - lastTime >= 1.0){ // If last cout was more than 1 sec ago
		char title[256];
		title[255] = '\0';
		double fps = nbFrames / (currentTime - lastTime);

		int viewPortWidth, viewPortHeight;
		glfwGetFramebufferSize(_window, &viewPortWidth, &viewPortHeight);
		sprintf_s(title, 255,
			"%s widthxHeight(%dx%d)- [FPS: %3.2f]",
			_app_title, viewPortWidth, viewPortHeight, fps);

		glfwSetWindowTitle(_window, title);

		nbFrames = 0;
		lastTime = glfwGetTime();
	}
}

inline static void FPSStatus(GLFWwindow *_window, const char *_app_title, int _viewPortWidth, int _viewPortHeight){
	// Measure speed
	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0){ // If last cout was more than 1 sec ago
		char title[256];
		title[255] = '\0';
		double fps = nbFrames / (currentTime - lastTime);
		sprintf_s(title, 255,
			"%s widthxHeight(%dx%d)- [FPS: %3.2f]",
			_app_title, _viewPortWidth, _viewPortHeight, fps);

		glfwSetWindowTitle(_window, title);

		nbFrames = 0;
		lastTime = glfwGetTime();
	}
}

inline static GLFWwindow* CreateGlfwWindow(const std::string &_window_name,
							 const unsigned int &_width,
							 const unsigned int &_height){
	lastTime = glfwGetTime();
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()){
		return nullptr;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(_width, _height, _window_name.c_str(), NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return nullptr;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);
	return window;
}


class AceWindow{
	GLFWwindow* window;
	double m_lasttime;

	std::string m_window_name;
	unsigned int m_nbFrames;
	char m_title[256];
public:
	AceWindow(){
		m_window_name = "Application";
		m_lasttime = 0;
		m_nbFrames = 0;
		m_title[255] = '\0';
	}

	AceWindow(const std::string &_window_name) :m_window_name(_window_name){
		m_lasttime = 0;
		m_nbFrames = 0;
		m_title[255] = '\0';
	}
	~AceWindow(){
		if (window){
			glfwDestroyWindow(window);
			glfwTerminate();
		}
	}

	bool init(const unsigned int &_width,
		const unsigned int &_height,
		bool _full_screen=false){
		m_lasttime = glfwGetTime();
		glfwSetErrorCallback(error_callback);
		if (!glfwInit()){
			return false;
		}
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWmonitor *monitor = NULL;
		if (_full_screen){
			monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			window = glfwCreateWindow(mode->width, mode->height, m_window_name.c_str(), monitor, NULL);
		}
		else{
			window = glfwCreateWindow(_width, _height, m_window_name.c_str(), monitor, NULL);
		}
		
		if (!window)
		{
			fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);
		// Initialize GLEW
		glewExperimental = true; // Needed for core profile
		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			return false;
		}

		// Ensure we can capture the escape key being pressed below
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		glfwSetKeyCallback(window, key_callback);
		return true;
	}

	void FPSStatus(int _viewPortWidth, int _viewPortHeight){
		// Measure speed
		double currentTime = glfwGetTime();
		m_nbFrames++;
		double time_delta = currentTime - m_lasttime;
		if (time_delta >= 1.0){ // If last cout was more than 1 sec ago
			
			
			double fps = m_nbFrames / time_delta;
			sprintf_s(m_title, 255,
				"%s widthxHeight(%dx%d)- [FPS: %3.2f]",
				m_window_name.c_str(), _viewPortWidth, _viewPortHeight, fps);

			glfwSetWindowTitle(window, m_title);

			m_nbFrames = 0;
			m_lasttime = glfwGetTime();
		}
	}

	void set_as_current_context(){
		glfwMakeContextCurrent(window);
	}

	GLFWwindow *get_window(){
		return window;
	}

	void set_title(const std::string &_title){
		m_window_name = _title; 
	}

};

class AceDialog{
	GLFWwindow* window;
	GLFWwindow *m_parent;
	double m_lasttime;

	std::string m_window_name;
	unsigned int m_nbFrames;
	char m_title[256];
	
public:
	AceDialog(){
		m_window_name = "Dailog";
		m_lasttime = 0;
		m_nbFrames = 0;
		m_title[255] = '\0';
		m_parent = glfwGetCurrentContext();
	}

	AceDialog(const std::string &_window_name) :m_window_name(_window_name){
		m_lasttime = 0;
		m_nbFrames = 0;
		m_title[255] = '\0';
		m_parent = glfwGetCurrentContext();
	}
	~AceDialog(){
		if (window){
			glfwDestroyWindow(window);
		}
		glfwMakeContextCurrent(m_parent);
	}

	bool show(const unsigned int &_width,
		const unsigned int &_height){
		m_lasttime = glfwGetTime();
		glfwSetErrorCallback(error_callback);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window = glfwCreateWindow(_width, _height, m_window_name.c_str(), NULL, NULL);

		if (!window)
		{
			fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
			return false;
		}
		glfwMakeContextCurrent(window);

		// Ensure we can capture the escape key being pressed below
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		glfwSetKeyCallback(window, key_callback);
		return true;
	}

	void FPSStatus(int _viewPortWidth, int _viewPortHeight){
		// Measure speed
		double currentTime = glfwGetTime();
		m_nbFrames++;
		double time_delta = currentTime - m_lasttime;
		if (time_delta >= 1.0){ // If last cout was more than 1 sec ago


			double fps = m_nbFrames / time_delta;
			sprintf_s(m_title, 255,
				"%s widthxHeight(%dx%d)- [FPS: %3.2f]",
				m_window_name.c_str(), _viewPortWidth, _viewPortHeight, fps);

			glfwSetWindowTitle(window, m_title);

			m_nbFrames = 0;
			m_lasttime = glfwGetTime();
		}
	}

	GLFWwindow *get_window(){
		return window;
	}

	void set_title(const std::string &_title){
		m_window_name = _title;
	}

};
#endif