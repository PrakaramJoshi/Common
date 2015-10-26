#ifndef CAMERA_VIEW_H
#define CAMERA_VIEW_H
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <OpenGL\ShadersUtility.h>
#include <Geometry\DrawingUtils.h>
struct GLFWwindow;
class CameraView
{
	struct uniforms_block
	{
		glm::mat4 mvp;
	};

	GLuint          uniforms_buffer;
		
	glm::vec3 position;
	
	float m_vertical_angle;
	
	float m_horizontal_angle;
	
	float m_fov;

	float speed;
	float mouseSpeed;
	double lastTime;

	GLFWwindow* window;

	float m_zNear;
	float m_zFar;

	float m_zoom_factor;

	float m_inital_fov;

	bool m_auto_rotate;
	glm::vec3 m_rotate_about;
	glm::vec3 m_rotation_axis;
	float m_rotate_speed;

	unsigned int m_pre_width;
	unsigned int m_pre_height;
	glm::mat4 m_view;
	glm::mat4 m_model;
	glm::mat4 m_projection;
	glm::mat4 m_mvp;

	glm::mat4 m_temp_view;
	glm::mat4 m_temp_model;
	glm::mat4 m_temp_projection;

	glm::vec3 m_direction;
	glm::vec3 m_right;
	glm::vec3 m_up;

	uniforms_block * block;

	DrawingUtils::PhongLighting m_phong;

	bool m_apply_phong_lighting;

	bool m_lights_on;

	GLuint m_programID;

	bool computeMatricesFromInputs(const int &_viewPortWidth,
		const int &_viewPortHeight,
		glm::mat4 &_view,
		glm::mat4 &_model,
		glm::mat4 &_projection,
		bool _update_matrices_always);

	void update_gpu();

public:
	CameraView(GLFWwindow* _window,
			float _fov,
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
			bool _use_phong_lighting=false);

	bool setup_phong_lighting(GLuint _programID);

	~CameraView();

	bool move(const int &_viewPortWidth,
		const int &_viewPortHeight,
		glm::mat4 &_view,
		glm::mat4 &_model,
		glm::mat4 &_projection);

	bool move(const int &_viewPortWidth,
		const int &_viewPortHeight);

	bool move(const int &_viewPortWidth,
		const int &_viewPortHeight,
		glm::mat4 &_mvp){
	
		if (computeMatricesFromInputs(_viewPortWidth, _viewPortHeight, m_temp_view, m_temp_model, m_temp_projection,false)){
			// Our ModelViewProjection : multiplication of our 3 matrices
			_mvp = m_temp_projection* m_temp_view* m_temp_model;// Remember, matrix multiplication is the other way around
			m_mvp = _mvp;
			update_gpu();
			return true;
		}
		else{
			_mvp = m_mvp;
			return false;
		}
		
	}

	bool toggle_auto_rotate();

	glm::mat4& get_projection(){
		return m_projection;
	};

	glm::mat4& get_model(){
		return m_model;
	};

	glm::mat4& get_view(){
		return m_view;
	};

	void Reset();

	float get_camera_x();

	float get_camera_y();

	float get_camera_z();

	void change_camera_rotation_speed_by(const float &_delta);

	bool toggle_lighting();
};
#endif

