#include "TabContainer.hpp"
#include "TabContainerInc.hpp"

namespace ng {
TabContainer::TabContainer() {
	//~ tabrect.texture = tabtex;
	cur_container = new Container();
	selectedTab = -1;
	
	#ifdef USE_SFML
		esctex.loadFromImage( loadImageFromXPM3( esc_xpm ) );
		esc.setTexture( &esctex );
		esc.setSize( sf::Vector2f( 15, 15 ) );
		tabtex.loadFromImage( loadImageFromXPM3( test2_xpm ) );
		esctex.setSmooth(true);
		tabtex.setSmooth(true);
		font = Fonts::GetFont( "default" );
	#elif USE_SDL
		m_font = Fonts::GetFont( "default" );
	#endif
	setInterceptMask(mouse_down);
	characterSize = 13;
	
}

void TabContainer::OnLostControl() {
	
}


void TabContainer::AddItem( Control* control ) {
	cur_container->AddItem( control );
}

int TabContainer::getTabHeight() {
	return characterSize * 2;
}



#ifdef USE_SFML
// not portable
// should add tab with previously added controls to it
void TabContainer::AddTab( std::string tab_name ) {
	tabcontainers.push_back( cur_container );
	cur_container = new Container();
	sf::RectangleShape r;
	r.setSize( sf::Vector2f( getTextWidth( tab_name ) + 15, characterSize*2 ) );
	r.setTexture( &tabtex );
	tabrects.push_back( r );
	tabnames.push_back( tab_name );
	sf::Text text;
	text.setFont( font );
	text.setCharacterSize( characterSize );
	text.setStri( tab_name );
	tabtexts.push_back( text );
	onPositionChange();
}
// not portable
int TabContainer::getTextWidth( const std::string &txt ) {
	int len = txt.length();
	int sum=0;
	for(int i=0; i < len; i++) {
		const sf::Glyph &g = font.getGlyph( txt[i], characterSize, false );
		sum += g.advance;
	}
	return sum;
}
// not portable
void TabContainer::Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected ) {
	//~ esc.setPosition( m_rect.x, m_rect.y );
	//~ esc.setSize( sf::Vector2f( 160, 30 ) );
	//~ esc.setTexture( &tabtex );
	ren.draw( esc, state );
	for(int i=tabrects.size()-1; i > selectedTab; --i) {
		ren.draw( tabrects[i], state );
		ren.draw( tabtexts[i], state );
	}
	for(int i=0; i < selectedTab; i++) {
		ren.draw( tabrects[i], state );
		ren.draw( tabtexts[i], state );
	}
	if(selectedTab != -1) {
		ren.draw( tabrects[selectedTab], state );
		ren.draw( tabtexts[selectedTab], state );
		//~ tabcontainers[selectedTab]->Render( ren, state, isSelected );
	}
}


// not portable
const int tab_overlap = 15;
void TabContainer::onPositionChange() {
	int acc = esc.getSize().x;
	for(auto &i : tabrects) {
		i.setPosition( m_rect.x + acc, m_rect.y );
		acc += i.getSize().x - tab_overlap;
	}
	acc = esc.getSize().x;
	for(auto &i : tabtexts) {
		i.setPosition( m_rect.x + acc + 5, m_rect.y );
		acc += getTextWidth( i.getStri() ) + 15 - tab_overlap;
	}
	esc.setPosition( m_rect.x, m_rect.y );
}

// requires intercept
void TabContainer::OnMouseDown( int mX, int mY ) {
	const Point &pt = getAbsoluteOffset();
	mX = mX - pt.x + m_rect.x;
	mY = mY - pt.y + m_rect.y;
	
	if ( mY <= m_rect.y + getTabHeight() ) {
		if( selectedTab != -1 ) {
			const sf::Vector2f &r1 = tabrects[selectedTab].getPosition();
			const sf::Vector2f &v1 = tabrects[selectedTab].getSize();
			if( mX >= r1.x && mX <= r1.x + v1.x ) {
				return;
			}
		}
		for(int i=0; i < tabrects.size(); ++i) {
			const sf::Vector2f &r = tabrects[i].getPosition();
			const sf::Vector2f &v = tabrects[i].getSize();
			if( mX >= r.x && mX <= r.x + v.x ) {
				selectedTab = i;
				break;
			}
		}
	}
}

