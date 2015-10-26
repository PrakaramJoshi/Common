#ifndef ACE_MENU_BAR_H
#define ACE_MENU_BAR_H
#include <map>
#include <BaseGUIObject.h>
namespace AceGUI{

	class Button;
	class Callback_base;
	class MenuBar :public BaseGUIElement{

		std::map<unsigned int ,Button*> m_buttons;

		float m_y;

		float m_x;

		bool m_vertical_bar;

		MenuBar(const MenuBar&) = delete;
		MenuBar(MenuBar&&) = delete;
		MenuBar& operator=(const MenuBar&) = delete;
		MenuBar& operator=( MenuBar&&) = delete;

		void add_default_events();

		float get_max_width()const;

		void get_next_x_y_horizontal_menu_bar(Button* _button,
			float &_x,
			float &_y);

		void get_next_x_y_vertical_menu_bar(Button* _button,
			float &_x,
			float &_y);

		void get_next_x_y(Button* _button,
			float &_x,
			float &_y);

		void set_buttons_to_max_width();

		public :
			
			MenuBar(bool _vertical_bar);

			unsigned int add_button(const std::string &_str);

			unsigned int add_button(const std::string &_str, Callback_base *_on_click);

			void set_on_click(unsigned int _id, Callback_base *_on_click);

			void window_sized(const int _width,
				const int _height);

			void remove_button(const unsigned int _id,bool _refresh=true);

			void render();

			bool is_point_within_bounds(const double &_x, const double &_y);

			void refresh();

			void set_to_minimum_size();

			void set_text(const unsigned int _id,
				const std::string &_str);

			void animate();

			bool toggleOrientation();

			~MenuBar();

	};
}

#endif