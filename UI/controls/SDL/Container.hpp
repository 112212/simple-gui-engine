#ifndef _H__CONTAINER
#define _H__CONTAINER

#include "../../Control.hpp"
#include "../../Widget.hpp"
#include "ScrollBar.hpp"


namespace ng {
class Container : public Widget {
	private:
		int m_grid_w, m_grid_h;
		bool m_is_mouseDown;
		int m_num_controls;
		int m_tx, m_ty;
		int max_v, max_h;
		bool overflow_v, overflow_h;
		Uint32 background_color;
		ScrollBar* m_scroll_v;
		ScrollBar* m_scroll_h;
		void onRectChange();
		void onOverflow();
		bool isOnVerticalScrollbar( int x, int y );
		bool isOnHorizontalScrollbar( int x, int y );
		inline void transformCoords( int &x, int &y );
		static int depth;
		Widget* innerWidget;
		
		void OnSetStyle(std::string& style, std::string& value);
	public:
		Container();
		~Container();
		
		void Render( Point position, bool isSelected );
		
		Container* Clone();
		void OnMWheel( int updown );
		void OnMouseDown( int x, int y, MouseButton which_button );
		void OnMouseUp( int x, int y, MouseButton which_button );
		void OnMouseMove( int x, int y, bool lmb );
		
		void AddItem( Control* control );
		void AddControl( Control* control );
};

}
#endif
