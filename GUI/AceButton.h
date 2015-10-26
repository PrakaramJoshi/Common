#ifndef ACE_BUTTON_H
#define ACE_BUTTON_H
#include <string>
#include <map>
#include <OpenGL\ShadersUtility.h>
#include <Geometry\DrawingUtils.h>
#include <BaseGUIObject.h>
namespace AceGUI{
	class Button :public BaseGUIElement{
		
		vao_state m_box_filled;
		DrawingUtils::DrawnRectangleFilled *m_box_filled_data;

		vao_state m_box_wire_frame;
		DrawingUtils::DrawnRectangleWireFrame *m_box_wire_frame_data;

		float m_size_x;
		float m_size_y;
		float m_center_x; 
		float m_center_y;

		float m_r_current;
		float m_g_current;
		float m_b_current;

		float m_r_default;
		float m_g_default;
		float m_b_default;

		float m_r_original;
		float m_g_original;
		float m_b_original;

		float m_min_size_x;

		unsigned int m_text_id;

		void update_button_box(const float & _text_size);

		void update_text_location();

		std::pair<float, float>get_text_location(const float &_text_size);

		float get_text_size();

		void add_default_events();

		void init_button(const std::string &_str,
			const float &_x,
			const float &_y);

		Button(const Button &) = delete;
		Button(Button &&) = delete;
		Button &operator=(const Button &) = delete;
		Button &operator=(Button &&) = delete;

	public:

		Button();

		Button(const std::string &_str,
			   const float &_x,
			   const float &_y);

		Button(const std::string &_str);

		~Button();

		enum BUTTON_EVENT{ HOVER,MOUSE_DOWN,MOUSE_CLICK};

		void create(const std::string &_text);

		void set_default_color(const float &_r,
			const float &_g,
			const float &_b);

		void get_default_color(float &_r,
			float &_g,
			float &_b);

		void set_current_color(const float &_r,
			const float &_g,
			const float &_b);

		void get_current_color(float &_r,
			float &_g,
			float &_b);

		void set_original_color(const float &_r,
			const float &_g,
			const float &_b);

		void get_original_color(float &_r,
			float &_g,
			float &_b);

		void set_text(const std::string &_str);

		void set_text_size(const float &_size);

		void set_text_color(const float &_r,
			const float &_g,
			const float &_b);

		void set_size(const float &_size_x,
			const float &_size_y);

		float get_size_x();

		void reset_min_size();
		
		float get_size_y();

		void set_center(const float &_center_x,
			const float &_center_y);

		void render();

		bool is_point_within_bounds(const double &_x,
			const double &_y);

		void set_min_size_x(const float &_x);

		float get_min_size_x();

		void animate() {};

	private:
		std::map<BUTTON_EVENT, void*> m_callbacks;
	};
}
#endif