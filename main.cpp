// Warped and rotated sprite experiment
// ====================================
// November 16, 2023
// Joseph21

// Test program to test the functionality of ManiplatedSprite module.

#define OLC_IMAGE_STB     // to enable sprite saving

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "ManipulatedSprite.h"


class WarpedRotatedSprite : public olc::PixelGameEngine {
public:
    WarpedRotatedSprite() {
        sAppName = "Warped and rotated sprite experiment - Joseph21";
    }

    // class variables for test code
    olc::Sprite* sprOrg  = nullptr;    // contains original image
    olc::Sprite* sprDemo = nullptr;    // contains working (scaled) image

    std::array<olc::vf2d, 4> points;              // the corner points for the test quad
    olc::vf2d *pSelected = nullptr;    // ptr needed for drag/drop

    float fTheta = 0.0;                // angle for (warped) sprite rotation

    int nScaleSprite = 2;              // scale factor for the sprite to be sampled
    float fDispPercentage = 0.5f;      // scale factor for the display area on screen
    olc::vf2d scaleFactor = { 1.0f, 1.0f };   // scale factor for rendering the rotated sprite
    olc::vf2d partSource  = { 0.0f, 0.0f };
    olc::vf2d partSize    = { 1.0f, 1.0f };

    enum SpriteRenderMode {
        RotateOnly = 0,
        Warped,
        Partial
    } enRenderMode;


public:

    std::string RenderMode2String( SpriteRenderMode mode ) {
        switch (mode) {
            case RotateOnly: return "ROTATE_ONLY";
            case Warped    : return "WARPED"     ;
            case Partial   : return "PARTIAL"    ;
        }
        return "_INVALID_";
    }

    // creates a copy of orgSprite by scaling it with factor nScale
    // the copy is passed as ppSprite
    void ScaleSprite( olc::Sprite *orgSprite, olc::Sprite **ppSprite, int nScale ) {
        // if ppSprite points to an existing sprite, delete it first
        if (*ppSprite != nullptr)
            delete *ppSprite;
        // create new sprite as scaled version of orgSprite
        *ppSprite = new olc::Sprite( orgSprite->width * nScale, orgSprite->height * nScale );
        // fill the new sprite
        for (int y = 0; y < orgSprite->height; y++) {
            for (int x = 0; x < orgSprite->width; x++) {
                olc::Pixel p = orgSprite->GetPixel( x, y );
                for (int i = 0; i < nScale; i++) {
                    for (int j = 0; j < nScale; j++) {
                        (*ppSprite)->SetPixel( x * nScale + j, y * nScale + i, p );
                    }
                }
            }
        }
    }

    // reset the display rectangle in pts as a percentage (fPerc) of screen height
    void ScaleDisplay( std::array<olc::vf2d, 4> &pts, float fPerc ) {
        // the margin is what's left divided by 2
        float fMarginPerc = (1.0f - fPerc) * 0.5f;
        // work out upper left and lower right corner points
        olc::vi2d screenUL = { int( ScreenHeight() *         fMarginPerc ), int( ScreenHeight() *         fMarginPerc ) };
        olc::vi2d screenLR = { int( ScreenHeight() * (1.0f - fMarginPerc)), int( ScreenHeight() * (1.0f - fMarginPerc)) };
        // set points array
        int nWidthMargin = (ScreenWidth() - ScreenHeight()) / 2;
        pts[0] = { float( screenUL.x + nWidthMargin ), float( screenUL.y ) };
        pts[1] = { float( screenUL.x + nWidthMargin ), float( screenLR.y ) };
        pts[2] = { float( screenLR.x + nWidthMargin ), float( screenLR.y ) };
        pts[3] = { float( screenLR.x + nWidthMargin ), float( screenUL.y ) };
    }

