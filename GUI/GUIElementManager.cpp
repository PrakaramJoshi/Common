#include <GUIElementManager.h>
#include <ShadersUtility.h>
#include <TextManager.h>
#include <chrono>
using namespace AceGUI;

GUIElementManager::GUIElementManager(){
	m_programID = 0;
	m_last_click_time = std::chrono::high_resolution_clock::now();
	m_click_time_delay = 300;
	m_last_id = 0;
	load_shaders();
}
void GUIElementManager::load_shaders(){

	if (m_programID)
		glDeleteProgram(m_programID);
	std::string shader_vs = R"(B:\Workspace\Common\GUI\Shaders\vertexcolor_gui_vs.glsl)";
	std::string shader_fs = R"(B:\Workspace\Common\GUI\Shaders\vertexcolor_gui_fs.glsl)";
	std::vector<std::pair<std::string, GLenum>> shaders;

	shaders.push_back(std::make_pair(shader_vs, GL_VERTEX_SHADER));
	shaders.push_back(std::make_pair(shader_fs, GL_FRAGMENT_SHADER));
	m_programID = GetProgramID_FileShaders(shaders);

	CHECK_GL_ERROR

}

GUIElementManager&  GUIElementManager::Instance(){
	static GUIElementManager guimanager;
	return guimanager;
}

void GUIElementManager::set_window_size(const int _width,
	const int _height){

	m_window_width = _width;
	m_window_height = _height;
	Ace::TextManager::InstanceGUI().on_window_resize(m_window_width, m_window_height);
}

unsigned int GUIElementManager::add(BaseGUIElement*_e){
	if (_e){
		if (m_gui_by_objects.find(_e) == m_gui_by_objects.end()){
			// do not add the same element twice
			m_last_id++;
			m_gui_by_id[m_last_id] = _e;
			m_gui_by_objects[_e] = m_last_id;
			return m_last_id;
		}
	}
	return 0;
}

void GUIElementManager::remove(const unsigned int _id){
	auto iter = m_gui_by_id.find(_id);
	if (iter != m_gui_by_id.end()){
		auto iter2 = m_gui_by_objects.find(iter->second);
		if (iter2 != m_gui_by_objects.end())
			m_gui_by_objects.erase(iter2);
		delete iter->second;
		m_gui_by_id.erase(iter);
	}
}

BaseGUIElement* GUIElementManager::get_by_id(const unsigned int _id){
	auto iter = m_gui_by_id.find(_id);
	if (iter != m_gui_by_id.end()){
		return (iter)->second;
	}
	return nullptr;
}

void  GUIElementManager::remove(BaseGUIElement*_e){
	auto iter = m_gui_by_objects.find(_e);
	if (iter != m_gui_by_objects.end()){
		auto iter2 = m_gui_by_id.find(iter->second);
		if (iter2 != m_gui_by_id.end())
			m_gui_by_id.erase(iter2);
		delete iter->first;
		m_gui_by_objects.erase(iter);
	}
}

void GUIElementManager::on_mouse_click(mouse_click_callback_params &_params,
	const double &_x,
	const double &_y){

	auto current_time = std::chrono::high_resolution_clock::now();
	auto duration = current_time - m_last_click_time;
	auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	bool isclick = false;

	if (milli < m_click_time_delay||true){
		isclick = _params.button() == m_last_button_clicked && m_last_action == 1 && _params.action()==0;
	}

	m_last_click_time = current_time;
	m_last_button_clicked = _params.button();
	m_last_action = _params.action();
	
	if (_params.button() == 0){
		for (auto e : m_gui_by_id){
			if (e.second->is_point_within_bounds(_x, _y)){
				if (_params.action() == 1){
					e.second->mouse_left_down();
				}
				else if (_params.action() == 0){
					e.second->mouse_left_up();
				}
				if (isclick){
					e.second->mouse_left_click();
				}
			}
			
		}
	}

	if (_params.button() == 1){
		for (auto e : m_gui_by_id){
			if (e.second->is_point_within_bounds(_x, _y)){
				if (_params.action() == 1){
					e.second->mouse_right_down();
				}
				else if (_params.action() == 0){
					e.second->mouse_right_up();
				}
				if (isclick){
					e.second->mouse_right_click();
				}
			}
		}
	}

	if (_params.button() == 2){
		for (auto e : m_gui_by_id){
			if (e.second->is_point_within_bounds(_x, _y)){
				if (_params.action() == 1){
					e.second->mouse_middle_down();
				}
				else if (_params.action() == 0){
					e.second->mouse_middle_up();
				}
				if (isclick){
					e.second->mouse_middle_click();
				}
			}
		}
	}
}

void GUIElementManager::on_mouse_move(const double &_x,
	const double &_y){

	for (auto e : m_gui_by_id){
		if (e.second->was_mouse_within_bounds()){
			// leave event possible 
			if (!e.second->is_point_within_bounds(_x, _y)){
				e.second->set_mouse_within_bounds(false);
				e.second->mouse_leave();
			}
		}
		else {
			// enter event possible 
			if (e.second->is_point_within_bounds(_x, _y)){
				e.second->set_mouse_within_bounds(true);
				e.second->mouse_enter();
			}
		}
	}
}

void GUIElementManager::enable_gui_gl_program(){
	glUseProgram(m_programID);
}

void  GUIElementManager::render(){
	glUseProgram(m_programID);
	for (auto e : m_gui_by_id)
		e.second->render();
	Ace::TextManager::InstanceGUI().render();
}

void GUIElementManager::clear(){
	std::vector<unsigned int>all_ids;
	all_ids.reserve(m_gui_by_id.size());
	for (auto e : m_gui_by_id){
		all_ids.push_back(e.first);
	}
	for (auto id : all_ids)
		remove(id);
	m_gui_by_objects.clear();
	m_gui_by_id.clear();
}

void GUIElementManager::clean_all(){
	enable_gui_gl_program();
	clear();
	glDeleteProgram(m_programID);
}

int GUIElementManager::get_window_width()const{
	return m_window_width;
}

int GUIElementManager::get_window_height()const{
	return m_window_height;
}

GUIElementManager::~GUIElementManager(){
}

void GUIElementManager::window_resized(const int _width,
	const int _height){
	m_scale = static_cast<double>(_height) / static_cast<double>(m_window_height);
	m_window_width = _width;
	m_window_height = _height;
	Ace::TextManager::InstanceGUI().on_window_resize(m_window_width, m_window_height);
	for (auto e : m_gui_by_id){
		e.second->window_sized(_width, _height);
	}
}