#ifndef ACE_GUI_TEXT_MANAGER_H
#define ACE_GUI_TEXT_MANAGER_H
#include <string>
#include <map>
#include <TextRender.h>
#include <CameraUtils\CameraView.h>
namespace Ace{
	
	class TextManager{
		TextManager();
		TextManager(const TextManager &)=delete;
		TextManager& operator=(const TextManager&)=delete;

		unsigned int m_last_id;
		std::map<unsigned int, AceGL::Text*> m_text;
		AceGL::TextRender *m_text_renderer;

	public:
		static TextManager& Instance();

		static TextManager& InstanceGUI();

		unsigned int AddText(const std::string &_str);

		font_manager_t * get_font_manager();

		// unmanaged text creation
		AceGL::Text *create_only_text(const std::string &_str);

		// unmanaged text creation
		AceGL::Text *create_only_text(const std::string &_str,
										const float &_x,
										const float &_y);


		unsigned int AddText(const std::string &_str, 
			const float &_x, 
			const float &_y);

		unsigned int AddText(const AceGL::Text &_t);

		unsigned int AddText(AceGL::Text *_t);

		bool update_text(const unsigned int _id,
			AceGL::Text &_new_text);

		bool update_text_location(const unsigned int _id,
			const float &_x,
			const float &_y);

		bool update_text_color(const unsigned int _id,
			const float &_r,
			const float &_g,
			const float &_b);

		bool update_text(const unsigned int _id,
			const std::string &_str);

		AceGL::Text * get_text(const unsigned int _id);

		void remove_text(const unsigned int _id);

		void remove_text(const float &_x, 
			const float &_y,
			bool _all);

		void scale_size(const unsigned int _id,
			const float &_factor);

		void set_size(const unsigned int _id,
			const float &_size);

		void render();

		void on_window_resize(const unsigned int _view_port_width,
			const unsigned int _view_port_height);

		void refresh();

		~TextManager();
	};
}


#endif