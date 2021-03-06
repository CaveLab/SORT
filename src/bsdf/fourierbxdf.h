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

#pragma once

// include header file
#include "bxdf.h"

//! @brief FourierBxdf.
/**
 * Same with MERL brdf, FourierBxdf is also a measured bxdf.
 * However, it is much compact comparing with MERL in term of memory usage.
 * There is also an importance sampling method provided in the bxdf type.
 */
class FourierBxdf : public Bxdf
{
public:
	//! Contstructor
	FourierBxdf();
	
    //! Evaluate the BRDF
    //! @param wo   Exitance direction in shading coordinate.
    //! @param wi   Incomiing direction in shading coordinate.
    //! @return     The evaluted BRDF value.
    Spectrum f( const Vector& wo , const Vector& wi ) const override;
	
    //! @brief Importance sampling for the bxdf.
    //!
    //! This method is not pure virtual and it has a default
    //! implementation, which sample out-going directions that have linear probability with the
    //! consine value between the out-going ray and the normal.\n
    //! However, it is suggested that each bxdf has its own importance sampling method for optimal
    //! convergence rate.\n
    //! One also needs to implement the function Pdf to make it consistance.
    //! @param wo   Exitance direction in shading coordinate.
    //! @param wi   Incomiing direction in shading coordinate.
    //! @param bs   Sample for bsdf that holds some random variables.
    //! @param pdf  Probability density of the selected direction.
    //! @return     The evaluted BRDF value.
    Spectrum sample_f( const Vector& wo , Vector& wi , const BsdfSample& bs , float* pdf ) const override;
    
    //! @brief Evalute the pdf of an existance direction given the incoming direction.
    //!
    //! If one implements customized sample_f for the brdf, it needs to have cooresponding version of
    //! this function, otherwise it is not unbiased.
    //! @param wo   Exitance direction in shading coordinate.
    //! @param wi   Incomiing direction in shading coordinate.
    //! @return     The probabilty of choosing the out-going direction based on the incoming direction.
    float Pdf( const Vector& wo , const Vector& wi ) const override;
    
    //! Load brdf data from Fourier Bxdf file.
    //! @param filename Name of Fourier Bxdf file.
    void	LoadData( const string& filename );

private:
    // Bxdf Table
    struct FourierBxdfTable{
        float   eta = 1.0f;
        int     nMax = 0;
        int     nChannels = 1;
        int     nMu = 0;
        float   *mu = nullptr;
        int     *m = nullptr;
        int     *aOffset = nullptr;
        float   *a = nullptr;
        float   *cdf = nullptr;
        float   *recip = nullptr;
        
        ~FourierBxdfTable(){
            SAFE_DELETE_ARRAY(mu);
            SAFE_DELETE_ARRAY(m);
            SAFE_DELETE_ARRAY(aOffset);
            SAFE_DELETE_ARRAY(a);
            SAFE_DELETE_ARRAY(cdf);
            SAFE_DELETE_ARRAY(recip);
        }
        
        float* GetAk( int offsetI , int offsetO , int* mptr ) const{
            const int offset = offsetO * nMu + offsetI;
            *mptr = m[offset];
            return a + aOffset[offset];
        }
    };
    
    FourierBxdfTable    bsdfTable;
    
    // Get weight and offset.
    void getWeightAndOffset( float costheta , int& offset , float* weight) const;
    // Fourier interpolation
    float fourier( const float* ak , int m , double cosPhi ) const;
    // Importance sampling for fourier interpolation
    // Refer these two wiki pages for further detail:
    // Bisection method :   https://en.wikipedia.org/wiki/Bisection_method
    // Newton method :      https://en.wikipedia.org/wiki/Newton%27s_method
    float sampleFourier( const float ak , const float* recip , int m , float u , float* pdf , float* phiptr );
};
