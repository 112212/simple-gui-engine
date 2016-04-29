#include "Gui.hpp"
#include "Widget.hpp"

#ifdef USE_SDL
#include "common/SDL/Drawing.hpp"
#endif

#define DEFAULT_FONT "/usr/share/fonts/TTF/DroidSans.ttf"

#define MAX_BASIC_EVENTS 6
namespace ng {
	

GuiEngine::GuiEngine() {
	#ifdef USE_SFML
		if( !Fonts::FontExists( "default" ) ) {
			Fonts::LoadFont( DEFAULT_FONT, "default" );
			Colors::InitColors();
		}
	#elif USE_SDL
		if( !Fonts::FontExists( "default" , 13 ) ) {
			if(!Fonts::LoadFont( DEFAULT_FONT, "default", 13 )) {
				cout << "[GUI] default font loadi failed\n";
			}
			Colors::InitColors();
		}
	#endif
	m_mouse_down = false;
	m_focus = false;
	m_keyboard_lock = false;
	m_focus_lock = false;
	m_widget_lock = false;
	m_lock_once = false;
	
	depth = 0;
	selected_control = 0;
	sel_first_depth_widget = 0;
	last_selected_widget = 0;
	
	hasIntercepted = false;
	sel_intercept = 0;
	sel_intercept_vector.resize(15);
	
}

void GuiEngine::LockWidget(Widget* widget) {
	if(!widget or widget->engine != this) return;
	sel_first_depth_widget = widget;
	Point ofs = {0,0};
	Widget *w;
	if(selected_control) {
		w = selected_control->getWidget();
		while(w && w != widget) {
			w = w->getWidget();
		}
		if(w != widget) {
			unselectControl();
			unselectWidgets();
		} else {
			int d = depth;
			w = last_selected_widget;
			while(w != widget) {
				d--;
				w = w->widget;
			}
			int dff = depth - d;
			for(int i=0; i < dff; i++) {
				sel_intercept_vector[i] = sel_intercept_vector[i+d-1];
			}
			sel_intercept_vector[dff].widget = 0;
			depth = dff;
		}
	} else {
		unselectWidgets();
		last_selected_widget = widget;
		
		sel_intercept_vector[0].intercept_mask = widget->intercept_mask;
		sel_intercept_vector[0].widget = widget;
		sel_intercept_vector[1].widget = 0;
		sel_intercept = widget->intercept_mask;
		depth = 1;
	}
	
	w = sel_first_depth_widget;
	while(w) {
		ofs = {ofs.x + w->m_rect.x, ofs.y + w->m_rect.y};
		w = w->widget;
	}
	sel_widget_offset = ofs;
	m_widget_lock = true;
}

void GuiEngine::UnlockWidget() {
	if(!m_widget_lock) return;
	
	// get real depth
	int d = -1;
	Widget *w = sel_first_depth_widget;
	while(w) {
		w = w->widget;
		d++;
	}
	// fix intercept vector
	// shift intercept vector
	for(int i=depth; i >= 0; i--) {
		sel_intercept_vector[i+d] = sel_intercept_vector[i];
	}
	// add intercepts from engine to widget
	w = sel_first_depth_widget;
	w = w->widget;
	depth += d;
	while(w) {
		d--;
		sel_intercept_vector[d].widget = w;
		sel_intercept_vector[d].intercept_mask = w->intercept_mask;
		sel_intercept |= w->intercept_mask;
		if(d == 0) {
			sel_first_depth_widget = w;
		}
		w = w->widget;
	}

	assert(d == 0);

	m_widget_lock = false;
}

Control* GuiEngine::GetControlByName(std::string name) {
	auto it = map_name_control.find(name);
	if(it != map_name_control.end())
		return it->second;
	else
		return 0;
}
Control* GuiEngine::GetControlById(unsigned int id) {
	auto it = map_id_control.find(id);
	if(it != map_id_control.end())
		return it->second;
	else
		return 0;
}


void GuiEngine::processControlEvent(int event_type) {

	
	switch(event_type) {
		case GUI_KEYBOARD_LOCK:
			m_keyboard_lock = true;
			break;
		case GUI_FOCUS_LOCK:
			m_focus_lock = true;
			break;
		case GUI_UNLOCK:
			m_focus_lock = false;
			m_keyboard_lock = false;
			break;
		case GUI_LOCK_ONCE:
			m_lock_once = true;
			break;
		case GUI_UNSELECT:
			unselectControl();
			return;
		case GUI_WIDGET_UNLOCK: {
				m_widget_lock = false;
				if(selected_control) {
					Widget* w = static_cast<Widget*>(selected_control);
					while(w && w->widget) {
						w = w->widget;
					}
					if(w) sel_first_depth_widget = w;
				}
			}
		case GUI_UNSELECT_WIDGET:
			unselectWidget();
			break;
	}
}


	  
void GuiEngine::AddControl( Control* control ) {
	if(control->engine) return;
	
	if(control->isWidget) {
		Widget* w = static_cast<Widget*>(control);

		recursiveProcessWidgetControls(w, true);
		
		w->set_engine(this);
	} else {
		control->engine = this;
	}
	
	addControlToCache(control);
	
	map_name_control[control->name] = control;
	map_id_control[control->id] = control;
}

void GuiEngine::RemoveControl( Control* control ) {
	if(control->engine != this) return;
	
	if(selected_control == control) {
		unselectControl();
	}
	
	std::string name = control->name;
	unsigned int id = control->id;
	
	map_name_control.erase(name);
	map_id_control.erase(id);
	
	// if its widget, make sure nothi breaks
	if(control->isWidget) {
		Widget* widget = (Widget*)control;
		
		recursiveProcessWidgetControls(widget, false);
		
		if(sel_first_depth_widget == widget) {
			unselectWidgets();
			sel_first_depth_widget = 0;
			m_widget_lock = false;
		}
		// if any of selected controls or widgets are inside this widget, we must break selection
		if(selected_control) {
			Widget* w = selected_control->widget;
			while(w && w != widget) {
				w = w->widget;
			}
			if(w == widget) {
				unselectControl();
			}
		}
		
		if(last_selected_widget) {
			Widget* w = last_selected_widget;
			while(w && w != widget) {
				w = w->widget;
			}
			if(w == widget) {
				unselectWidgets();
			}
		}
	}
	
	if(!control->widget)
		removeControlFromCache(control);
	else
		control->widget->removeControlFromCache(control);
	control->engine = 0;
	control->widget = 0;
}

#define INTERCEPT_HOOK(action_enum,action) {						\
	if((sel_intercept & Widget::imask::action_enum) != 0) {  	\
		for(int i=0; !hasIntercepted; i++) {                        \
			interceptInfo &v = sel_intercept_vector[i];             \
			if(!v.widget) break;                                    \
			if((v.intercept_mask                                    \
				& Widget::imask::action_enum) != 0) {          \
				v.widget->action;                                   \
			}                                                       \
		}                                                           \
	} 																\
	if(hasIntercepted)												\
		hasIntercepted = false; 									\
	else 															\
		selected_control->action;										\
	}
	
