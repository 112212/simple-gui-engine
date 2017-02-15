#ifndef NG_COMBOBOX_HPP
#define NG_COMBOBOX_HPP

#include "../../Control.hpp"
#include "ScrollBar.hpp"
#include "TextBox.hpp"

#include <vector>
namespace ng {

#define KVADRAT_SIZE 20
#define INNER_X 5
#define INNER_Y 6
class ComboBox : public Control {
	private:
		Rect m_text_loc;
		
		bool m_is_mouseDown;
		bool m_is_onarrow;
		bool m_is_opened;
		bool m_drawscrollbar;
		bool m_scrollbar_focus;
		bool m_textbox_focus;
		bool m_is_textbox_mode;
		
		void onPositionChange();
		
		// items
		std::vector<std::string> m_items;

		Uint32 tex_sel;
		TTF_Font* m_font;
		
		struct TextLine{
			Uint32 tex;
			int w,h;
		};
		
		std::vector<TextLine> text_lines;
		
		Rect m_scrollrect;
		ScrollBar *m_scrollbar;
		TextBox *m_textbox;
		
		int m_max_dropdown_items;
		int m_dropdown_size;
		int m_last_scroll;
		int m_selection_color;
		
		int m_max_width;
		
		int m_selected_index;
		int m_virtual_selected_index;
		
		int m_scroll_pos;
		int m_font_height;
		
		unsigned int characterSize;
		
		bool isOnArrow( int mX, int mY );
		bool isOnScrollbar( int mX, int mY );
		bool isOnText( int mX, int mY );
		int getMaxText( std::string txt, int w );
		void openBox();
		void updateSelection();
		int getListOffset();
		void updateItemsSize();
		int getAverageHeight();
		std::string clipText( std::string s, int w );
		
		
		
	public:
		ComboBox();
		~ComboBox();
		
		void OnMouseDown( int mX, int mY, MouseButton which_button );
		void OnMouseUp( int mX, int mY, MouseButton which_button );
		//~ virtual void OnKeyDown( SDL_Keycode &sym, SDL_Keymod &mod, Uint16 &unicode );
		void OnLostFocus();
		void OnGetFocus();
		void OnMWheel( int updown );
		
		void OnLostControl();
		void OnMouseMove( int x, int y, bool mouseState );
		void Render( Point position, bool isSelected );
		void OnKeyDown( SDL_Keycode &sym, SDL_Keymod &mod );
		
		Control* Clone();
		void OnSetStyle(std::string& style, std::string& value);
		void Clear() { m_items.clear(); }
		void AddItem( const char* item );
		void AddItem( std::string item );
		void SetMaxWidth( int w );
		void SetMaxDropdown( int drp );
		int GetSelectedIndex();
		void SetSelectedIndex( int index );
		void SetTextEditableMode( bool editablemode );
		bool IsTextEditableMode() { return m_is_textbox_mode; }
		std::string GetText();
};
}
#endif