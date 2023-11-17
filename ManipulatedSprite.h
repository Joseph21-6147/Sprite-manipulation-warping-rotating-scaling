#ifndef MANIPULATEDSPRITE_H
#define MANIPULATEDSPRITE_H

// Warped and rotated sprite module
// ================================
// November 17, 2023
// Joseph21
//
// The warped sprite implementation is heavily inspired on the nice article by Nathan Reed:
//   https://www.reedbeta.com/blog/quadrilateral-interpolation-part-2/
//
// This module is inspired by and created for use with the olcPixelGameEngine by Javidx9:
//   https://github.com/OneLoneCoder/olcPixelGameEngine
//
// The rotated sprite drawing is implemented by rotating the quad corner points and draw
// the rotated quad calling the DrawWarpedSprite() implementation.
//
// Have fun with it!
// Joseph21

#include "olcPixelGameEngine.h"

namespace olc {

    // Convenience function to obtain the bounding box for a quad.
    // The quad is input parameter points, the bounding box is passed back as two corner points
    // by references UpLeft and LwRight. The function is overloaded, both for float and for double quad points.
    void GetQuadBoundingBox( std::array<olc::vf2d, 4> points, olc::vi2d &UpLeft, olc::vi2d &LwRght );
    void GetQuadBoundingBox( std::array<olc::vd2d, 4> points, olc::vi2d &UpLeft, olc::vi2d &LwRght );

    // Convenience function to calculate and return the center point of a quad.
    olc::vi2d GetQuadCenterpoint( std::array<olc::vf2d, 4> points );

    // Calculates new coordinates for each of the quadPoints, by rotating them with dAngle around centerPoint.
    // Note that parameter quadPoints is affected in this function.
    void RotateQuadPoints( std::array<olc::vd2d, 4> &quadPoints, double dAngle, olc::vd2d centerPoint );

    // This function is called from DrawWarpedSprite() (as alternative for the regular Sample() function).
    // The bilinear warping is implemented for the better part in this function
    bool WarpedSample( olc::vd2d q, olc::vd2d b1, olc::vd2d b2, olc::vd2d b3, olc::Sprite *pSprite, olc::Pixel &colour );

    // Draws a sprite with 4 arbitrary points, warping the texture to look "correct".
    // The different signatures are there for compliancy with the PGE on the DrawWarpedDecal() family of functions.
    void DrawWarpedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const std::array<olc::vf2d, 4> &cornerPoints );
    void DrawWarpedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const olc::vf2d(&pos)[4] );
    void DrawWarpedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const olc::vf2d *pos );

    // Draws a sprite at screen location pos, rotated to specified fAngle (radians), with point of rotation offset. You can scale the rotated sprite as well.
    void DrawRotatedSprite( PixelGameEngine *gfx, const olc::vf2d& pos, olc::Sprite *pSprite, const float fAngle, const olc::vf2d& center = { 0.0f, 0.0f }, const olc::vf2d& scale = { 1.0f, 1.0f } );

    // Pretty much the same as DrawRotatedSprite(), but only a part of the sprite is rendered
    void DrawPartialRotatedSprite( PixelGameEngine *gfx, const olc::vf2d& pos, olc::Sprite *pSprite, const float fAngle, const olc::vf2d& center, const olc::vf2d& source_pos, const olc::vf2d& source_size, const olc::vf2d& scale = { 1.0f, 1.0f } );

    // Draws a warped sprite that is rotated around centerPoint by fAngle.
    // NOTE: the same effect could be achieved by calling RotateQuadPoints() and then call DrawWarpedSprite() [ in fact this is how it's implemented ]
    void DrawWarpedRotatedSprite( PixelGameEngine *gfx, olc::Sprite *pSprite, const std::array<olc::vf2d, 4> &cornerPoints, float fAngle, olc::vf2d centerPoint );
};

#endif // MANIPULATEDSPRITE_H
