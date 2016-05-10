#include "TextBox.hpp"
#include <cctype> // toupper
namespace ng {
TextBox::TextBox() {
	setType( TYPE_TEXTBOX );
	m_font = Fonts::GetFont( "default", 13 );
	m_text_selection.x = m_text_selection.y = 0;
	m_surf_first = 0;
	m_surf_middle = 0;
	m_surf_last = 0;
	m_text = "";
	m_last_sel = 0;
	m_cursor_pt = 0;
	m_cursor_sel = 0;
	m_first_index = 0;
	m_is_mouseDown = false;
	tex_first = tex_middle = tex_last = 0;
}

TextBox::~TextBox() {
	m_text = "";
}

void TextBox::Render( SDL_Rect pos, bool isSelected ) {
	
	
	#ifdef SELECTION_MARK
		Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, isSelected ? Colors::Yellow : Colors::White );
	#else
		Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, Colors::White );
	#endif
	
	
	
	
	// prvo mora da postoji prvi tekst
	if(m_surf_first) {
		// CSurface::OnDraw( ren, m_surf_first, m_text_loc.x+pos.x, m_text_loc.y+pos.y );
		Drawing::TexRect( m_text_loc.x+pos.x, m_text_loc.y+pos.y, m_surf_first->w, m_surf_first->h, tex_first );
		// ako postoji prvi tekst, da li postoji drugi tekst (selekcija)
		if(m_surf_middle) {
			// CSurface::OnDraw( ren, m_surf_middle, m_text_loc.x + m_surf_first->w+pos.x, m_text_loc.y+pos.y );
			Drawing::TexRect( m_text_loc.x + m_surf_first->w+pos.x, m_text_loc.y+pos.y, m_surf_middle->w, m_surf_middle->h, tex_middle );
			// ako postoji drugi tekst, da li postoji treci tekst (znaci da postoji selekcija negde izmedju)
			if(m_surf_last) {
				// CSurface::OnDraw( ren, m_surf_last, m_text_loc.x + m_surf_first->w + m_surf_middle->w + pos.x, m_text_loc.y + pos.y );
				Drawing::TexRect( m_text_loc.x + m_surf_first->w + m_surf_middle->w + pos.x, m_text_loc.y + pos.y, m_surf_last->w, m_surf_last->h, tex_last );
			}
		}
	}
	
	//Draw_HLine(surf, m_text_loc.x, m_rect.y+m_rect.h+10, getCharPos( m_maxtext ), CColors::c_yellow );
	
	if(isSelected) {
		//Draw_VLine(surf, m_cursor_pt, m_rect.y+5, m_rect.y+m_rect.h-5, CColors::c_white ); 
		Drawing::VLine(m_cursor_pt+pos.x, m_rect.y+pos.y+5, m_rect.y+pos.y+m_rect.h-5, Colors::White); 
		// vlineColor(ren, m_cursor_pt+pos.x, m_rect.y+5+pos.y, m_rect.y+m_rect.h-5+pos.y, Colors::White );
	}
}

void TextBox::SetText( std::string text ) {
	m_text = text;
	m_text_selection.x = m_text_selection.y = 0;
	
	// pocetak teksta ...
	m_text_loc.x = m_rect.x + 5;
	m_text_loc.y = m_rect.y + int(m_rect.h * 0.15);
	
	m_first_index = 0;
	m_cursor_sel = m_text.length();
	
	updateMaxText();
	updateCursor();
	updateSelection();
}

void TextBox::SetSelection( int start, int end ) {
	m_text_selection.x = std::max(start, 0);
	m_text_selection.y = std::min(end, (int)m_text.length() );
	
	updateSelection();
}

void TextBox::OnMouseDown( int mX, int mY ) {
	sendGuiCommand( GUI_KEYBOARD_LOCK );
	// std::cout << "mouse down " << mX << " " << mY << "\n";
	// generisati event za gui mozda ...
	// ili posetiti callback funkciju :)
	int sel = getSelectionPoint( mX );
	//cout << "first selection: " << sel << endl;
	m_last_sel = sel;
	m_is_mouseDown = true;
	m_cursor_sel = sel;
	updateCursor();
}

void TextBox::OnMouseUp( int mX, int mY ) {
	m_is_mouseDown = false;
	int sel = getSelectionPoint( mX );
	if(sel == m_last_sel) {
		m_text_selection.x = m_text_selection.y = 0;
		updateSelection();
	}
}

void TextBox::OnLostFocus() {
	m_is_mouseDown = false;
	// SDL_SetCursor( CCursors::defaultCursor );
}

void TextBox::OnLostControl() {
	m_text_selection.x = m_text_selection.y = 0;
	updateSelection();
}

