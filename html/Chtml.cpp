#include "stdafx.h"
#include "Chtml.h"
#include <cctype>
#include <boost\algorithm\string.hpp>
#include <boost\algorithm\string\replace.hpp>
#include <set>
using namespace HTML;

Chtml::Chtml()
{
	m_text = "";
}
Chtml::~Chtml()
{
}

Chtml::Chtml(const std::string &_text){
	set_text(_text);
}

void Chtml::set_text(const std::string &_text){
	m_text = _text;
	remove_reserved_tokens();
}

void Chtml::remove_html_reserved_tokens(std::string &_str){
	for (int i = 0; i < RESERVED_CHARS::TOTAL_RESERVED; i++)
		boost::replace_all(_str, reserved_chars_tokens[i], reserved_chars_value[i]);
}

void Chtml::remove_reserved_tokens(){
	remove_html_reserved_tokens(m_text);
}

void Chtml::get_ignore_html_tags(std::vector<HTML::TAGS> &_tags)const{

	_tags.push_back(HTML::doctype);
	_tags.push_back(HTML::applet);
	_tags.push_back(HTML::audio);
	_tags.push_back(HTML::button);
	_tags.push_back(HTML::canvas);
	_tags.push_back(HTML::command);
	_tags.push_back(HTML::code);
	_tags.push_back(HTML::input);
	_tags.push_back(HTML::keygen);
	_tags.push_back(HTML::meta);
	_tags.push_back(HTML::noscript);
	_tags.push_back(HTML::object);
	_tags.push_back(HTML::script);
	_tags.push_back(HTML::var);
	_tags.push_back(HTML::video);
}

void Chtml::plain_text(std::vector<std::string> &_text)const{
	std::vector<HTML::TAGS> tags;
	get_ignore_html_tags(tags);
	contents(tags, _text);
}

void Chtml::contents(std::vector<TAGS>& _ignore_tags,
					std::vector<std::string> &_content)const{
	content_within(_ignore_tags, 0, _content);

}

void Chtml::content_within(std::vector<TAGS>& _ignore_tags,
						unsigned long _index,
						std::vector<std::string> &_content)const{
	auto max_size = m_text.size();
	auto next_tag_start = get_next_tag_start(_index);
	std::set<TAGS> igonre_tags;
	for (auto t : _ignore_tags)
		igonre_tags.insert(t);

	while (next_tag_start <max_size){
		auto next_tag = get_tag_at(next_tag_start);
		if (igonre_tags.find(next_tag) == igonre_tags.end()){
			//tag contents should be considered
			std::string content = "";
			get_contents_for_tag_at(next_tag_start, content);
			_content.push_back(content);
		}
		next_tag_start = get_next_tag_start(next_tag_start+1);


	}
}

unsigned long Chtml::get_next_tag_start(const unsigned long &_current_index)const{

	auto max_size = m_text.size();
	for (auto i = _current_index; i < max_size; ++i){
		if (m_text[i] == '<')
			return i;
	}
	return m_text.size();

}

unsigned long Chtml::get_next_tag_end(const unsigned long &_current_index)const{

	auto max_size = m_text.size();
	for (auto i = _current_index; i < max_size; ++i){
		if (m_text[i] == '>')
			return i;
	}
	return m_text.size();

}


TAGS Chtml::get_tag_at(const unsigned long &_index)const{

	if (m_text[_index] != '<')
		throw "No tag starts here!";
	auto max_size = m_text.size();

	std::string tag = "";
	auto index = _index + 1;
	bool tag_started = false;
	while (index<max_size){
		if (std::isspace(m_text[index])||m_text[index]=='>' || m_text[index] == '/'){
			if (tag_started)
				return get_tag(tag);
		}
		else{
			tag += m_text[index];
			tag_started = true;
		}
		index++;
	}
	return TAGS::total_tags;

}

TAGS Chtml::get_tag(std::string _tag)const{
	
	boost::trim(_tag);
	boost::to_lower(_tag);

	for (auto i = 0; i < TAGS::total_tags; i++){
		if (boost::iequals(_tag, tags_str[i]))
			return TAGS(i);
	}
	return TAGS::total_tags;
}

void Chtml::get_contents_for_tag_at(const unsigned long &_tag_start,
									std::string &_contents)const{
	auto start_content = get_next_tag_end(_tag_start);
	auto next_tag_start = get_next_tag_start(_tag_start + 1);
	start_content++;
	while (start_content < next_tag_start){
		_contents += m_text[start_content];
		start_content++;
	}
}
