#ifndef NG_FONT_HPP
#define NG_FONT_HPP
#include <string>
#include "Resource.hpp"
namespace ng {

struct GlyphMetrics {
	int height;
	int advance;
};

class Image;

class Font : public Resource {
	protected:
		Font();
	public:
		virtual ~Font();
		virtual GlyphMetrics GetGlyphMetrics( uint32_t c );
		virtual void BlitText( Image* img, int x, int y, std::string text, uint32_t color );
		virtual Image* GetTextImage( std::string text, uint32_t color );
		
		// metrics
		int GetMaxText( const std::string &text, int width );
		int GetMaxTextRep( char c, int width );
		int GetMaxTextBw( const std::string &text, int width );
		int GetTextSize( const std::string &text );
		int GetHeight();
		std::string ClipText( std::string s, int w );
};


}

#endif
