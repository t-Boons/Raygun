// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#pragma once

namespace Tmpl8 {

// helper macro for line clipping
#define OUTCODE(x,y) (((x)<xmin)?1:(((x)>xmax)?2:0))+(((y)<ymin)?4:(((y)>ymax)?8:0))

// pixel operations

// ScaleColor: change the intensity of red, green and blue using a single
// fixed-point scale in the range 0..256, where 256 is 100%.
inline uint32_t ScaleColor( const uint32_t c, const uint32_t scale )
{
	const uint32_t rb = (((c & 0xff00ff) * scale) >> 8) & 0x00ff00ff;
	const uint32_t ag = (((c & 0xff00ff00) >> 8) * scale) & 0xff00ff00;
	return rb + ag;
}

// AddBlend: add together two colors, with clamping.
inline uint32_t AddBlend( const uint32_t c1, const uint32_t c2 )
{
	const uint32_t r1 = (c1 >> 16) & 255, r2 = (c2 >> 16) & 255;
	const uint32_t g1 = (c1 >> 8) & 255, g2 = (c2 >> 8) & 255;
	const uint32_t b1 = c1 & 255, b2 = c2 & 255;
	const uint32_t r = min( 255u, r1 + r2 );
	const uint32_t g = min( 255u, g1 + g2 );
	const uint32_t b = min( 255u, b1 + b2 );
	return (r << 16) + (g << 8) + b;
}

// SubBlend: subtract a color from another color, with clamping.
inline uint32_t SubBlend( uint32_t a_Color1, uint32_t a_Color2 )
{
	int red = (a_Color1 & 0xff0000) - (a_Color2 & 0xff0000);
	int green = (a_Color1 & 0x00ff00) - (a_Color2 & 0x00ff00);
	int blue = (a_Color1 & 0x0000ff) - (a_Color2 & 0x0000ff);
	if (red < 0) red = 0;
	if (green < 0) green = 0;
	if (blue < 0) blue = 0;
	return (uint32_t)(red + green + blue);
}

// 32-bit surface container
class Surface
{
	enum { OWNER = 1 };
public:
	// constructor / destructor
	Surface() = default;
	Surface( int w, int h, uint32_t* buffer );
	Surface( int w, int h );
	Surface( const char* file );
	~Surface();
	// operations
	void InitCharset();
	void SetChar( int c, const char* c1, const char* c2, const char* c3, const char* c4, const char* c5 );
	void Print( const char* t, int x1, int y1, uint32_t c );
	void Clear( uint32_t c );
	void Line( float x1, float y1, float x2, float y2, uint32_t c );
	void Plot( int x, int y, uint32_t c );
	void LoadFromFile( const char* file );
	void CopyTo( Surface* dst, int x, int y );
	void Box( int x1, int y1, int x2, int y2, uint32_t color );
	void Bar( int x1, int y1, int x2, int y2, uint32_t color );
	// attributes
	uint32_t* pixels = 0;
	int width = 0, height = 0;
	bool ownBuffer = false;
	// static data for the hardcoded font
	static inline char font[51][5][6];
	static inline int transl[256];
	static inline bool fontInitialized = false;
};

// 8-bit (paletized) surface container
class Surface8
{
public:
	Surface8( int w, int h )
	{
		pixels = new unsigned char[w * h];
		pal = new unsigned int[256];
	}
	unsigned char* pixels;
	unsigned int* pal;
	int width, height;
};

}