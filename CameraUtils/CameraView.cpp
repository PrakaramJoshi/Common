#include <CameraView.h>
#include <OpenGL\glfwWindow.h>
#include <iostream>
#include <CGMath.h>
int mouse_roll=0;
void scroll(GLFWwindow* window, double x, double y){
	if (window != glfwGetCurrentContext())
		return;
	mouse_roll = y;
}


CameraView::CameraView(GLFWwindow* _window, float _fov,
	float _zNear,
	float _zFar,
	float _zoom_factor,
	bool _auto_rotate,
	float _rotate_x,
	float _rotate_y,
	float _rotate_z,
	float _rotate_about_x,
	float _rotate_about_y,
	float _rotate_about_z,
	float _rotation_speed,
	bool _use_phong_lighting)
{
	window = _window;
	m_zFar = _zFar;
	m_zNear = _zNear;
	m_zoom_factor = _zoom_factor;
	block = nullptr;

	speed = .5f; // 3 units / second
	mouseSpeed = 0.005f;
	m_inital_fov = _fov;
	Reset();
	//hAngle = 0;
	//vAngle = 0;
	glfwSetScrollCallback(window, scroll);
	m_auto_rotate = _auto_rotate;
	m_rotation_axis = { _rotate_about_x != 0 ? 1 : 0, _rotate_about_y != 0 ? 1 : 0, _rotate_about_z != 0 ? 1 : 0 };
	m_rotate_about = { _rotate_x, _rotate_y, _rotate_z };
	m_rotate_speed = _rotation_speed;

	m_pre_height = 0;
	m_pre_width = 0;
	m_model = glm::mat4(1.0f);
	m_apply_phong_lighting = _use_phong_lighting;
	m_lights_on = _use_phong_lighting;
	
	if (!m_apply_phong_lighting)
		generate_gl_uniform_buffer<uniforms_block>(uniforms_buffer);
	else
		uniforms_buffer = 0;
}

bool CameraView::setup_phong_lighting(GLuint _programID){
	m_programID = _programID;
	if (!m_apply_phong_lighting){
		AceLogger::Log("camera not constructed to use phong lighting", AceLogger::LOG_ERROR);
		return false;
	}	
	if (m_phong.setup(_programID)){
		m_phong.set_lighting_parameters(100, glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.3, 0.3, 0.3));
		return true;
	}
	return false;
}

bool CameraView::move(const int &_viewPortWidth,
	const int &_viewPortHeight,
	glm::mat4 &_view,
	glm::mat4 &_model,
	glm::mat4 &_projection){
	//viewPortHeight = _viewPortHeight;
	//viewPortWidth = _viewPortWidth;
	if (computeMatricesFromInputs(_viewPortWidth, _viewPortHeight, _view, _model, _projection,true)){
		m_mvp = _projection* _view* _model;
		update_gpu();
		return true;
	}
	return false;

}

bool CameraView::move(const int &_viewPortWidth,
	const int &_viewPortHeight){
	if (computeMatricesFromInputs(_viewPortWidth, _viewPortHeight, m_temp_view, m_temp_model, m_temp_projection,false)){
		// Our ModelViewProjection : multiplication of our 3 matrices
		m_mvp = m_temp_projection* m_temp_view* m_temp_model;// Remember, matrix multiplication is the other way around;
		update_gpu();
		return true;
	}
	else{
		return false;
	}
}

void CameraView::change_camera_rotation_speed_by(const float &_delta){
	m_rotate_speed += _delta;
}

void CameraView::Reset(){
	// Initial position : on +Z
	position = glm::vec3(0.5, 0.5, 2);
	m_model = glm::mat4(1.0f);
	//Look at -Z
	m_horizontal_angle = -CGConstants::PI;

	// Look straight
	m_vertical_angle = 0;
	// Initial Field of View
	m_fov = m_inital_fov;

	lastTime = glfwGetTime();

	m_pre_height = 0;
	m_pre_width = 0;
}

