// Warped and rotated sprite experiment
// ====================================
// November 16, 2023
// Joseph21

// This implementation is inspired by Nathan Reed - https://www.reedbeta.com/blog/quadrilateral-interpolation-part-2/
// and is created using the olc PixelGameEngine by Javidx9 - https://github.com/OneLoneCoder/olcPixelGameEngine

// For the rotated variant of the warped sprite drawing, the idea is to just rotate the corner points, and call
// the warped sprite drawing function.

#include <climits>       // needed for MIN_ and MAX_INT constants - thanks @Moros1138!

#include "ManipulatedSprite.h"

// if abs. value is smaller than this constant, it is considered to be 0
#define NEAR_ZERO   0.000001   // Note: floats have only 6-7 significant digits
                               //       doubles have 15-16

// convenience function to obtain the bounding box for a quad
void olc::GetQuadBoundingBox( std::array<olc::vf2d, 4> points, olc::vi2d &UpLeft, olc::vi2d &LwRght ) {
    // first determine bounding box of the quad
    UpLeft = { INT_MAX, INT_MAX };
    LwRght = { INT_MIN, INT_MIN };
    for (int i = 0; i < 4; i++) {
        UpLeft = UpLeft.min( points[i] );
        LwRght = LwRght.max( points[i] );
    }
}

// convenience function to obtain the bounding box for a quad
void olc::GetQuadBoundingBox( std::array<olc::vd2d, 4> points, olc::vi2d &UpLeft, olc::vi2d &LwRght ) {
    // first determine bounding box of the quad
    UpLeft = { INT_MAX, INT_MAX };
    LwRght = { INT_MIN, INT_MIN };
    for (int i = 0; i < 4; i++) {
        UpLeft = UpLeft.min( points[i] );
        LwRght = LwRght.max( points[i] );
    }
}

// convenience function to calculate the interception point of the two diagonals of a quad
// NOTE: not sure if this is correct since it only accounts for one diagonal
olc::vi2d olc::GetQuadCenterpoint( std::array<olc::vf2d, 4> points ) {
    // first determine bounding box of the quad
    olc::vi2d UpperLeft, LowerRight;
    GetQuadBoundingBox( points, UpperLeft, LowerRight );
    // then return the points where the diagonals intersect
    return UpperLeft + (LowerRight - UpperLeft) / 2.0;
}

// Function WarpedSample() and lambdas Get_q(), Get_b1() through Get_b3() are based on the
// article by Nathan Reed - https://www.reedbeta.com/blog/quadrilateral-interpolation-part-2/
// These are applied in DrawWarpedSprite() on a per pixel basis

// This struct holds one point (screen space) and the value for q from the bilinear interpolation analysis
struct AugmentedVertex {
    olc::vd2d pos = { 0.0, 0.0 };
    olc::vd2d q   = { 0.0, 0.0 };
};