#define WIDGET_HOOK(action_enum,action) if(last_selected_widget) { 	\
	if((sel_intercept & Widget::imask::action_enum) != 0) {		\
		/*sel_first_depth_widget->action;*/								\
		for(int i=0; !hasIntercepted; i++) {                        	\
			interceptInfo &v = sel_intercept_vector[i];             	\
			if(!v.widget) break;                                    	\
			if((v.intercept_mask                                    	\
				& Widget::imask::action_enum) != 0) {          	\
				v.widget->action;                                   	\
			}                                                       	\
		}    															\
	} 																	\
	if(hasIntercepted)													\
		hasIntercepted = false; 										\
}

void GuiEngine::OnMouseDown( int mX, int mY ) {
	m_mouse_down = true;
	Point control_coords{mX-sel_widget_offset.x, mY-sel_widget_offset.y};
	
	#ifndef OVERLAPPING_CHECK
	if( !selected_control ) {
		check_for_new_collision( mX, mY );
	}
	#else
	if(!m_focus_lock) {
		check_for_new_collision( mX, mY );
	}
	#endif
	
	if( selected_control ) {
		if(m_focus_lock) {
			INTERCEPT_HOOK(mouse_down, OnMouseDown( control_coords.x, control_coords.y ));
			// selected_control->OnMouseDown( control_coords.x, control_coords.y );
			if(m_focus_lock)
				return;
			if(m_lock_once) {
				m_lock_once = false;
				return;
			}
		}

		if( checkngControl_collision(selected_control, control_coords.x, control_coords.y) ) {
			// selected_control->OnMouseDown( control_coords.x, control_coords.y );
			INTERCEPT_HOOK(mouse_down, OnMouseDown( control_coords.x, control_coords.y ));
		} else {
			unselectControl();
			check_for_new_collision( mX, mY );
			if(selected_control) {
				selected_control->OnGetFocus();
				// selected_control->OnMouseDown( control_coords.x, control_coords.y );
				INTERCEPT_HOOK(mouse_down, OnMouseDown( mX-sel_widget_offset.x, mY-sel_widget_offset.y ));
			}
		}
	} else WIDGET_HOOK(mouse_down, OnMouseDown( mX, mY ));
}