void TextBox::updateSelection() {
	
	TTF_Font* my_font = m_font;
	
	if(m_surf_first)
		SDL_FreeSurface( m_surf_first );
		
	// izbaci ostale surfove
	if(m_surf_middle) {
		SDL_FreeSurface( m_surf_middle );
		m_surf_middle = 0;
	}
	if(m_surf_last) {
		SDL_FreeSurface( m_surf_last );
		m_surf_last = 0;
	}
	
	if(m_text_selection.x >= (int)m_text.length()) {
		m_text_selection.x = m_text_selection.y = 0;
	}
	
	int maxtext = m_maxtext;
	int len2;
	
	if(m_text_selection.x != m_text_selection.y) {
		if(m_text_selection.x <= m_first_index) {
			len2 = std::min( m_text_selection.y-m_text_selection.x+1, maxtext );
			maxtext -= len2;
			
			
			// m_surf_first = TTF_RenderText_Shaded( my_font, m_text.substr( m_first_index, len2 ).c_str(), {255,255,255}, {100,100,100} );
			m_surf_first = TTF_RenderText_Blended( my_font, m_text.substr( m_first_index, len2 ).c_str(), {255,255,255} );
			tex_first = Drawing::GetTextureFromSurface(m_surf_first, tex_first);
			// tex_text = Drawing::GetTextureFromSurface(m_surf_first);
			
			
			if(maxtext > 0) {
				if(m_text_selection.y+1 < m_text.length()) {
					// m_surf_middle = TTF_RenderText_Solid( my_font, m_text.substr( m_text_selection.y+1, maxtext ).c_str(), {255,255,255} );
					m_surf_middle = TTF_RenderText_Blended( my_font, m_text.substr( m_text_selection.y+1, maxtext ).c_str(), {255,255,255} );
					tex_middle = Drawing::GetTextureFromSurface(m_surf_middle, tex_middle);
				}
			}
		} else {
			//  if(m_text_selection.x < m_first_index + m_maxtext)
			len2 = std::min( maxtext, m_text_selection.x-m_first_index );
			maxtext -= len2;
			// m_surf_first = TTF_RenderText_Blended( my_font, m_text.substr(m_first_index, len2).c_str(), {255,255,255} );
			m_surf_first = TTF_RenderText_Blended( my_font, m_text.substr(m_first_index, len2).c_str(), {255,255,255} );
			tex_first = Drawing::GetTextureFromSurface(m_surf_first, tex_first);
			
			
			if(maxtext > 0) {
				len2 =  std::min(m_text_selection.y-m_text_selection.x+1, maxtext);
				maxtext -= len2;

				// m_surf_middle = TTF_RenderText_Shaded( my_font, m_text.substr( m_text_selection.x, len2 ).c_str(), {255,255,255}, {100,100,100});
				m_surf_middle = TTF_RenderText_Blended( my_font, m_text.substr( m_text_selection.x, len2 ).c_str(), {255,255,255});
				tex_middle = Drawing::GetTextureFromSurface(m_surf_middle, tex_middle);
				
			}
			
			if(m_text_selection.y+1 < m_text.length()) {
				if(maxtext > 0) {
					// m_surf_last = TTF_RenderText_Solid( my_font, m_text.substr( m_text_selection.y+1, maxtext ).c_str(), {255,255,255} );
					m_surf_last = TTF_RenderText_Blended( my_font, m_text.substr( m_text_selection.y+1, maxtext ).c_str(), {255,255,255} );
					tex_last = Drawing::GetTextureFromSurface(m_surf_last, tex_last);
				}
			}
		}
			
	} else {

		if(m_text.size() > 0) {
			m_surf_first = TTF_RenderText_Blended( my_font, m_text.substr(m_first_index, maxtext).c_str(), {255,255,255} );
			tex_first = Drawing::GetTextureFromSurface(m_surf_first, tex_first);
		}
	}
	
}

void TextBox::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetText(value);
	}
		
}

int TextBox::getSelectionPoint( int &mX ) {
	TTF_Font* fnt = m_font;
	
	int dummy, advance;
	if(mX < m_text_loc.x)
		return 0;
	else if( mX > m_rect.x+m_rect.w )
		return m_text.length();
	int sum=0;
	for(int i=m_first_index; i < m_text.length(); i++) {
		TTF_GlyphMetrics( fnt, m_text[i],&dummy,&dummy,&dummy,&dummy,&advance);
		if( m_text_loc.x+sum >= mX )
			return i;
		sum += advance;
	}
	return m_text.length();
}

void TextBox::OnGetFocus() {
	// SDL_SetCursor( CCursors::textCursor );
	
}

void TextBox::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(m_is_mouseDown) {
		int sel = getSelectionPoint( mX );
		m_text_selection.x = m_last_sel;
		m_text_selection.y = sel;
		fixSelection();
		updateSelection();
		m_cursor_sel = sel;
		updateCursor();
	}
}

void TextBox::fixSelection() {
	if( m_text_selection.y < m_text_selection.x ) {
		int ex;
		// zamena
		ex = m_text_selection.x;
		m_text_selection.x = m_text_selection.y;
		m_text_selection.y = ex;
	}
}

