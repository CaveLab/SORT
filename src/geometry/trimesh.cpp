/*
    This file is a part of SORT(Simple Open Ray Tracing), an open-source cross
    platform physically based renderer.
 
    Copyright (c) 2011-2016 by Cao Jiayin - All rights reserved.
 
    SORT is a free software written for educational purpose. Anyone can distribute
    or modify it under the the terms of the GNU General Public License Version 3 as
    published by the Free Software Foundation. However, there is NO warranty that
    all components are functional in a perfect manner. Without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.
 
    You should have received a copy of the GNU General Public License along with
    this program. If not, see <http://www.gnu.org/licenses/gpl-3.0.html>.
 */

// include the header file
#include "trimesh.h"
#include "sort.h"
#include "math/point.h"
#include "managers/meshmanager.h"
#include "geometry/triangle.h"
#include "geometry/instancetri.h"
#include "managers/logmanager.h"
#include "managers/memmanager.h"
#include "managers/matmanager.h"

// default constructor
TriMesh::TriMesh( const string& name ):m_Name(name)
{
	// initialize default data
	_init();
}
// destructor
TriMesh::~TriMesh()
{
	_release();
}

// initialize default data
void TriMesh::_init()
{
	m_pMemory = 0;
	m_pMaterials = 0;
}

// release the default data
void TriMesh::_release()
{
	SAFE_DELETE_ARRAY(m_pMaterials);
}

// load the mesh
bool TriMesh::LoadMesh( const string& str , Transform& transform )
{
	// set the tranformation
	m_Transform = transform;

	// load the mesh
	bool flag = MeshManager::GetSingleton().LoadMesh( str , this );
	if( false == flag )
		return false;

	_copyMaterial();

	return true;
}

// copy materials
void TriMesh::_copyMaterial()
{
	SAFE_DELETE_ARRAY(m_pMaterials);

	unsigned trunk_size = m_pMemory->m_TrunkBuffer.size();
	m_pMaterials = new Reference<Material>[trunk_size];
	
	for( unsigned i = 0 ; i < trunk_size ; ++i )
	{
		if( m_pMemory->m_TrunkBuffer[i]->m_mat )
			m_pMaterials[i] = m_pMemory->m_TrunkBuffer[i]->m_mat;
		else
			m_pMaterials[i] = MatManager::GetSingleton().GetDefaultMat();
		m_pMemory->m_TrunkBuffer[i]->m_mat = 0;
	}
}

// fill buffer into vector
void TriMesh::FillTriBuf( vector<Primitive*>& vec )
{
	unsigned base = vec.size();
	if( m_bInstanced == false )
	{
		// generate the triangles
		unsigned trunkNum = m_pMemory->m_TrunkBuffer.size();
		for( unsigned i = 0 ; i < trunkNum ; i++ )
		{
			unsigned trunkTriNum = m_pMemory->m_TrunkBuffer[i]->m_IndexBuffer.size() / 3;
			for( unsigned k = 0 ; k < trunkTriNum ; k++ )
				vec.push_back( new Triangle( base+k , this , &(m_pMemory->m_TrunkBuffer[i]->m_IndexBuffer[3*k]) , m_pMaterials[i]) );
			base += m_pMemory->m_TrunkBuffer[i]->m_IndexBuffer.size() / 3;
		}
	}else
	{
		// generate the triangles
		unsigned trunkNum = m_pMemory->m_TrunkBuffer.size();
		for( unsigned i = 0 ; i < trunkNum ; i++ )
		{
			unsigned trunkTriNum = m_pMemory->m_TrunkBuffer[i]->m_IndexBuffer.size() / 3;
			for( unsigned k = 0 ; k < trunkTriNum ; k++ )
				vec.push_back( new InstanceTriangle( base+k , this , &(m_pMemory->m_TrunkBuffer[i]->m_IndexBuffer[3*k]) , &m_Transform , m_pMaterials[i] ) );
			base += m_pMemory->m_TrunkBuffer[i]->m_IndexBuffer.size() / 3;
		}
	}
}

// reset material
void TriMesh::ResetMaterial( const string& setname , const string& matname )
{
	// get the material first
	Material* mat = MatManager::GetSingleton().FindMaterial( matname );
	if( mat == 0 )
		LOG_WARNING<<"There is no such a material named \'"<<matname<<"\'."<<ENDL;

	// if there is no set name , all of the sets are set the material with the name of 'matname'
	if( setname.empty() )
	{
		unsigned size = m_pMemory->m_TrunkBuffer.size();
		for( unsigned i = 0 ; i < size ; ++i )
			m_pMaterials[i] = mat;
		return;
	}

	int id = _getSubsetID( setname );
	if( id < 0 )
	{
		LOG_WARNING<<"There is no such subset named "<<setname<<ENDL;
		return;
	}
	m_pMaterials[id] = mat;
}

// get the subset of the mesh
int TriMesh::_getSubsetID( const string& setname )
{
	int size = (int)m_pMemory->m_TrunkBuffer.size();
	for( int i = 0 ; i < size ; ++i )
	{
		if( m_pMemory->m_TrunkBuffer[i]->name == setname )
			return i;
	}
	return -1;
}