    bool OnUserCreate() override {

        std::string sPath = "./_assets/";
        sprOrg = new olc::Sprite( sPath +
            // pick one of the test sprite files below
//            "small_mario.png"         //   13 x   16 pixels
            "elfgirl05.png"           // 100 x 100 pixels
//            "tree 150x150.png"        //  150 x  150 pixels
//            "compass.png"             //  231 x  231 pixels
//            "crate.png"               // 464 x 464 pixels
//            "my_tree.png"             //  900 x  900 pixels
//            "Risor_3008x1692.png"     // 3008 x 1692 pixels
        );
        nScaleSprite = 2;
        ScaleSprite( sprOrg, &sprDemo, nScaleSprite );
        fDispPercentage = 0.5f;
        ScaleDisplay( points, fDispPercentage );

        scaleFactor = {
            float( points[3].x - points[0].x ) / sprDemo->width,
            float( points[1].y - points[0].y ) / sprDemo->height,
        };

        partSource = { 0.0f, 0.0f };
        partSize = { float( sprDemo->width ), float( sprDemo->height ) };

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // User input part
        // ===============

        olc::vd2d mouse = { double(GetMouseX()), double(GetMouseY()) };  // grab mouse position

        // if mouse button is pressed, determine if any of the corner points is selected
        // signal this using the value of pSelected
        if (GetMouse(0).bPressed) {
            pSelected = nullptr;
            for (auto &p : points) {
                if ((p - mouse).mag() < 6)
                    pSelected = &p;
            }
        }
        // if mouse button is released, cancel any selection
        if (GetMouse(0).bReleased)
            pSelected = nullptr;
        // if a corner point is selected, follow mouse pointer
        if (pSelected != nullptr) {
            *pSelected = mouse;
        }

        // determine multiplier factor (for speeding up or down)
        double dMultiplier = 1.0;
        if (GetKey( olc::SHIFT ).bHeld) dMultiplier *= 4.00;
        if (GetKey( olc::CTRL  ).bHeld) dMultiplier *= 0.25;

        // rotate sprite
        if (GetKey( olc::Key::LEFT  ).bHeld) fTheta -= dMultiplier * fElapsedTime;
        if (GetKey( olc::Key::RIGHT ).bHeld) fTheta += dMultiplier * fElapsedTime;

        olc::vf2d centerPt = GetQuadCenterpoint( points );

        // scale sprite
        if (GetKey( olc::Key::NP_ADD).bPressed) {
            nScaleSprite += 1;
            ScaleSprite( sprOrg, &sprDemo, nScaleSprite );
        }
        if (GetKey( olc::Key::NP_SUB).bPressed) {
            nScaleSprite -= 1;
            if (nScaleSprite < 1)
                nScaleSprite = 1;
            ScaleSprite( sprOrg, &sprDemo, nScaleSprite );
        }
        // scale display surface
        if (GetKey( olc::Key::PGDN ).bHeld) { fDispPercentage -= float( dMultiplier ) * fElapsedTime; ScaleDisplay( points, fDispPercentage ); }
        if (GetKey( olc::Key::PGUP ).bHeld) { fDispPercentage += float( dMultiplier ) * fElapsedTime; ScaleDisplay( points, fDispPercentage ); }
        // scale sprite itself
        if (GetKey( olc::Key::HOME ).bHeld) { scaleFactor.x -= 1.0f * fElapsedTime; }
        if (GetKey( olc::Key::END  ).bHeld) { scaleFactor.x += 1.0f * fElapsedTime; }
        if (GetKey( olc::Key::INS  ).bHeld) { scaleFactor.y -= 1.0f * fElapsedTime; }
        if (GetKey( olc::Key::DEL  ).bHeld) { scaleFactor.y += 1.0f * fElapsedTime; }
        // cycle rendermode
        if (GetKey( olc::Key::M ).bPressed) {
            switch (enRenderMode) {
                case RotateOnly: enRenderMode = Warped    ; break;
                case Warped    : enRenderMode = Partial   ; break;
                case Partial   : enRenderMode = RotateOnly; break;
            }
        }
        // change partial sprite parameters
        if (GetKey( olc::Key::NP1 ).bHeld) { partSource -= olc::vf2d( 10.0f * fElapsedTime, 10.0f * fElapsedTime ); }
        if (GetKey( olc::Key::NP7 ).bHeld) { partSource += olc::vf2d( 10.0f * fElapsedTime, 10.0f * fElapsedTime ); }
        if (GetKey( olc::Key::NP3 ).bHeld) { partSize   -= olc::vf2d( 10.0f * fElapsedTime, 10.0f * fElapsedTime ); }
        if (GetKey( olc::Key::NP9 ).bHeld) { partSize   += olc::vf2d( 10.0f * fElapsedTime, 10.0f * fElapsedTime ); }

        bool bSaveMode = GetKey( olc::S ).bPressed;
        olc::Sprite *pSaveSpr = nullptr;

        // Rendering part
        // ==============

        Clear( olc::VERY_DARK_BLUE );

        // here's the actual test code
        // ---------------------------
        SetPixelMode( olc::Pixel::MASK );

        olc::vf2d originPoint = olc::vf2d( points[0].x, points[0].y );

        if (bSaveMode) {
            // create a sprite the size of the screen and set it as draw target
            pSaveSpr = new olc::Sprite( ScreenWidth(), ScreenHeight() );
            SetDrawTarget( pSaveSpr );
        }

        switch (enRenderMode) {
            case RotateOnly: DrawRotatedSprite(        this, originPoint, sprDemo, fTheta, centerPt,                       scaleFactor ); break;
            case Warped    : DrawWarpedRotatedSprite(  this, sprDemo, points     , fTheta, centerPt                                    ); break;
            case Partial   : DrawPartialRotatedSprite( this, originPoint, sprDemo, fTheta, centerPt, partSource, partSize, scaleFactor ); break;
        }

        if (bSaveMode) {
            std::string sFileName = "snap" + std::to_string( time( 0 )) + ".png";
            pSaveSpr->SaveToFile( sFileName );
            SetDrawTarget( nullptr );
        }

        SetPixelMode( olc::Pixel::NORMAL );

        // for the demo - draw lines around the quad and circles at the corner points
        DrawLine( points[0].x, points[0].y, points[1].x, points[1].y, olc::YELLOW );
        DrawLine( points[1].x, points[1].y, points[2].x, points[2].y, olc::YELLOW );
        DrawLine( points[2].x, points[2].y, points[3].x, points[3].y, olc::YELLOW );
        DrawLine( points[3].x, points[3].y, points[0].x, points[0].y, olc::YELLOW );
        for (int i = 0; i < 4; i++) {
            FillCircle( points[i], 4, olc::YELLOW );
        }

        // output the test parameters on screen
        DrawString(  10, 10, "[NP_SUB/NP_ADD] Sprite scale      : " + std::to_string( nScaleSprite    )                                            , olc::YELLOW );
        DrawString(  10, 20, "                Sprite size       : " + std::to_string( sprDemo->width  ) + " x " + std::to_string( sprDemo->height ), olc::YELLOW );
        DrawString(  10, 30, "[  PGDN/PGUP  ] Display scale     : " + std::to_string( fDispPercentage ) + " = "
                                                                   + std::to_string( fDispPercentage * ScreenHeight() ) + " pixels"               , olc::YELLOW );
        DrawString(  10, 40, "[   <--/-->   ] Rot. angle        : " + std::to_string( fTheta          )                                            , olc::YELLOW );
        DrawString(  10, 50, "[  HOME/END   ]"                                                                                                     , olc::YELLOW );
        DrawString(  10, 60, "[    INS/DEL  ] Sprite renderscale: " + std::to_string( scaleFactor.x   ) + " x " + std::to_string( scaleFactor.y   ), olc::YELLOW );

        DrawString( 610, 10, "Mode                       : " + RenderMode2String( enRenderMode )                                                   , olc::YELLOW );
        DrawString( 610, 20, "[  NP_1/NP_7  ] Part origin: (" + std::to_string( partSource.x ) + ", " + std::to_string( partSource.y ) + ")"       , olc::YELLOW );
        DrawString( 610, 30, "[  NP_3/NP_9  ] Part size  : (" + std::to_string( partSize.x )   + ", " + std::to_string( partSize.y )   + ")"       , olc::YELLOW );

        return !GetKey( olc::Key::ESCAPE ).bPressed;
    }
};

// keep the screen dimensions constant and vary the resolution by adapting the pixel size
#define SCREEN_X   1400
#define SCREEN_Y    800
#define PIXEL_X       1
#define PIXEL_Y       1

int main()
{
    WarpedRotatedSprite demo;
    if (demo.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y ))
        demo.Start();

    return 0;
}
