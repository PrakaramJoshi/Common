#include <AceButton.h>
#include <TextManager.h>
#include <GUIElementManager.h>
using namespace AceGUI;

Button::Button(){
	init_button("Button", 0.0, 0.0);
}

Button::Button(const std::string &_str,
	const float &_x,
	const float &_y){
	init_button(_str, _x, _y);
}

Button::Button(const std::string &_str){
	init_button(_str, 0.0, 0.0);
}

void Button::init_button(const std::string &_str,
	const float &_x,
	const float &_y){
	AceGUI::GUIElementManager::Instance().enable_gui_gl_program();
	m_min_size_x = 50;

	m_center_x = _x;
	m_center_y = _y;
	m_r_default = 7.0/255.0;
	m_r_current = 7.0/255.0;
	m_r_original = 7.0 / 255.0;
	m_g_default = 30.0/255.0;
	m_g_current = 30.0 / 255.0;
	m_g_original = 30.0 / 255.0;
	m_b_default = 50.0 / 255.0;
	m_b_current = 50.0 / 255.0;
	m_b_original = 50.0 / 255.0;
	generate_gl_buffers(m_box_filled,true);
	generate_gl_buffers(m_box_wire_frame,true);
	auto box_filled_vao = m_box_filled; 
	auto box_wire_frame_vao = m_box_wire_frame; 

	m_box_filled_data = new DrawingUtils::DrawnRectangleFilled(box_filled_vao.p_vertexbuffer, box_filled_vao.p_colours_vbo, box_filled_vao.p_vao);
	m_box_wire_frame_data = new DrawingUtils::DrawnRectangleWireFrame(box_wire_frame_vao.p_vertexbuffer, box_wire_frame_vao.p_colours_vbo, box_wire_frame_vao.p_vao);
	create(_str);

	add_default_events();
}

void Button::reset_min_size(){
	m_min_size_x = 50;
	update_button_box(get_text_size());
}
void Button::add_default_events(){

	// add the default events...
	auto this_ptr = this;
	auto mouse_down = [this_ptr](const AceGUI::call_back_params *){
		float r, g, b;
		this_ptr->get_current_color(r, g, b);
		r /= 5.0;
		g /= 5.0;
		b /= 5.0;
		this_ptr->set_current_color(r, g, b);
	};

	auto mouse_up = [this_ptr](const AceGUI::call_back_params *){
		float r, g, b;
		this_ptr->get_default_color(r, g, b);
		this_ptr->set_current_color(r, g, b);
	};

	auto mouse_enter = [this_ptr](const AceGUI::call_back_params *){
		float r, g, b;
		r = 1.0;
		g = 102.0/255.0;
		b = 0;
		this_ptr->set_default_color(r, g, b);
		this_ptr->set_current_color(r, g, b);
	};

	auto mouse_leave = [this_ptr](const AceGUI::call_back_params *){
		float r, g, b;
		this_ptr->get_original_color(r, g, b);
		this_ptr->set_default_color(r, g, b);
		this_ptr->set_current_color(r, g, b);
	};

	set_on_mouse_left_down(CREATE_CALLBACK(mouse_down));
	set_on_mouse_right_down(CREATE_CALLBACK(mouse_down));
	set_on_mouse_middle_down(CREATE_CALLBACK(mouse_down));


	set_on_mouse_left_up(CREATE_CALLBACK(mouse_up));
	set_on_mouse_right_up(CREATE_CALLBACK(mouse_up));
	set_on_mouse_middle_up(CREATE_CALLBACK(mouse_up));

	set_on_mouse_leave_event(CREATE_CALLBACK(mouse_leave));
	set_on_mouse_enter_event(CREATE_CALLBACK(mouse_enter));

}

Button::~Button(){
	delete_vao_state(m_box_filled);
	delete_vao_state(m_box_wire_frame);
	delete m_box_filled_data;
	delete m_box_wire_frame_data;
}

float Button::get_text_size(){
	return Ace::TextManager::InstanceGUI().get_text(m_text_id)->get_size_along_x();
}

void Button::update_button_box(const float  &_text_size_along_x){

	AceGUI::GUIElementManager::Instance().enable_gui_gl_program();
	m_box_filled_data->clear();
	m_box_wire_frame_data->clear();
	m_size_x = 2.0*(_text_size_along_x+10) / static_cast<double>(m_window_width);
	m_size_y = 2.0*(10.0*3.0 / static_cast<double>(m_window_height));

	m_size_x = std::max(2.0*m_min_size_x / static_cast<double>(m_window_width),(double)m_size_x);
	float x, y;
	gui_coordinate_to_gl_coordinate(m_center_x, m_center_y,x,y);

	DrawingUtils::RectangleXY box(x, y, 0, m_size_x, m_size_y, true, 0.8,0.8,0.8,m_r_current, m_g_current, m_b_current);
	
	box.get_colors(m_box_filled_data->get_colors_ref());
	DrawingUtils::add_alpha_to_rgb(m_box_filled_data->get_colors_ref(), 0.9);
	box.get_vertices(m_box_filled_data->get_vertices_ref());

	DrawingUtils::RectangleXY box_outline(x, y, 0, m_size_x, m_size_y, false, 0.8, 0.8, 0.8, m_r_current / 5.0, m_g_current / 5.0, m_b_current / 5.0);
	box_outline.get_colors(m_box_wire_frame_data->get_colors_ref());
	DrawingUtils::add_alpha_to_rgb(m_box_wire_frame_data->get_colors_ref(), 0.9);
	box_outline.get_vertices(m_box_wire_frame_data->get_vertices_ref());
}

