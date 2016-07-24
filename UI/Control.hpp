#ifndef NG_CONTROL_HPP
#define NG_CONTROL_HPP

#ifdef USE_SFML
	#include <SFML/Graphics.hpp>
	#include <SFML/Window/Event.hpp>
#elif USE_SDL
	#include <SDL2/SDL.h>
#endif

#include "common/common.hpp"
#include "common/Colors.hpp"
#include "common/Cursors.hpp"
#include "common/Fonts.hpp"

#define SELECTION_MARK

#include "common/debug.hpp"
#include <vector>
#include <ostream>

namespace ng {
struct Point {
	union{
		int x,min;
	};
	union {
		int y,max;
	};
	Point() {}
	Point(int _x, int _y) : x(_x),y(_y) {}
	bool operator< (const Point& b) const { return x < b.x || (x == b.x && y < b.y); }
};

struct Rect : Point {
	int w;
	int h;
	Rect() {}
	Rect(int _x, int _y, int _w, int _h) : Point(_x,_y),w(_w),h(_h) {}
};
Rect getRect( int x, int y, int w, int h );

struct Anchor {
	Point coord;
	float x,y;
	float W,w,H,h;
	float sW,sH;
	float sx,sy;
	bool isrelative;
	int ax,ay;
	Anchor& operator+=(const Anchor& b);
	friend std::ostream& operator<< (std::ostream& stream, const Anchor& anchor);
};

// #define SSTR( x ) dynamic_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str()



enum controlType {
	TYPE_CONTROL = 0,
	TYPE_WIDGET,
	TYPE_SCROLLBAR,
	TYPE_TRACKBAR,
	TYPE_BUTTON,
	TYPE_RADIOBUTTON,
	TYPE_CHECKBOX,
	TYPE_LABEL,
	TYPE_CANVAS,
	TYPE_TEXTBOX,
	TYPE_COMBOBOX,
	TYPE_LISTBOX,
	TYPE_GRID_CONTAINER,
	TYPE_DIALOG,
	MAX_CONTROL_TYPES
};

enum {
	GUI_UNSELECT = 0xff,
	GUI_UNSELECT_WIDGET,
	GUI_UNLOCK,
	GUI_FOCUS_LOCK,
	GUI_KEYBOARD_LOCK,
	GUI_WIDGET_UNLOCK,
	GUI_LOCK_ONCE
};

class Widget;
class Control;
struct Event;


class GuiEngine;

// cache related
enum CacheUpdateFlag {
	all = 0,
	position = 1,
	attributes = 2
};

class Control {
	private:
		bool isWidget;
		controlType type;
		GuiEngine* engine;
		Widget* widget;
		friend class Widget;
		friend class GuiEngine;
		friend class ControlManager;
		bool visible;
		bool interactible;
		std::string id;
		std::vector< std::vector< std::function<void(Control*)> > > subscribers;
		Anchor anchor;
		
		// compiler screams ambiguous for this, so had to add _
		void _updateCache(CacheUpdateFlag flag);
		// void updateCache() {}
	protected:
		int minor_type;
		int z_index;
		
		// this shouldn't be changed manually
		// TODO: put this to private, and make everythi use GetRect()
		Rect m_rect;
		static bool custom_check;
		
		// functions used by controls
		void setType(controlType type);
		void initEventVector(int max_events) { subscribers.resize(max_events); }
		void emitEvent( int EventID );
		void sendGuiCommand( int eventId );
		inline Widget* getWidget() { return widget; }
		inline GuiEngine* getEngine() { return engine; }
		bool check_collision(int x, int y);
		void setInteractible(bool interactible);
		const Point getOffset();
		
		const std::vector<Control*> getWidgetControls();
		const std::vector<Control*> getEngineControls();
		
		// internal virtual
		virtual void onPositionChange();
		virtual bool customBoundary( int x, int y );
		
		// called by gui engine, controls can override these functions
		#ifdef USE_SFML
			virtual void Render( sf::RenderTarget &ren, sf::RenderStates state, bool isSelected );
			virtual void OnKeyDown( sf::Event::KeyEvent &sym );
			virtual void OnKeyUp( sf::Event::KeyEvent &sym );
		#elif USE_SDL
			virtual void Render( SDL_Rect position, bool isSelected );
			virtual void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod );
			virtual void OnKeyUp(  SDL_Keycode &sym, SDL_Keymod mod );
		#endif
		virtual void OnMouseMove( int mX, int mY, bool mouseState );
		virtual void OnMouseDown( int mX, int mY );
		virtual void OnMouseUp( int mX, int mY );
		virtual void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnLostControl();
		virtual void OnMWheel( int updown );
		virtual void STYLE_FUNC(value);
		
	public:
		Control();
		~Control();
		virtual Control* Clone();
		void SetRect( Rect r );
		const Rect& GetRect( ) { return m_rect; } 
		void SetRect( int x, int y, int w, int h );
		void SetPosition( int x, int y ) { SetRect(x, y, m_rect.w, m_rect.h); }
		
		void SetAnchor( float W, float w, float x, float H, float h, float y );
		void SetAnchor( const Anchor& anchor );
		const Anchor& GetAnchor();
		
		controlType GetType() { return type; }
		
		bool IsWidget() { return isWidget; }
		void SetStyle(std::string& style, std::string& value);
		void SetVisible(bool visible);
		bool IsVisible() { return visible; }
		void SetId( std::string id );
		std::string GetId() { return this->id; }
		void SetZIndex( int zindex );
		int GetZIndex() { return z_index; }
		void SubscribeEvent( int event_type, std::function<void(Control*)> callback );
};
}
#endif