bool CameraView::computeMatricesFromInputs(const int &_viewPortWidth,
	const int &_viewPortHeight,
	glm::mat4 &_view,
	glm::mat4 &_model,
	glm::mat4 &_projection,
	bool _update_matrices_always){

	bool update_matrix = false;

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
		Reset();
		update_matrix = true;
	}

	if (_viewPortHeight != m_pre_height || _viewPortWidth != m_pre_width){
		m_pre_height = _viewPortHeight;
		m_pre_width = _viewPortWidth;
		update_matrix = true;
	}

	
	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	lastTime = currentTime;
	// Compute new orientation

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
		m_vertical_angle -= mouseSpeed;
		update_matrix = true;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		m_vertical_angle += mouseSpeed;
		update_matrix = true;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ){
		m_horizontal_angle -= mouseSpeed;
		update_matrix = true;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		m_horizontal_angle += mouseSpeed;
		update_matrix = true;
	}
	if (update_matrix){
		if (m_horizontal_angle >= CGConstants::PI2)
			m_horizontal_angle = 0;
		if (m_vertical_angle >= CGConstants::PI2)
			m_vertical_angle = 0;
		if (m_vertical_angle < 0)
			m_vertical_angle += CGConstants::PI2;
		if (m_horizontal_angle < 0)
			m_horizontal_angle += CGConstants::PI2;

		// Direction : Spherical coordinates to Cartesian coordinates conversion
		m_direction =glm::vec3 (
			cos(m_vertical_angle) * sin(m_horizontal_angle),
			sin(m_vertical_angle),
			cos(m_vertical_angle)*cos(m_horizontal_angle)
			);

		// Right vector
		m_right = glm::vec3(
			sin(m_horizontal_angle - CGConstants::M_PI_2),
			0,
			cos(m_horizontal_angle - CGConstants::M_PI_2)
			);

		// Up vector
		m_up = glm::cross(m_right, m_direction);
	}


	
	
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		position += m_direction * deltaTime * speed;
		update_matrix = true;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		position -= m_direction * deltaTime * speed;
		update_matrix = true;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		position += m_right * deltaTime * speed;
		update_matrix = true;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		position -= m_right * deltaTime * speed;
		update_matrix = true;
	}

	// move up
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
		position += m_up * deltaTime * speed;
		update_matrix = true;
	}
	// move down
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
		position -= m_up * deltaTime * speed;
		update_matrix = true;
	}
	if (mouse_roll)
		update_matrix = true;

	if (!update_matrix){
		if (m_auto_rotate){
			m_model = glm::translate(m_model, -m_rotate_about);
			m_model = glm::rotate(m_model, m_rotate_speed*deltaTime, m_rotation_axis);
			m_model = glm::translate(m_model, m_rotate_about);
			_model = m_model;
			if (_update_matrices_always){
				_view = m_view;
				_projection = m_projection;
			}
			return true;
		}
		if (_update_matrices_always){
			_view = m_view;
			_projection = m_projection;
			_model = m_model;
		}
		
		return false;
	}
	
	m_fov = m_fov - m_zoom_factor * mouse_roll; // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
	if (m_fov <= 0)
		m_fov = 0.01;
	mouse_roll = 0;
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	_projection = glm::perspective(m_fov, (float)_viewPortWidth / (float)_viewPortHeight, m_zNear, m_zFar);
	// Camera matrix
	_view = glm::lookAt(
		position,           // Camera is here
		position + m_direction, // and looks here : at the same position, plus "direction"
		m_up                  // Head is up (set to 0,-1,0 to look upside-down)
		);

	if (m_auto_rotate){
		m_model = glm::translate(m_model, -m_rotate_about);
		m_model = glm::rotate(m_model, m_rotate_speed*deltaTime, m_rotation_axis);
		m_model = glm::translate(m_model, m_rotate_about);
	}

	_model = m_model;

	m_view = _view;
	m_projection = _projection;
	return true;
}

void CameraView::update_gpu(){
	if (!m_apply_phong_lighting){
		if (!block){
			//uniforms_buffer=glGetUniformLocation(m_programID, "mvp");
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
			block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER,
				0,
				sizeof(uniforms_block),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			block->mvp = m_mvp;
			//glUnmapBuffer(GL_UNIFORM_BUFFER);
		}
		else
			block->mvp = m_mvp;
	}
	else{
		glm::vec3 camera_pos;
		camera_pos = { get_camera_x(), get_camera_y(), get_camera_z() };
		m_phong.render_lighting(m_view, m_model, m_projection, camera_pos);
	}
}
bool CameraView::toggle_lighting(){
	if (m_apply_phong_lighting){
		glUseProgram(m_programID);
		m_lights_on = !m_lights_on;
		if (m_lights_on)
			m_phong.turn_on_lighting();
		else
			m_phong.turn_off_ligting();
		update_gpu();
		return m_lights_on;
	}
	return false;
}

float CameraView::get_camera_x(){
	return position[0];
}

float CameraView::get_camera_y(){
	return position[1];
}

float CameraView::get_camera_z(){
	return position[2];
}

bool CameraView::toggle_auto_rotate(){
	if (!m_apply_phong_lighting){
		AceLogger::Log("Cannot toggle lights, camera not contructed with lights!", AceLogger::LOG_ERROR);
		return false;
	}
	m_auto_rotate = !m_auto_rotate;
	return m_auto_rotate;
}

CameraView::~CameraView()
{
	if (block)
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	if (uniforms_buffer)
		glDeleteBuffers(1, &uniforms_buffer);

}