// Works out the warped sample point for input denoted by q, b1, b2 and b3, and samples pSprite with it.
// The sampled colour is passed as a reference argument.
// Returns false if sampling is outside [0.0f, 1.0f] ^2 range ]
bool olc::WarpedSample( olc::vd2d q, olc::vd2d b1, olc::vd2d b2, olc::vd2d b3, olc::Sprite *pSprite, olc::Pixel &colour ) {
    // see the article by Nathan Reed, the "wedge product" appears to be some sort of cross product (?)
    auto wedge_2d = [=]( olc::vd2d v, olc::vd2d w ) {
        return v.x * w.y - v.y * w.x;
    };

    // Set up quadratic formula
    double A = wedge_2d( b2, b3 );
    double B = wedge_2d( b3, q  ) - wedge_2d( b1, b2 );
    double C = wedge_2d( b1, q  );

    // Solve for v
    olc::vd2d uv = { 0.0, 0.0 };
    if (fabs(A) < NEAR_ZERO) {
        // Linear form
        if (fabs(B) < NEAR_ZERO) {
            return false;
        }
        uv.y = -C / B;
    } else {
        // Quadratic form: Take positive root for CCW winding with V-up
        double D = B * B - 4 * A * C;
        if (D <= 0.0) {         // if discriminant <= 0, then the point is not inside the quad
            return false;
        }
        uv.y = 0.5 * (-B + sqrt( D )) / A;
    }
    // Solve for u, using largest magnitude component
    olc::vd2d denom = b1 + b3 * uv.y;
    if (fabs( denom.x ) > fabs( denom.y )) {
        if (fabs( denom.x ) < NEAR_ZERO) {
            return false;
        }
        uv.x = (q.x - b2.x * uv.y) / denom.x;
    } else {
        if (fabs( denom.y ) < NEAR_ZERO) {
            return false;
        }
        uv.x = (q.y - b2.y * uv.y) / denom.y;
    }
    // Note that vertical texel coord is mirrored because the algorithm assumes positive y to go up
    colour = pSprite->Sample( uv.x, 1.0 - uv.y );

    // return whether sampling produced a valid pixel
    return (uv.x >= 0.0 && uv.x <  1.0 &&
            uv.y >  0.0 && uv.y <= 1.0);
}

void olc::DrawWarpedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const std::array<olc::vf2d, 4> &cornerPoints ) {

    // These lambdas return respectively the values q, b1 - b3 from the bilinear interpolation analysis
    // The b1 - b3 values can be determined once per quad. The q value is associated per pixel
    auto Get_q  = [=] ( const std::array<olc::vd2d, 4> &cPts, const olc::vd2d &curVert ) -> olc::vd2d { return curVert - cPts[0];                     };
    auto Get_b1 = [=] ( const std::array<olc::vd2d, 4> &cPts                           ) -> olc::vd2d { return cPts[1] - cPts[0];                     };
    auto Get_b2 = [=] ( const std::array<olc::vd2d, 4> &cPts                           ) -> olc::vd2d { return cPts[2] - cPts[0];                     };
    auto Get_b3 = [=] ( const std::array<olc::vd2d, 4> &cPts                           ) -> olc::vd2d { return cPts[0] - cPts[1] - cPts[2] + cPts[3]; };

    // note that the corner points are passed in order: ul, ll, lr, ur, but the WarpedSample() algorithm
    // assumes the order ll, lr, ul, ur. This rearrangement is done here
    std::array<olc::vd2d, 4> localCornerPoints;
    localCornerPoints[0] = cornerPoints[1];
    localCornerPoints[1] = cornerPoints[2];
    localCornerPoints[2] = cornerPoints[0];
    localCornerPoints[3] = cornerPoints[3];

    // get b1-b3 values from the quad corner points
    // NOTE: the q value is associated per pixel and is obtained in the nested loop below
    olc::vd2d b1 = Get_b1( localCornerPoints );
    olc::vd2d b2 = Get_b2( localCornerPoints );
    olc::vd2d b3 = Get_b3( localCornerPoints );

    // determine the bounding box around the quad
    olc::vi2d UpperLeft, LowerRight;
    GetQuadBoundingBox( localCornerPoints, UpperLeft, LowerRight );

    // iterate all pixels within the bounding box of the quad...
    for (int y = UpperLeft.y; y <= LowerRight.y; y++) {
        for (int x = UpperLeft.x; x <= LowerRight.x; x++) {
            // ... and render them if sampling produces valid pixel
            olc::Pixel pix2render;
            olc::vd2d q = Get_q( localCornerPoints, { (double)x, (double)y } );

            if (WarpedSample( q, b1, b2, b3, pSprite, pix2render )) {
                gfx->Draw( x, y, pix2render );
            }
        }
    }
}

// These variants are there for compliance with the PGE interface - the implementation on decals

