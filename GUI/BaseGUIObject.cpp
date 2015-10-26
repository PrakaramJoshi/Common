#include <BaseGUIObject.h>
#include <GUIElementManager.h>

using namespace AceGUI;

BaseGUIElement::BaseGUIElement(){

	m_window_width = AceGUI::GUIElementManager::Instance().get_window_width();
	m_window_height = AceGUI::GUIElementManager::Instance().get_window_height();
	m_window_resized = false;

	AceGUI::GUIElementManager::Instance().enable_gui_gl_program();
	m_id=AceGUI::GUIElementManager::Instance().add(this);
	m_mouse_within_bounds = false;
};

unsigned int BaseGUIElement::get_id()
{
	return m_id;
}

void BaseGUIElement::set_on_mouse_left_click(Callback_base *_event){
	m_mouse_left_click.push_back(_event);
}

void BaseGUIElement::set_on_mouse_right_click(Callback_base *_event){
	m_mouse_right_click.push_back(_event);
}

void BaseGUIElement::set_on_mouse_middle_click(Callback_base *_event){
	m_mouse_middle_click.push_back(_event);
}

void BaseGUIElement::set_on_mouse_left_down(Callback_base *_event){
	m_mouse_left_down.push_back(_event);
}

void BaseGUIElement::set_on_mouse_left_up(Callback_base *_event){
	m_mouse_left_up.push_back(_event);
}

void BaseGUIElement::set_on_mouse_right_down(Callback_base *_event){
	m_mouse_right_down.push_back(_event);
}

void BaseGUIElement::set_on_mouse_right_up(Callback_base *_event){
	m_mouse_right_up.push_back(_event);
}

void BaseGUIElement::set_on_mouse_middle_down(Callback_base *_event){
	m_mouse_middle_down.push_back(_event);
}

void BaseGUIElement::set_on_mouse_middle_up(Callback_base *_event){
	m_mouse_middle_up.push_back(_event);
}

void BaseGUIElement::set_on_mouse_leave_event(Callback_base *_event){
	m_mouse_leave.push_back(_event);
}

void BaseGUIElement::set_on_mouse_enter_event(Callback_base *_event){
	m_mouse_enter.push_back(_event);
}

void BaseGUIElement::exec_event_vector(std::vector<Callback_base*> &_events,
	mouse_click_callback_params &_params){

	for (auto f : _events){
		f->exec(&_params);
	}
}

void BaseGUIElement::exec_event_vector(std::vector<Callback_base*> &_events, 
	const call_back_params *_params){
	for (auto f : _events){
		f->exec(_params);
	}
}

void BaseGUIElement::mouse_left_click(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_left_click, &button_click);
}

void BaseGUIElement::mouse_right_click(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_right_click, &button_click);
}

void BaseGUIElement::mouse_middle_click(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_middle_click, &button_click);
}

void BaseGUIElement::mouse_left_down(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_left_down, &button_click);
}

void BaseGUIElement::mouse_left_up(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_left_up, &button_click);
}

void BaseGUIElement::mouse_right_down(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_right_down, &button_click);
}

void BaseGUIElement::mouse_right_up(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_right_up, &button_click);
}

void BaseGUIElement::mouse_middle_down(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_middle_down, &button_click);
}

void BaseGUIElement::mouse_middle_up(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_middle_up, &button_click);
}

void BaseGUIElement::mouse_enter(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_enter, &button_click);
}

void BaseGUIElement::mouse_leave(){
	button_click_params button_click(m_id);
	exec_event_vector(m_mouse_leave, &button_click);
}


bool BaseGUIElement::was_mouse_within_bounds(){
	return m_mouse_within_bounds;
}

void BaseGUIElement::set_mouse_within_bounds(bool _within_bounds){
	m_mouse_within_bounds = _within_bounds;
}

void BaseGUIElement::clear_event_container(std::vector<Callback_base*> &_events){
	for (auto e : _events)
		delete e;
	_events.clear();
}

void BaseGUIElement::window_sized(const int _width,
	const int _height){
	m_window_width = _width;
	m_window_height = _height;
	m_window_resized = true;
}

int BaseGUIElement::get_window_width(){
	return m_window_width;
}

int BaseGUIElement::get_window_height(){
	return m_window_height;
}

void BaseGUIElement::gui_coordinate_to_gl_coordinate(const float &_gui_x,
	const float &_gui_y,
	float &_gl_x,
	float &_gl_y){

	float center_x = static_cast<float>(m_window_width) / 2.0;
	float center_y = static_cast<float>(m_window_height) / 2.0;

	_gl_x = (_gui_x - center_x) / (static_cast<float>(m_window_width) / 2.0);
	_gl_y = (center_y - _gui_y) / (static_cast<float>(m_window_height) / 2.0);
}

void BaseGUIElement::gl_coordinate_to_gui_coordinate(const float &_gl_x,
	const float &_gl_y,
	float &_gui_x,
	float &_gui_y){

	float center_x = static_cast<float>(m_window_width) / 2.0;
	float center_y = static_cast<float>(m_window_height) / 2.0;

	_gui_x = center_x + (_gl_x*(static_cast<float>(m_window_width) / 2.0));
	_gui_y = center_y - (_gl_y*(static_cast<float>(m_window_height) / 2.0));
}

BaseGUIElement::~BaseGUIElement(){
	clear_event_container(m_mouse_middle_up);
	clear_event_container(m_mouse_middle_down);
	clear_event_container(m_mouse_middle_click);

	clear_event_container(m_mouse_right_up);
	clear_event_container(m_mouse_right_down);
	clear_event_container(m_mouse_right_click);

	clear_event_container(m_mouse_left_up);
	clear_event_container(m_mouse_left_down);
	clear_event_container(m_mouse_left_click);

	clear_event_container(m_mouse_enter);
	clear_event_container(m_mouse_leave);
}