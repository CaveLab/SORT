/*
 * filename :	texture.h
 *
 * programmer :	Cao Jiayin
 */

#ifndef SORT_TEXTURE
#define SORT_TEXTURE

#include "../sort.h"
#include "texshower/texshower.h"
#include "../spectrum/spectrum.h"

// pre-decleration
class TexShower;

enum TEXCOORDFILTER
{
	TCF_WARP = 0 ,
	TCF_CLAMP ,
	TCF_MIRROR
};

//////////////////////////////////////////////////////////////
// defination of class Texture
class Texture
{
// public method
public:
	// default constructor
	Texture();
	// destructor
	~Texture();

	// set tex shower
	// para 'texShower' : the pointer to the texshower to set
	static void SetTexShower( TexShower* texShower );
	// output the texture
	// result : return true , if outputing is successful
	bool Output( const string& str );

	// get the texture value
	// para 'x' :	x coordinate , if out of range , use filter
	// para 'y' :	y coordinate , if out of range , use filter
	// result	:	spectrum value in the position
	virtual const Spectrum& GetColor( int x , int y ) const = 0;

	// get the texture value
	// para 'u'	:	x coordinate , if out of range , use filter
	// para 'v' :	y coordinate , if out of range , use filter
	virtual const Spectrum& GetColor( float u , float v ) const;

	// set texture coordinate filter mode
	void	SetTexCoordFilter( TEXCOORDFILTER mode );

	// get the size of the texture
	unsigned	GetWidth() const
	{
		return m_iTexWidth;
	}
	unsigned	GetHeight() const
	{
		return m_iTexHeight;
	}

	// set the size of the texture
	virtual void	SetWidth( unsigned w )
	{
		m_iTexWidth = w;
	}
	virtual void	SetHeight( unsigned h )
	{
		m_iTexHeight = h;
	}
	virtual void	SetSize( unsigned w , unsigned h )
	{
		m_iTexWidth = w;
		m_iTexHeight = h;
	}

// protected field
protected:
	// the texture shower
	static TexShower*  m_pTexShower;

	// the size of the texture
	// default values are zero
	unsigned	m_iTexWidth;
	unsigned	m_iTexHeight;

	// texture coordinate filter mode
	// default value is warp
	TEXCOORDFILTER	m_TexCoordFilter;

// protected method
protected:
	// initialize default value for texture
	void _init();

	// do texture filter
	void _texCoordFilter( int& u , int&v ) const;
};

#endif

