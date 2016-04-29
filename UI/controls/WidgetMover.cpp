#include "WidgetMover.hpp"
#include "../common/SDL/Drawing.hpp"

namespace ng {
WidgetMover::WidgetMover() {
	setType(TYPE_CONTROL);
}
void WidgetMover::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(mouseState) {
		getWidget()->SetPosition(last_pos.x + mX, last_pos.y + mY);
	}
}
void WidgetMover::OnMouseDown( int mX, int mY ) {
	if(!getWidget()) return;
	last_pos = getWidget()->GetRect();
}
void WidgetMover::OnMouseUp( int mX, int mY ) {
	last_pos = getWidget()->GetRect();
}

void WidgetMover::Render( SDL_Rect pos, bool isSelected ) {
	#ifdef SELECTION_MARK
		Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, isSelected ? Colors::Yellow : Colors::White );
	#else
		Drawing::Rect( m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, Colors::White );
	#endif
	
	
}
}