void GuiEngine::OnMouseUp( int mX, int mY ) {
	m_mouse_down = false;
	Point control_coords{mX-sel_widget_offset.x, mY-sel_widget_offset.y};
	if( selected_control )
		// selected_control->OnMouseUp( control_coords.x, control_coords.y );
		INTERCEPT_HOOK(mouse_up, OnMouseUp( control_coords.x, control_coords.y ));
}

void GuiEngine::OnMouseMove( int mX, int mY ) {
	Point control_coords{mX-sel_widget_offset.x, mY-sel_widget_offset.y};
	if(selected_control) {
		if( m_mouse_down || m_focus_lock ) {
			// selected_control->OnMouseMove( control_coords.x, control_coords.y, m_mouse_down );
			INTERCEPT_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ));
			return;
		}
		
		#ifdef OVERLAPPING_CHECK
		if(!m_keyboard_lock) {
			Control *lastngControl = selected_control;
			check_for_new_collision( mX, mY );
			if(selected_control) {				
				if(lastngControl != selected_control) {
					INTERCEPT_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ));
					return;
				}
			} else {
				WIDGET_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ));
				return;
			}
		}
		#endif
		
		if(selected_control->custom_check ? 
			selected_control->customBoundary(control_coords.x, control_coords.y) : 
			checkngControl_collision(selected_control, control_coords.x, control_coords.y))
		{
			// selected_control->OnMouseMove( control_coords.x, control_coords.y, m_mouse_down );
			INTERCEPT_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ));
			if(!m_focus) {
				selected_control->OnGetFocus();
				m_focus = true;
			}
		} else {
			selected_control->OnLostFocus();
			m_focus = false;
			if(!(m_focus_lock || m_keyboard_lock)) {
				unselectControl();
			}
		}
	} else {
		WIDGET_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ));
		check_for_new_collision( mX, mY );
	}
}



void GuiEngine::OnMWheel( int updown ) {
	if(selected_control) {
		INTERCEPT_HOOK(mwheel, OnMWheel( updown ));
		// selected_control->OnMWheel( updown );
	} else WIDGET_HOOK(mwheel, OnMWheel( updown ));
}


