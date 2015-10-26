#ifndef ACE_GUI_ELEMENT_MANAGER_H
#define ACE_GUI_ELEMENT_MANAGER_H
#include <OpenGL\ShadersUtility.h>
#include <BaseGUIObject.h>
#include <map>
#include <chrono>
namespace AceGUI{

	class GUIElementManager{

		GLuint m_programID;

		int m_window_width;

		int m_window_height;

		float m_scale;

		std::chrono::system_clock::time_point m_last_click_time;

		long long m_click_time_delay;

		int m_last_button_clicked;

		int m_last_action;

		unsigned int m_last_id;

		void load_shaders();

		std::map<unsigned int,BaseGUIElement* > m_gui_by_id;

		std::map<BaseGUIElement*,unsigned int> m_gui_by_objects;

		GUIElementManager();

		GUIElementManager(const GUIElementManager&) = delete;

		GUIElementManager& operator=(const GUIElementManager&) = delete;

	public:
		static GUIElementManager& Instance();

		unsigned int add(BaseGUIElement*);

		void enable_gui_gl_program();

		void on_mouse_click(mouse_click_callback_params &_params,
			const double &_x,
			const double &_y);

		void on_mouse_move(const double &_x,
			const double &_y);

		void remove(BaseGUIElement*);

		void remove(const unsigned int _id);

		BaseGUIElement* get_by_id(const unsigned int _id);

		void render();

		void clear();

		void set_window_size(const int _width,
			const int _height);

		int get_window_width()const;
		
		int get_window_height()const;

		void window_resized(const int _width,
			const int _height);

		//cleans up the gl buffers and states and program
		void clean_all();

		~GUIElementManager();
	};
}
#endif