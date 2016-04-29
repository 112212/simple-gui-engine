#ifndef NG_CANVAS_HPP
#define NG_CANVAS_HPP

#include "../../Control.hpp"

namespace ng {
enum {
	EVENT_CANVAS_CHANGE,
	
};

class Canvas : public Control {
	private:
		SDL_Surface* m_drawing;
		SDL_Texture* m_tex_drawing;
		bool m_is_mouseDown;
		bool m_is_readonly;
		void onPositionChange();
		int pixel_size;
		void put_pixel(int x, int y);
		void put_pixel_interpolate(int x, int y, int last_x, int last_y);
		int pixel_color;
		int last_x, last_y;
		bool align_to_grid;
		bool display_grid;
		bool maketex;
		
		void Render( SDL_Rect pos, bool isSelected );
		void OnMouseDown( int mX, int mY );
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseUp( int mX, int mY );
		void OnLostFocus();
	public:
		Canvas();
		~Canvas();
		
		void SetPixelSize(int size);
		void SetAlignToGrid(bool align) { align_to_grid = align; }
		void DisplayGrid( bool grid ) { display_grid = grid; }
		void SetPixelColor(int color) { pixel_color = color; }
		SDL_Surface* GetDrawingSurface() { return m_drawing; }
		void Clear(int color);
		void SetReadOnly( bool isReadOnly ) { m_is_readonly = isReadOnly; }

};

}
#endif