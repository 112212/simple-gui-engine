#ifndef COMMON_HPP
#define COMMON_HPP

typedef unsigned int Uint32;

namespace ng {

int old_bcd_to_binary( const char* str, int start, int end );
void old_binary_to_bcd( int binary, char* str, int start, int end );

int bcd_to_binary( const char* str, int start, int length );
void binary_to_bcd( int binary, char* str, int start, int length );

unsigned int hex_to_binary( const char* str, int start, int length );

constexpr unsigned int hash(const char *s, int off = 0) {
    return s[off] ? (hash(s, off+1)*33) ^ s[off] : 5381;
}

#ifdef USE_SFML
	char SFMLCodeToChar( unsigned char sf_code, bool shift );
#endif

}	
#endif