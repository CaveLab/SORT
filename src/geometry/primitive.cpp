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

#include "primitive.h"
#include "light/light.h"
#include "bbox.h"
#include "managers/matmanager.h"

// delete the cache
void Primitive::ClearBBoxCache()
{
	m_bbox.Delete();
}

// get material
Material* Primitive::GetMaterial() const 
{ 
	if( m_mat == 0 ) 
		return MatManager::GetSingleton().GetDefaultMat();
	return m_mat; 
}
