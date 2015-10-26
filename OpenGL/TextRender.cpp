#include <TextRender.h>
#include <fstream>
#include <stdio.h>
#include <wchar.h>
#include <mutex>
#include <StringUtils.h>

using namespace AceGL;
TextRender::TextRender()
{
	mat4_set_identity(&m_projection);
	mat4_set_identity(&m_model);
	mat4_set_identity(&m_view);
	m_text_buffer = text_buffer_new(LCD_FILTERING_OFF);
}

void TextRender::on_window_resize(const int _viewPortWidth,
									const int _viewPortHeight){

	glUseProgram(m_text_buffer->shader);
	mat4_set_orthographic(&m_projection, 0, _viewPortWidth, 0, _viewPortHeight, -1, 1);
	glUniformMatrix4fv(glGetUniformLocation(m_text_buffer->shader, "model"),
		1, 0, m_model.data);
	glUniformMatrix4fv(glGetUniformLocation(m_text_buffer->shader, "view"),
		1, 0, m_view.data);
	glUniformMatrix4fv(glGetUniformLocation(m_text_buffer->shader, "projection"),
		1, 0, m_projection.data);
}
// ---------------------------------------------------------------- display ---
void TextRender::display()
{
	glUseProgram(m_text_buffer->shader);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	text_buffer_render(m_text_buffer);
}

/* ----------------------------------------------------------------- print - */
void TextRender::print(const Text &_text)
{
	glUseProgram(m_text_buffer->shader);
	text_buffer_clear(m_text_buffer);
	std::wstring text = StringUtils::get_wstring(_text.get_text());
	text_buffer_add_text(m_text_buffer, _text.get_pen(), _text.get_markup(), const_cast<wchar_t*>(text.c_str()), text.size());
}

void TextRender::append(const Text &_text){
	glUseProgram(m_text_buffer->shader);	
	text_buffer_add_text(m_text_buffer, _text.get_pen(), _text.get_markup(), const_cast<wchar_t*>(_text.get_textw().c_str()), _text.get_textw().size());
}

void TextRender::clear_text(){
	glUseProgram(m_text_buffer->shader);
	text_buffer_clear(m_text_buffer);
}

font_manager_t* TextRender::get_font_manager(){
	return m_text_buffer->manager;

}

TextRender::~TextRender()
{

}