#elif USE_SDL

	// should add tab with previously added controls to it
	void TabContainer::AddTab( std::string tab_name ) {
		tabcontainers.push_back( cur_container );
		cur_container = new Container();
		// sf::RectangleShape r;
		// r.setSize( sf::Vector2f( getTextWidth( tab_name ) + 15, characterSize*2 ) );
		// r.setTexture( &tabtex );
		tabrects.push_back( r );
		tabnames.push_back( tab_name );
		// sf::Text text;
		// text.setFont( font );
		// text.setCharacterSize( characterSize );
		// text.setStri( tab_name );
		tabtexts.push_back( text );
		onPositionChange();
	}
	
	int TabContainer::getTextWidth( std::string txt ) {
		TTF_Font* fnt = m_font;
		int dummy, advance;
		int len = txt.length();
		int sum=0;
		for(int i=0; i < len; i++) {
			TTF_GlyphMetrics( fnt, txt[i], &dummy, &dummy, &dummy, &dummy, &advance);
			sum += advance;
		}
		return sum;
	}
	
	// not portable
	void TabContainer::Render( SDL_Renderer* ren, SDL_Rect pos, bool isSelected ) {
		//~ esc.setPosition( m_rect.x, m_rect.y );
		//~ esc.setSize( sf::Vector2f( 160, 30 ) );
		//~ esc.setTexture( &tabtex );
		// ren.draw( esc, state );
		// for(int i=tabrects.size()-1; i > selectedTab; --i) {
			// ren.draw( tabrects[i], state );
			// ren.draw( tabtexts[i], state );
		// }
		// for(int i=0; i < selectedTab; i++) {
			// ren.draw( tabrects[i], state );
			// ren.draw( tabtexts[i], state );
		// }
		// if(selectedTab != -1) {
			// ren.draw( tabrects[selectedTab], state );
			// ren.draw( tabtexts[selectedTab], state );
			// //~ tabcontainers[selectedTab]->Render( ren, state, isSelected );
		// }
	}


	// not portable
	const int tab_overlap = 15;
	void TabContainer::onPositionChange() {
		int acc = esc.getSize().x;
		for(auto &i : tabrects) {
			i.setPosition( m_rect.x + acc, m_rect.y );
			acc += i.getSize().x - tab_overlap;
		}
		acc = esc.getSize().x;
		for(auto &i : tabtexts) {
			i.setPosition( m_rect.x + acc + 5, m_rect.y );
			acc += getTextWidth( i.getStri() ) + 15 - tab_overlap;
		}
		esc.setPosition( m_rect.x, m_rect.y );
	}
	// not portable
	void TabContainer::OnMouseDown( int mX, int mY ) {
		if( mY >= m_rect.y && mY <= m_rect.y + getTabHeight() ) {
			
			if( selectedTab != -1 ) {
				const sf::Vector2f &r1 = tabrects[selectedTab].getPosition();
				const sf::Vector2f &v1 = tabrects[selectedTab].getSize();
				if( mX >= r1.x && mX <= r1.x + v1.x ) {
					return;
				}
			}
			for(int i=0; i < tabrects.size(); ++i) {
				const sf::Vector2f &r = tabrects[i].getPosition();
				const sf::Vector2f &v = tabrects[i].getSize();
				if( mX >= r.x && mX <= r.x + v.x ) {
					selectedTab = i;
					break;
				}
			}
		} else if( selectedTab != -1 ) {
			tabcontainers[ selectedTab ]->OnMouseDown( mX, mY );
		}
	}

#endif
}