// Draws a sprite with 4 arbitrary points, warping the texture to look "correct"
void olc::DrawWarpedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const olc::vf2d(&pos)[4] ) {
    std::array<olc::vf2d, 4> localPoints;
    for (int i = 0; i < 4; i++) {
        localPoints[i] = pos[i];
    }
    DrawWarpedSprite( gfx, pSprite, localPoints );
}

// Draws a sprite with 4 arbitrary points, warping the texture to look "correct"
void olc::DrawWarpedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const olc::vf2d *pos ) {
    std::array<olc::vf2d, 4> localPoints;
    for (int i = 0; i < 4; i++) {
        localPoints[i] = pos[i];
    }
    DrawWarpedSprite( gfx, pSprite, localPoints );
}

// calculates new coordinates for each of the quadPoints, by rotating them with dAngle around centerPoint
void olc::RotateQuadPoints( std::array<olc::vd2d, 4> &quadPoints, double dAngle, olc::vd2d centerPoint ) {
    double dCosTheta = cos( dAngle );
    double dSinTheta = sin( dAngle );
    // iterate quad points
    for (int i = 0; i < 4; i++) {
        // first translate point so that center point aligns with origin
        quadPoints[i] -= centerPoint;
        // rotate point - because x component is altered and used for calculating y component
        // a cache is applied
        olc::vd2d cachePoint = quadPoints[i];
        quadPoints[i].x = cachePoint.x * dCosTheta - cachePoint.y * dSinTheta;
        quadPoints[i].y = cachePoint.x * dSinTheta + cachePoint.y * dCosTheta;
        // translate back
        quadPoints[i] += centerPoint;
    }
}

// Draws a sprite rotated to specified angle, with point of rotation offset
void olc::DrawRotatedSprite( PixelGameEngine *gfx, const olc::vf2d& pos, olc::Sprite *pSprite, const float fAngle, const olc::vf2d& center, const olc::vf2d& scale ) {
    // prepare call to RotateQuadPoints()
    std::array<olc::vd2d, 4> localPoints;
    olc::vd2d ul = pos;
    olc::vd2d lr = pos + olc::vd2d( pSprite->width * scale.x, pSprite->height * scale.y );
    localPoints[0] = olc::vd2d( ul.x, ul.y );
    localPoints[1] = olc::vd2d( ul.x, lr.y );
    localPoints[2] = olc::vd2d( lr.x, lr.y );
    localPoints[3] = olc::vd2d( lr.x, ul.y );
    olc::vd2d dCenterPoint = olc::vd2d( double( center.x ), double( center.y ));
    // rotate the points
    RotateQuadPoints( localPoints, double( fAngle ), dCenterPoint );
    // convert back to float type
    std::array<olc::vf2d, 4> renderPoints;
    for (int i = 0; i < 4; i++) {
        renderPoints[i] = localPoints[i];
    }
    // render sprite using the rotated cornerpoints
    DrawWarpedSprite( gfx, pSprite, renderPoints );
}

// renders a warped sprite that is rotated around centerPoint by fAngle
void olc::DrawWarpedRotatedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const std::array<olc::vf2d, 4> &cornerPoints, float fAngle, olc::vf2d centerPoint ) {
    // copy quad corner points
    std::array<olc::vd2d, 4> rotatedPoints;
    for (int i = 0; i < 4; i++) {
        rotatedPoints[i] = cornerPoints[i];
    }
    // convert float parameters to doubles
    double dAngle = double( fAngle );
    olc::vd2d dCenterPoint( centerPoint );

    // rotate them around center point
    RotateQuadPoints( rotatedPoints, dAngle, dCenterPoint );
    // convert back to correct type
    std::array<olc::vf2d, 4> localPoints;
    for (int i = 0; i < 4; i++) {
        localPoints[i] = rotatedPoints[i];
    }
    // draw sprite using rotated cornerpoints
    DrawWarpedSprite( gfx, pSprite, localPoints );
}
