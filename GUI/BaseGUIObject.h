#ifndef ACE_BASE_GUI_OBJECT_H
#define ACE_BASE_GUI_OBJECT_H
#include <vector>
#define CREATE_CALLBACK(x) AceGUI::Callback<decltype(x)>::create_callback(x)
namespace AceGUI{

	class call_back_params{
	public:
		virtual ~call_back_params(){

		};
	};

	class mouse_click_callback_params :public call_back_params{

		int m_action;
		int m_button;
		int m_mod;
	public:
		mouse_click_callback_params(int _action, int _button, int _mod) :m_action(_action), m_mod(_mod), m_button(_button){

		};

		int action()const{
			return m_action;
		};

		int button()const{
			return m_button;
		}

		int mod()const {
			return m_mod;
		}
	};

	class button_click_params :public call_back_params{

		unsigned int m_id;
	public:
		button_click_params(unsigned int _id) :m_id(_id){

		};

		int id()const{
			return m_id;
		};
	};

	class Callback_base{
	public:
		virtual void exec(const call_back_params *_params) = 0;
		virtual ~Callback_base(){

		};
	};
	template<typename Callbackfn>
	class Callback :public Callback_base{
		Callbackfn m_fn;
	public:
		Callback(Callbackfn _fn) :m_fn(_fn){

		};
		void exec(const call_back_params *_params){
			m_fn(_params);
		}
		static Callback* create_callback(Callbackfn _fn){
			return new Callback<Callbackfn>(_fn);
		}
	};


	class BaseGUIElement{

		void exec_event_vector(std::vector<Callback_base*> &_events,
								mouse_click_callback_params &_params);

		void exec_event_vector(std::vector<Callback_base*> &_events,
			const call_back_params *_params);

		void clear_event_container(std::vector<Callback_base*> &_events);


		// delete copy and move constructors
		BaseGUIElement(const BaseGUIElement &) = delete;
		BaseGUIElement(BaseGUIElement &&) = delete;
		BaseGUIElement &operator=(const BaseGUIElement &) = delete;
		BaseGUIElement &operator=(BaseGUIElement &&) = delete;

		bool m_mouse_within_bounds;
		unsigned int m_id;

	protected :
		int m_window_width;
		int m_window_height;
		bool m_window_resized;

		std::vector<Callback_base*> m_mouse_left_click;
		std::vector<Callback_base*> m_mouse_right_click;
		std::vector<Callback_base*> m_mouse_middle_click;
		std::vector<Callback_base*> m_mouse_left_down;
		std::vector<Callback_base*> m_mouse_right_down;
		std::vector<Callback_base*> m_mouse_middle_down;
		std::vector<Callback_base*> m_mouse_left_up;
		std::vector<Callback_base*> m_mouse_right_up;
		std::vector<Callback_base*> m_mouse_middle_up;

		std::vector<Callback_base*> m_mouse_leave;
		std::vector<Callback_base*> m_mouse_enter;

		virtual void add_default_events() = 0;

	public:
		BaseGUIElement();

		virtual void window_sized(const int _width,
			const int _height);

		virtual void render() = 0;

		virtual bool is_point_within_bounds(const double &_x, const double &_y) = 0;

		virtual void animate() = 0;

		unsigned int get_id();

		//set the mouse events
		void set_on_mouse_left_click(Callback_base *_event);
		void set_on_mouse_right_click(Callback_base *_event);
		void set_on_mouse_middle_click(Callback_base *_event);

		void set_on_mouse_left_down(Callback_base *_event);
		void set_on_mouse_left_up(Callback_base *_event);

		void set_on_mouse_right_down(Callback_base *_event);
		void set_on_mouse_right_up(Callback_base *_event);

		void set_on_mouse_middle_down(Callback_base *_event);
		void set_on_mouse_middle_up(Callback_base *_event);

		void set_on_mouse_leave_event(Callback_base *_event);
		void set_on_mouse_enter_event(Callback_base *_event);
		
		// mouse event execute
		void mouse_left_click();
		void mouse_right_click();
		void mouse_middle_click();

		void mouse_left_down();
		void mouse_left_up();

		void mouse_right_down();
		void mouse_right_up();

		void mouse_middle_down();
		void mouse_middle_up();

		void mouse_enter();
		void mouse_leave();

		bool was_mouse_within_bounds();

		void set_mouse_within_bounds(bool _within_bounds);

		void gui_coordinate_to_gl_coordinate(const float &_gui_x,
			const float &_gui_y,
			float &_gl_x,
			float &_gl_y);

		void gl_coordinate_to_gui_coordinate(const float &_gl_x,
			const float &_gl_y,
			float &_gui_x,
			float &_gui_y);

		int get_window_width();

		int get_window_height();

		virtual ~BaseGUIElement();
	};
}
#endif