bool GuiEngine::checkngControl_collision( Control* c, int mX, int mY ) {
	Rect r = c->m_rect;
	if( mX > r.x && mX < r.x + r.w ) {
		if( mY > r.y && mY < r.y + r.h ) {
			return true;
		}
	}
	return false;
}
void GuiEngine::check_for_new_collision( int x, int y ) {
	
	Control* lastngControl = selected_control;
	Point offset;
	Point &o = offset;
	Widget *p = 0;
	if(m_widget_lock || last_selected_widget) {
		offset = sel_widget_offset;
		if(m_widget_lock)
			p = sel_first_depth_widget;
	}
	
	if(last_selected_widget) {
		p = last_selected_widget;
		if(m_widget_lock) {
			Rect r = p->m_rect;
			while(p != sel_first_depth_widget and !(x >= o.x && x <= o.x + r.w && 
				y >= o.y && y <= o.y + r.h)) {
				p->selected_control = 0;
				const Point &o2 = p->offset;
				o = {o.x - r.x - o2.x, o.y - r.y - o2.y};
				p = p->widget;
				r = p->m_rect;
				depth--;
			}
			if(p == sel_first_depth_widget and !(x >= o.x && x <= o.x + r.w && 
				y >= o.y && y <= o.y + r.h)) {
				// mouse not in locked widget
				return;
			}
		} else {
			Rect r = p->m_rect;
			while(p && !(x >= o.x && x <= o.x + r.w && 
				y >= o.y && y <= o.y + r.h)) {
				p->selected_control = 0;
				const Point &o2 = p->offset;
				o = {o.x - r.x - o2.x, o.y - r.y - o2.y};
				p = p->widget;
				if(p) {
					r = p->m_rect;
				}
				depth--;
			}
		}
	}
	
	if(p != last_selected_widget) {
		unselectControl();
	}
	
	std::vector<cache_entry>::reverse_iterator it, it_end;
	
	if(p) {
		it = p->cache.rbegin();
		it_end = p->cache.rend();
	} else {
		it = cache.rbegin();
		it_end = cache.rend();
		depth = 0;
		o = {0,0};
	}
	
	Widget* last_widget = p;
	last_selected_widget = p;
	sel_widget_offset = o;
	// --- big iterations ---
	while(it != it_end) {
		if(!it->interactible) { it++; continue; }
		bool in = false;
		if(it->custom_check) {
			if(it->control->customBoundary( x-o.x, y-o.y )) {
				in = true;
			}
		} else {
			Rect& r = it->rect;
			if(x >= r.x + o.x && x <= r.x + o.x + r.w &&
			   y >= r.y + o.y && y <= r.y + o.y + r.h)
			   in = true;
		}
		if(in) {
			if(it->isWidget) {
				Widget* w = static_cast<Widget*>(it->control);
				if(depth == 0 && !m_widget_lock) {
					sel_first_depth_widget = w;
				} else if(last_widget) {
					last_widget->selected_control = w;
				}
				last_widget = w;
				
				// per widget
				sel_intercept_vector[depth].intercept_mask = w->intercept_mask;
				sel_intercept_vector[depth].widget = w;
				
				w->cached_absolute_offset.x = offset.x + it->rect.x;
				w->cached_absolute_offset.y = offset.y + it->rect.y;
				offset.x += it->rect.x + w->offset.x;
				offset.y += it->rect.y + w->offset.y;
				
				it = w->cache.rbegin();
				it_end = w->cache.rend();
			} else {
				selected_control = it->control;
				if(last_widget) {
					last_widget->selected_control = selected_control;
				}
				break;
			}
			depth++;
		} else {
			it++;
		}
	}
	
	if(last_widget) {
		last_selected_widget = last_widget;
		sel_widget_offset = offset;
	}
	
	// if(last_selected_widget)
	// cout << "widget: " << last_selected_widget->id << ", " << " depth: " << depth << ", selected_control: " << (selected_control != 0) << endl;
	// else cout << "depth: " << depth << endl;
	
	sel_intercept_vector[depth].widget = 0;
	
	// global intercept flag
	sel_intercept = 0;
	for(int i=0; i < depth; i++)
		sel_intercept |= sel_intercept_vector[i].intercept_mask;
		
	if(lastngControl != selected_control) {
		if(lastngControl)
			lastngControl->OnLostControl();
		if(selected_control) {
			selected_control->OnGetFocus();
			m_focus = true;
			m_keyboard_lock = false;
		}
	}
}

void GuiEngine::recursiveProcessWidgetControls(Widget* wgt, bool add_or_remove) {
	Widget* w = wgt;
	for(auto it = w->cache.begin(); it != w->cache.end(); it++) {
		if(it->isWidget) {
			recursiveProcessWidgetControls((Widget*)it->control, add_or_remove);
		} else {
			if(add_or_remove) {
				map_id_control[it->control->id] = it->control;
				map_name_control[it->control->name] = it->control;
			} else {
				map_id_control.erase(it->control->id);
				map_name_control.erase(it->control->name);
			}
		}
	}
}

void GuiEngine::unselectWidget() {
	if(last_selected_widget) {
		if(selected_control)
			unselectControl();
		Widget* w = last_selected_widget;
		// w->selected_control = 0;
		w = w->widget;
		last_selected_widget = w;
		depth--;
		cout << "depth: " << depth << endl;
	}
}
void GuiEngine::unselectWidgets() {
	if(selected_control)
		unselectControl();
	if(last_selected_widget) {
		Widget* w = last_selected_widget;
		while(w) {
			w->selected_control = 0;
			w = w->widget;
		}
		last_selected_widget = 0;
		depth = 0;
	}
}

void GuiEngine::unselectControl() {
	if(!selected_control) return;
	selected_control->OnLostControl();
	m_focus_lock = false;
	m_keyboard_lock = false;
	if(selected_control->widget) {
		selected_control->widget->selected_control = 0;
	}
	selected_control = 0;
}

#ifdef USE_EVENT_QUEUE
	bool GuiEngine::HasEvents( ) {
		return !m_events.empty();
	}

	Event GuiEngine::PopEvent() {
		Event evt = m_events.front();
		m_events.pop();
		return evt;
	}
