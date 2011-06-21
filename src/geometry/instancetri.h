/*
 * filename :	instancetri.h
 *
 * programmer :	Cao Jiayin
 */

#ifndef	SORT_INSTANCETRI
#define	SORT_INSTANCETRI

// include the header
#include "primitive.h"

// pre-declera class
class Triangle;

////////////////////////////////////////////////////////////////////////////////
// defination of instance triangle
// note :	an instance triangle is just a copy of a triangle, it doesn't 
//			save the position data of the triangle. when performing intersection
//			test , it first transform the ray from world space to object space
//			and then to the prototype-primitive space, it costs more than
//			original triangle , but saves memory space.
class	InstanceTriangle : public Primitive
{
// public method
public:
	// constructor from a Triangle
	InstanceTriangle( const Triangle* tri );
	// destructor
	~InstanceTriangle();

	// get the instersection between a ray and a instance triangle
	float	GetIntersect( const Ray& r ) const;	

// private field
private:

};

#endif