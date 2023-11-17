# Sprite-manipulation-warping-rotating-scaling
A module to add functions for sprite rotating, warping and scaling when using the olcPixelGameEngine

The implementation of the sprite warping function is heavily inspired by Nathan Reed (see: https://www.reedbeta.com/blog/quadrilateral-interpolation-part-2/).

The function is to be used in combination with the olc PixelGameEngine by Javidx9 (see: https://github.com/OneLoneCoder/olcPixelGameEngine)

The sprite rotation functions are created by simply rotating the corner points of the sprite around the specified center point, and then calling the warped sprite drawing function.

Have fun!
Joseph21
