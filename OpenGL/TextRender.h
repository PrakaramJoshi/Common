#pragma once
#include <freetype-gl.h>
#include <font-manager.h>
#include <vertex-buffer.h>
#include <text-buffer.h>
#include <markup.h>
#include <shader.h>
#include <mat4.h>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <StringUtils.h>
namespace AceGL{
	class Text{
		std::string m_str;
		std::wstring m_wstr;
		std::string m_font;
		float m_r;
		float m_g;
		float m_b;

		float m_x;
		float m_y;

		float m_size_along_x;

		font_manager_t *m_font_manager;

		mutable markup_t m_markup;

		mutable vec2 m_pen;

		void init_markup(){

			vec4 black = { { 0.0, 0.0, 0.0, 1.0 } };
			vec4 fore_ground = { { m_r,m_g,m_b, 1.0 } };
			vec4 none = { { 1.0, 1.0, 1.0, 0.0 } };
			m_markup.family = const_cast<char*>(m_font.c_str());
			m_markup.size = 15;
			m_markup.bold = 0;
			m_markup.italic = 0;
			m_markup.rise = 0.0;
			m_markup.spacing = 0.0;
			m_markup.gamma = 1.0;
			m_markup.foreground_color = fore_ground;
			m_markup.background_color = none;
			m_markup.underline = 0;
			m_markup.underline_color = black;
			m_markup.overline = 0;
			m_markup.overline_color = black;
			m_markup.strikethrough = 0;
			m_markup.strikethrough_color = black;
			set_markup_font(font_manager_get_from_markup(m_font_manager, &m_markup));

		}

		Text& operator=(Text &&) = delete;
		Text(Text&&) = delete;

		void size_along_x_internal(){
			m_size_along_x = 0;
			
			for (auto i = 0; i < m_wstr.length(); i++){
				auto glyph = texture_font_get_glyph(m_markup.font, m_wstr[i]);
				m_size_along_x += glyph->advance_x * (1.0 + m_markup.spacing);
				float kerning = 0;
				if (i>0 && m_markup.font->kerning)
				{
					kerning = texture_glyph_get_kerning(glyph, m_wstr[i-1]);
				}
				m_size_along_x += kerning;
				
			}

		}

		Text() = delete;
	public:

		Text(font_manager_t *_font_manager){
			m_str = "";
			m_wstr = StringUtils::get_wstring(m_str);
			m_font = "fonts/VeraMono.ttf";
			m_r = 1.0;
			m_g = 1.0;
			m_b = 1.0;
			m_font_manager = _font_manager;
			init_markup();
			m_x = 0;
			m_y = 0;
			size_along_x_internal();
		}

		Text(font_manager_t *_font_manager,
			const std::string &_str){
			m_str = _str;
			m_wstr = StringUtils::get_wstring(m_str);
			m_font = "fonts/VeraMono.ttf";
			m_r = 1.0;
			m_g = 1.0;
			m_b = 1.0;
			m_font_manager = _font_manager;
			init_markup();
			m_x = 0;
			m_y = 0;
			size_along_x_internal();
		};

		Text(font_manager_t *_font_manager,
			const std::string &_str,
			const float &_x,
			const float &_y){
			m_str = _str;
			m_wstr = StringUtils::get_wstring(m_str);
			m_font = "fonts/VeraMono.ttf";
			m_r = 1.0;
			m_g = 1.0;
			m_b = 1.0;
			m_font_manager = _font_manager;
			init_markup();
			m_x = _x;
			m_y = _y;
			size_along_x_internal();
		}

		Text(font_manager_t *_font_manager, 
			const std::string &_str,
			const float &_x,
			const float &_y,
			const float &_r,
			const float &_g,
			const float &_b){
			m_font_manager = _font_manager;
			m_str = _str;
			m_wstr = StringUtils::get_wstring(m_str);
			m_font = "fonts/VeraMono.ttf";
			m_r = _r;
			m_g = _g;
			m_b = _b;
			init_markup();
			m_x = _x;
			m_y = _y;
			size_along_x_internal();
		}
		Text(const Text &_t){
			m_str = _t.m_str;
			m_wstr=_t.m_wstr;
			m_font = _t.m_font;
			m_r = _t.m_r;
			m_g = _t.m_g;
			m_b = _t.m_g;
			m_font_manager = _t.m_font_manager;
			init_markup();
			m_x = _t.m_x;
			m_y = _t.m_y;
			size_along_x_internal();
		}

		Text& operator=(const Text &_t){
			m_str = _t.m_str;
			m_wstr = _t.m_wstr;
			m_font = _t.m_font;
			m_r = _t.m_r;
			m_g = _t.m_g;
			m_b = _t.m_g;
			m_font_manager = _t.m_font_manager;
			init_markup();
			m_x = _t.m_x;
			m_y = _t.m_y;
			size_along_x_internal();
			return *this;
		}

		void set_color(const float &_r,
			const float &_g,
			const float &_b){
			m_r = _r;
			m_g = _g;
			m_b = _b; 
			init_markup();
		};

		void set_location(const float &_x,
			const float &_y){

			m_x = _x;
			m_y = _y;
		}

		void set_text(const std::string &_str){
			m_str = _str;
			m_wstr = StringUtils::get_wstring(m_str);
			size_along_x_internal();
		}

		void set_font(const std::string &_font_file){
			m_font = _font_file;
			init_markup();
			size_along_x_internal();
		}

		void scale_size(const float &_factor){
			m_markup.size = m_markup.size*_factor;
			size_along_x_internal();
		}

		void set_size(const float &_size){
			m_markup.size = _size;
			size_along_x_internal();
		}

		float get_x(){ return m_x; };

		float get_y(){ return m_y; };

		void set_markup_font(texture_font_t* _t){
			m_markup.font = _t;
			size_along_x_internal();
		}

		markup_t* get_markup()const{
			return &m_markup;
		}

		vec2 *get_pen()const{
			m_pen = { { m_x, m_y } };
			return &m_pen;
		}

		const std::string& get_text()const{
			return m_str;
		}

		const std::wstring& get_textw()const{
			return m_wstr;
		}

		float get_size_along_x()const{
			return m_size_along_x;
		}
		
		void static gui_coordinate_to_text_coordinate(
			const float &_window_height,
			const float &_gui_x,
			const float &_gui_y,
			float &_text_x,
			float &_text_y){

			_text_x = _gui_x;
			_text_y = _window_height - _gui_y;
		};

		void static text_coordinate_to_gui_coordinate(
			const float &_window_height,
			const float &_text_x,
			const float &_text_y,
			float &_gui_x,
			float &_gui_y){

			_gui_x=_text_x;
			_gui_y = _window_height - _text_y;
		};
		
	};
	class TextRender
	{
		text_buffer_t * m_text_buffer;
		mat4   m_model, m_view, m_projection;

	public:
		TextRender();

		void display();

		void clear_text();

		void print(const Text &_text);

		void append(const Text &_text);

		void on_window_resize(const int _viewPortWidth,
			const int _viewPortHeight);

		void test();

		font_manager_t* get_font_manager();

		~TextRender();
	};


}
