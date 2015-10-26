#include <MenuBar.h>
#include <AceButton.h>
#include <GUIElementManager.h>
using namespace AceGUI;

MenuBar::MenuBar(bool _vertical_bar){
	m_x = 0;
	m_y = 0;
	m_vertical_bar = _vertical_bar;
}

void MenuBar::get_next_x_y_horizontal_menu_bar(Button* _button,
	float &_x,
	float &_y){
	auto height = get_window_height();
	auto width = get_window_width();
	auto size_x = _button->get_size_x();
	float size_y = _button->get_size_y();

	if (m_x + size_x > width){
		//start next row
		m_x = 0;
		m_y = m_y + _button->get_size_y() + 5;
	}
	_x = m_x + (size_x / 2.0);
	_y = m_y + (size_y / 2.0);

	m_x = m_x + size_x + 5;
}

float MenuBar::get_max_width()const{
	float max_width = std::numeric_limits<float>::min();
	for (auto b : m_buttons){
		if (b.second->get_size_x() > max_width)
			max_width = b.second->get_size_x();
	}
	if (max_width <= 0)
		max_width = 0;
	return max_width;

}

void MenuBar::set_buttons_to_max_width(){
	auto max_width = get_max_width();
	for (auto b : m_buttons)
		b.second->set_min_size_x(max_width);
	refresh();
}

void MenuBar::get_next_x_y_vertical_menu_bar(Button* _button,
	float &_x,
	float &_y){
	auto height = get_window_height();
	auto width = get_window_width();
	auto size_x = _button->get_size_x();
	float size_y = _button->get_size_y();

	if (m_y + size_y > height){
		//start next row
		m_x = m_x+get_max_width()+5;
		m_y = 0;
	}
	_x = m_x + (size_x / 2.0);
	_y = m_y + (size_y / 2.0);

	m_y = m_y + size_y + 5;
}

void MenuBar::get_next_x_y(Button* _button,
	float &_x,
	float &_y){
	if (m_vertical_bar)
		get_next_x_y_vertical_menu_bar(_button, _x, _y);
	else
		get_next_x_y_horizontal_menu_bar(_button, _x, _y);
}
unsigned int MenuBar::add_button(const std::string &_str){

	auto button = new Button(_str);
	float x, y;
	get_next_x_y(button, x, y);
	button->set_center(x ,y);

	m_buttons[button->get_id()] = button;

	set_buttons_to_max_width();

	return button->get_id();
}

unsigned int MenuBar::add_button(const std::string &_str, Callback_base *_on_click){
	auto id =add_button(_str);
	set_on_click(id, _on_click);
	return id;
}

void MenuBar::set_to_minimum_size(){
	for (auto b : m_buttons)
		b.second->reset_min_size();
}

void MenuBar::set_on_click(unsigned int _id, Callback_base *_on_click){
	auto b = AceGUI::GUIElementManager::Instance().get_by_id(_id);
	if (b)
		b->set_on_mouse_left_click(_on_click);
}

void MenuBar::set_text(const unsigned int _id,
	const std::string &_str){
	auto b = AceGUI::GUIElementManager::Instance().get_by_id(_id);
	if (b){
		set_to_minimum_size();
		static_cast<AceGUI::Button*>(b)->set_text(_str);
		if (m_vertical_bar)
			set_buttons_to_max_width();
		else
			refresh();
	}
}

void MenuBar::remove_button(const unsigned int _id,
	bool _refresh){
	auto iter = m_buttons.find(_id);
	if (iter != m_buttons.end()){
		m_buttons.erase(iter);
		AceGUI::GUIElementManager::Instance().remove(_id);
	}
	if (_refresh)
		refresh();
}

void MenuBar::add_default_events(){

}

void MenuBar::render(){

}

bool MenuBar::is_point_within_bounds(const double &_x, const double &_y){
	for (auto b : m_buttons){
		if (b.second->is_point_within_bounds(_x, _y))
			return true;
	}
	return false;
	
}

void MenuBar::refresh(){
	m_x = 0;
	m_y = 0;
	auto height = get_window_height();
	auto width = get_window_width();
	for (auto b : m_buttons){
		float x, y;
		get_next_x_y(b.second, x, y);
		b.second->set_center(x, y);
	}
	
}

void MenuBar::window_sized(const int _width,
	const int _height){
	BaseGUIElement::window_sized(_width, _height);
	refresh();

}

bool MenuBar::toggleOrientation(){
	m_vertical_bar = !m_vertical_bar;
	set_to_minimum_size();
	refresh();
	return m_vertical_bar; 
}

void MenuBar::animate(){

}

MenuBar::~MenuBar(){
	for (auto b:m_buttons)
		AceGUI::GUIElementManager::Instance().remove(b.first);
	m_buttons.clear();
}