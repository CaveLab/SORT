/*
 * filename :	texmanager.cpp
 *
 * programmer :	Cao Jiayin
 */

// include header file
#include "texmanager.h"
#include "texio/bmpio.h"

// instance the singleton with tex manager
DEFINE_SINGLETON(TexManager);

// create tex manager
void TexManager::CreateTexManager()
{
	new TexManager();
}

// destructor
TexManager::~TexManager()
{
	_release();
}

// default constructor
TexManager::TexManager()
{
	_init();
}

// initialize data
void TexManager::_init()
{
	// push the texture outputer
	m_TexIOVec.push_back( new BmpIO() );
}

// release data
void TexManager::_release()
{
	// release texture outputer
	vector<TexIO*>::const_iterator it = m_TexIOVec.begin();
	while( it != m_TexIOVec.end() )
	{
		delete *it;
		it++;
	}
	m_TexIOVec.clear();
}

// output texture
bool TexManager::Write( const string& str , const Texture* tex , TEX_OUTPUT_TYPE type )
{
	// find the specific texio first
	TexIO* io = 0;
	vector<TexIO*>::const_iterator it = m_TexIOVec.begin();
	while( it != m_TexIOVec.end() )
	{
		if( (*it)->GetTOT() == type )
		{
			io = *it;
			break;
		}
		it++;
	}

	if( io != 0 )
		io->Write( str , tex );
	
	return true;
}