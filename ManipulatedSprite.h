#ifndef MANIPULATEDSPRITE_H
#define MANIPULATEDSPRITE_H

// Warped and rotated sprite module
// ================================
// November 16, 2023
// Joseph21
//
// The warped sprite implementation is heavily inspired on the nice article by Nathan Reed:
//   https://www.reedbeta.com/blog/quadrilateral-interpolation-part-2/
//
// This module is inspired on and created for use with the olc PixelGameEngine by Javidx9:
//   https://github.com/OneLoneCoder/olcPixelGameEngine
//
// The rotated sprite drawing is implemented by rotating the quad corner points and draw
// the rotated quad calling the DrawWarpedSprite() implementation.
//
// Have fun with it!
// Joseph21

#include "olcPixelGameEngine.h"

namespace olc {

    // convenience function to obtain the bounding box for a quad
    void GetQuadBoundingBox( std::array<olc::vf2d, 4> points, olc::vi2d &UpLeft, olc::vi2d &LwRght );
    void GetQuadBoundingBox( std::array<olc::vd2d, 4> points, olc::vi2d &UpLeft, olc::vi2d &LwRght );
    // convenience function to calculate the center point of a quad
    olc::vi2d GetQuadCenterpoint( std::array<olc::vf2d, 4> points );
    // calculates new coordinates for each of the quadPoints, by rotating them with dAngle around centerPoint
    void RotateQuadPoints( std::array<olc::vd2d, 4> &quadPoints, double dAngle, olc::vd2d centerPoint );

    // this function is called instead of regulare Sample() from DrawWarpedSprite()
    bool WarpedSample( olc::vd2d q, olc::vd2d b1, olc::vd2d b2, olc::vd2d b3, olc::Sprite *pSprite, olc::Pixel &colour );
    // Draws a sprite with 4 arbitrary points, warping the texture to look "correct"
    void DrawWarpedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const std::array<olc::vf2d, 4> &cornerPoints );
    void DrawWarpedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const olc::vf2d(&pos)[4] );
    void DrawWarpedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const olc::vf2d *pos );

    // Draws a sprite rotated to specified angle, with point of rotation offset
    void DrawRotatedSprite( PixelGameEngine *gfx, const olc::vf2d& pos, olc::Sprite *pSprite, const float fAngle, const olc::vf2d& center = { 0.0f, 0.0f }, const olc::vf2d& scale = { 1.0f, 1.0f } );

    // YET TO IMPLEMENT: void DrawPartialRotatedSprite( const olc::vf2d& pos, olc::Sprite *pSprite, const float fAngle, const olc::vf2d& center, const olc::vf2d& source_pos, const olc::vf2d& source_size, const olc::vf2d& scale = { 1.0f, 1.0f } );

    // renders a warped sprite that is rotated around centerPoint by fAngle
    void DrawWarpedRotatedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const std::array<olc::vf2d, 4> &cornerPoints, float fAngle, olc::vf2d centerPoint );
};

#endif // MANIPULATEDSPRITE_H