#endif


void GuiEngine::SubscribeEvent( int id, int event_type, std::function<void(Control*)> callback ) {
	// subscribers.insert(std::make_pair(std::make_pair(id,event_type), callback));
	auto it = map_id_control.find(id);
	if(it != map_id_control.end()) {
		it->second->SubscribeEvent(event_type, callback);
	}
}


void GuiEngine::OnCleanup() {
}

#ifdef USE_SFML
	void GuiEngine::draw(sf::RenderTarget& target, sf::RenderStates states) const {

		bool has_selected_control = false;
		for(auto &ca : cache) {
			Control* const &c = ca.control;
			if(c != selected_control) {
				c->Render( target, states, false );
			} else {
				has_selected_control = true;
			}
		}
		
		if(has_selected_control) {
			selected_control->Render(target,states,true);
		}
	}
	void GuiEngine::OnEvent(sf::Event &event) {
		switch(event.type) {
			case sf::Event::KeyPressed:
				OnKeyDown( event.key );
				break;
			case sf::Event::KeyReleased:
				OnKeyUp( event.key );
				break;
			case sf::Event::MouseButtonPressed:
				if(event.mouseButton.button == sf::Mouse::Button::Left) {
					OnMouseDown( event.mouseButton.x, event.mouseButton.y );
				}
				break;
			case sf::Event::MouseButtonReleased:
				if(event.mouseButton.button == sf::Mouse::Button::Left) {
					OnMouseUp( event.mouseButton.x, event.mouseButton.y );
				}
				break;
			case sf::Event::MouseMoved:
				OnMouseMove( event.mouseMove.x, event.mouseMove.y );
				break;
			case sf::Event::MouseWheelScrolled:
				int delta = event.mouseWheelScroll.delta;
				OnMWheel( delta );
				break;
		}
	}
	void GuiEngine::OnKeyDown( sf::Event::KeyEvent &sym ) {
		if(selected_control) {
			INTERCEPT_HOOK(key_down, OnKeyDown( sym ));
			// selected_control->OnKeyDown( sym );
		}
	}

	void GuiEngine::OnKeyUp( sf::Event::KeyEvent &sym ) {
		if(selected_control) {
			INTERCEPT_HOOK(key_up, OnKeyUp( sym ));
			// selected_control->OnKeyUp( sym );
		}
	}
#elif USE_SDL
	void GuiEngine::OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod) {
		if(selected_control) {
			INTERCEPT_HOOK(key_down, OnKeyDown( sym, mod ));
			// selected_control->OnKeyDown( sym, mod );
		}
	}

	void GuiEngine::OnKeyUp(  SDL_Keycode &sym, SDL_Keymod mod ) {
		if(selected_control) {
			INTERCEPT_HOOK(key_up, OnKeyUp( sym, mod ));
			// selected_control->OnKeyUp( sym, mod );
		}
	}
	void GuiEngine::Render(  ) {
		bool has_selected_control = false;
		SDL_Rect pos = {0,0,0,0};
		for(auto &ca : cache) {
			if(ca.visible) {
				Control* const &c = ca.control;
				
				#ifdef SELECTED_CONTROL_ON_TOP
				if(c != selected_control) {
					c->Render( ren, pos, false );
				} else {
					has_selected_control = true;
				}
				#else
					c->Render(pos, c == selected_control);
				#endif
			}
		}
		
		#ifdef SELECTED_CONTROL_ON_TOP
		if(has_selected_control) {
			selected_control->Render(ren,pos,true);
		}
		#endif
	}
	void GuiEngine::OnEvent(SDL_Event &event) {
		switch(event.type) {
			case SDL_MOUSEMOTION:
				OnMouseMove( event.motion.x, event.motion.y );
				break;
			case SDL_KEYDOWN:
				OnKeyDown( event.key.keysym.sym, (SDL_Keymod)event.key.keysym.mod );
				break;
			case SDL_KEYUP:
				OnKeyUp( event.key.keysym.sym, (SDL_Keymod)event.key.keysym.mod );
				break;
			case SDL_MOUSEBUTTONUP:
				OnMouseUp( event.button.x, event.button.y );
				break;
			case SDL_MOUSEBUTTONDOWN:
				OnMouseDown( event.button.x, event.button.y );
				break;
			case SDL_MOUSEWHEEL:
				OnMWheel( event.wheel.y );
				break;
		}
	}
#endif
}