void TextBox::onPositionChange() {
	SetText( m_text.c_str() );
}


void TextBox::OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod ) {
	int val = sym;
	
	// if(val >= 256 && val <= 265) val -= 208;
	
	// cout << val << endl;
	switch(val) {
		case SDLK_BACKSPACE:
			if(m_text.length() > 0) {
				if(m_text_selection.x != m_text_selection.y) {
					if(m_text_selection.x >= 0) {
						m_text.erase(m_text_selection.x, m_text_selection.y-m_text_selection.x+1);
						m_cursor_sel = m_text_selection.x;
						// remove selection
						m_text_selection.x = m_text_selection.y = 0;
					}
				} else if(m_cursor_sel > 0) {
					m_text = m_text.erase(m_cursor_sel-1, 1);
					m_cursor_sel--;
					int mtext = Fonts::getMaxTextBw( m_font, m_text.substr(0,m_cursor_sel), m_rect.w );
					if(mtext != m_maxtext) {
						m_maxtext = mtext;
						m_first_index = m_cursor_sel - mtext;
					}
				}
				updateCursor();
				emitEvent( event::textbox_change );
			}
			break;
		case SDLK_LEFT:
			if(m_cursor_sel >= 0) {
				m_cursor_sel--;
				updateCursor();
			}
			break;
		case SDLK_RIGHT:
			if(m_cursor_sel < m_text.length()) {
				m_cursor_sel++;
				updateCursor();
			}
			break;
		case SDLK_END:
			m_cursor_sel = m_text.length();
			updateCursor();
			break;
		case SDLK_HOME:
			setFirstIndex(0);
			m_cursor_sel = 0;
			updateCursor();
			break;
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			emitEvent( event::textbox_enter );
			break;
			
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
		case SDLK_LCTRL:
		case SDLK_RCTRL:
		case SDLK_LALT:
		case SDLK_RALT:
		
			break;
			
		default:
		
			if(mod & KMOD_CTRL) {
				switch(val) {
					case 'c': // copy
						cout << "Ctrl-c\n";
						break;
					case 'v': // paste
						cout << "Ctrl-v\n";
						break;
					case 'a': // select all
						cout << "Ctrl-a\n";
						break;
				}
				break;
			}
			
			if(val == SDLK_KP_0) {
				val = '0';
			} else if(val >= SDLK_KP_1 && val <= SDLK_KP_9) {
				val = val - SDLK_KP_1 + 0x31;
			} else if(val == SDLK_KP_DIVIDE) {
				val = '/';
			}
		
			if(val == SDLK_KP_PERIOD) 
				val = SDLK_PERIOD;
				
			if(mod & KMOD_SHIFT) {
				val = toupper(val);
			}
			
			
			if(m_text_selection.x != m_text_selection.y) {
				m_text.erase(m_text_selection.x, m_text_selection.y-m_text_selection.x+1);
				m_cursor_sel = m_text_selection.x;
				m_text_selection.x = m_text_selection.y = 0;
			}
			
			m_text.insert(m_cursor_sel, 1, val);
			
			// pomeriti cursor
			m_cursor_sel++;
			updateCursor();
			
			emitEvent( event::textbox_change );
			break;
	}
	updateSelection();
}

int TextBox::getCharPos( int num ) {
	int sum=m_text_loc.x;
	
	
	// TODO: replace this with small version
	
	int dummy, advance;
	int len = m_first_index+num;
	for(int i=m_first_index; i < len; i++) {
		TTF_GlyphMetrics( m_font, m_text[i], &dummy, &dummy, &dummy, &dummy, &advance);
		sum += advance;
	}
	return sum;
}

void TextBox::updateCursor() {

	if(m_cursor_sel < m_first_index) {
		if(m_cursor_sel < 0)m_cursor_sel = 0;
		m_first_index=m_cursor_sel;
		m_maxtext = Fonts::getMaxText(m_font, m_text.substr(m_first_index), m_rect.w);
	} else if(m_cursor_sel-m_first_index > m_maxtext) {
		m_maxtext = Fonts::getMaxTextBw( m_font, m_text.substr(0,m_cursor_sel), m_rect.w );
		m_first_index = m_cursor_sel - m_maxtext;
	} else
		m_maxtext = Fonts::getMaxText(m_font, m_text.substr(m_first_index), m_rect.w);
	m_cursor_pt = getCharPos( m_cursor_sel-m_first_index );
}


void TextBox::updateMaxText() {
	m_maxtext = Fonts::getMaxText(m_font, m_text.substr(m_first_index), m_rect.w);
}

void TextBox::setFirstIndex( int index ) {
	m_first_index = index;
	m_maxtext = Fonts::getMaxText(m_font, m_text.substr(m_first_index), m_rect.w);
}

const char* TextBox::GetText( ) {
	return m_text.c_str();
}

const char* TextBox::GetSelectedText() {
	return m_text.substr( m_text_selection.x, m_text_selection.y-m_text_selection.x+1 ).c_str();
}
}