std::pair<float, float> Button::get_text_location(const float &_size_along_x){

	float x, y;
	AceGL::Text::gui_coordinate_to_text_coordinate(m_window_height,(m_center_x - (_size_along_x/2.0)), m_center_y, x, y);
	return std::make_pair(x, y);
}

void Button::create(const std::string &_text){
	
	AceGL::Text *text = Ace::TextManager::InstanceGUI().create_only_text(_text);
	text->set_color(0.01, 0.01, 0.01);
	auto loc = get_text_location(text->get_size_along_x());
	text->set_location(loc.first, loc.second);

	m_text_id = Ace::TextManager::InstanceGUI().AddText(text);
	m_window_resized = false;
	update_button_box(text->get_size_along_x());
}

void Button::set_original_color(const float &_r,
	const float &_g,
	const float &_b){
	m_r_original = _r;
	m_g_original = _g;
	m_b_original = _b;
}

void Button::get_original_color(float &_r,
	float &_g,
	float &_b){
	_r = m_r_original;
	_g = m_g_original;
	_b = m_b_original;
}

void Button::set_default_color(const float &_r,
	const float &_g,
	const float &_b){
	m_r_default = _r;
	m_g_default = _g;
	m_b_default = _b;
	update_button_box(get_text_size());
}

void Button::set_current_color(const float &_r,
	const float &_g,
	const float &_b){
	m_r_current = _r;
	m_g_current = _g;
	m_b_current = _b;
	update_button_box(get_text_size());
}

void Button::get_current_color(float &_r,
	float &_g,
	float &_b){
	_r = m_r_current;
	_g = m_g_current;
	_b = m_b_current;
}

void Button::set_text(const std::string &_str){
	Ace::TextManager::InstanceGUI().update_text(m_text_id, _str);

	auto loc = get_text_location(get_text_size());
	Ace::TextManager::InstanceGUI().update_text_location(m_text_id, loc.first, loc.second);

	update_button_box(get_text_size());
}

void Button::set_text_color(const float &_r,
	const float &_g,
	const float &_b){

	Ace::TextManager::InstanceGUI().update_text_color(m_text_id, _r, _g, _b);
}

void Button::get_default_color(float &_r,
						float &_g,
						float &_b){
	_r = m_r_default;
	_g = m_g_default;
	_b = m_b_default;
}

void Button::set_size(const float &_size_x,
	const float &_size_y){
	m_size_x = _size_x / (static_cast<double>(m_window_width)/2.0);
	m_size_y = _size_y / (static_cast<double>(m_window_height)/2.0);
	update_button_box(get_text_size());
	update_text_location();
}

float Button::get_size_x(){
	return m_size_x* static_cast<double>(m_window_width)/2.0;
}

float Button::get_size_y(){
	return m_size_y *static_cast<double>(m_window_height)/2.0;
}

void Button::update_text_location(){
	auto loc = get_text_location(get_text_size());
	Ace::TextManager::InstanceGUI().update_text_location(m_text_id, loc.first, loc.second);
	m_window_resized = false;
}

void Button::set_center(const float &_center_x,
	const float &_center_y){
	m_center_x = _center_x;
	m_center_y = _center_y;
	update_text_location();
	update_button_box(get_text_size());
	m_window_resized = false;
}

void Button::set_min_size_x(const float &_x){
	m_min_size_x = _x;
	update_button_box(get_text_size());
}

float Button::get_min_size_x(){
	return m_min_size_x;
}

void Button::render(){
	if (m_window_resized)
	{
		update_text_location();
		update_button_box(get_text_size());
	}
	m_box_filled_data->draw(true);
	m_box_wire_frame_data->draw(true);
}

bool Button::is_point_within_bounds(const double &_x,
									const double &_y){
	float x, y;
	gui_coordinate_to_gl_coordinate(_x, _y, x, y);

	float normalized_center_x, normalized_center_y;
	gui_coordinate_to_gl_coordinate(m_center_x, m_center_y, normalized_center_x, normalized_center_y);

	return  (x <= (normalized_center_x + (m_size_x / 2.0)) && x >= (normalized_center_x - (m_size_x / 2.0))) &&
		(y <= (normalized_center_y + (m_size_y / 2.0)) && y >= (normalized_center_y - (m_size_y / 2.0)));

}