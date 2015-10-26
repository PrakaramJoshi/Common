#include <TextManager.h>
#include <CGMath.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
using namespace Ace;
using namespace  AceGL;
TextManager::TextManager(){
	m_last_id = 0;
	m_text_renderer = new TextRender();
}

TextManager& TextManager::Instance(){
	static TextManager text_manager;
	return text_manager;
}

TextManager& TextManager::InstanceGUI(){
	static TextManager text_manager;
	return text_manager;
}

unsigned int TextManager::AddText(const std::string &_str){
	return AddText(Text(get_font_manager(),_str));
}

unsigned int TextManager::AddText(const std::string &_str, 
	const float &_x, 
	const float &_y){
	return AddText(Text(get_font_manager(), _str, _x, _y));
}

unsigned int TextManager::AddText(const Text &_t){
	
	return AddText(new Text(_t));
}

unsigned int TextManager::AddText(Text *_t){
	m_last_id++;
	m_text[m_last_id] = _t;
	refresh();
	return m_last_id;
}

AceGL::Text *TextManager::create_only_text(const std::string &_str){
	return new Text(get_font_manager(), _str);
}

AceGL::Text *TextManager::create_only_text(const std::string &_str,
											const float &_x,
											const float &_y){
	return new Text(get_font_manager(), _str,_x,_y);
}

bool TextManager::update_text(const unsigned int _id,
	Text &_new_text){
	auto iter = m_text.find(_id);
	if (iter == m_text.end())
		return false;

	(*(*iter).second) = _new_text;
	refresh();
	return true;
}

bool TextManager::update_text_location(const unsigned int _id,
	const float &_x,
	const float &_y){

	auto iter = m_text.find(_id);
	if (iter == m_text.end())
		return false;

	(*iter).second->set_location(_x,_y);
	refresh();
	return true;
}

bool TextManager::update_text_color(const unsigned int _id,
	const float &_r,
	const float &_g,
	const float &_b){

	auto iter = m_text.find(_id);
	if (iter == m_text.end())
		return false;

	(*iter).second->set_color(_r, _g, _b);
	refresh();
	return true;
}

bool TextManager::update_text(const unsigned int _id,
	const std::string &_str){

	auto iter = m_text.find(_id);
	if (iter == m_text.end())
		return false;

	(*iter).second->set_text(_str);
	refresh();
	return true;
}

Text * TextManager::get_text(const unsigned int _id){
	auto iter = m_text.find(_id);
	if (iter == m_text.end())
		return nullptr;
	return (*iter).second;
}

void TextManager::remove_text(const unsigned int _id){
	auto iter = m_text.find(_id);
	if (iter != m_text.end()){
		delete (*iter).second;
		m_text.erase(iter);
		refresh();
	}
}

void TextManager::remove_text(const float &_x, 
	const float &_y,
	bool _all){
	bool deleted = false;
	for (auto iter = m_text.begin(); iter != m_text.end();){
		if (CGMath::GeoEqual(_x, (*iter).second->get_x()) && CGMath::GeoEqual(_y, (*iter).second->get_y())){
			delete (*iter).second;
			iter = m_text.erase(iter);
			if (!_all){
				deleted = true;
				break;
			}
		}
		else
			++iter;
	}
	if (deleted)
		refresh();
	
}

void TextManager::scale_size(const unsigned int _id,
	const float &_factor){
	auto text = get_text(_id);
	if (text){
		text->scale_size(_factor);
	}
}

void TextManager::set_size(const unsigned int _id,
	const float &_size){
	auto text = get_text(_id);
	if (text){
		text->set_size(_size);
	}
}

void TextManager::render(){
	m_text_renderer->display();
}

void TextManager::on_window_resize(const unsigned int _view_port_width,
	const unsigned int _view_port_height){
	m_text_renderer->on_window_resize(_view_port_width, _view_port_height);
}

void TextManager::refresh(){
	m_text_renderer->clear_text();
	for (auto t : m_text){
		m_text_renderer->append(*t.second);
	}
}

font_manager_t * TextManager::get_font_manager(){
	return m_text_renderer->get_font_manager();
}

TextManager::~TextManager(){
	for (auto t : m_text){
		delete t.second;
	}
	delete m_text_renderer;
}