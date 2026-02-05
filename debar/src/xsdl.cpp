#include <SDL3/SDL.h>
#include <SDL3_gfx/SDL3_gfxPrimitives.h>

/**
 * Extended variant of XSDL_RenderFillRectRound.
 * This allows you to specify radius per corner.
 */
void XSDL_RenderFillRectRoundEx(SDL_Renderer* renderer, float radiustl,float radiustr, float radiusbl, float radiusbr, SDL_FRect rect) {
    
}

bool XSDLGFX_aaFilledCircleRGBA(SDL_Renderer* renderer, float x, float y, float rad, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!filledCircleRGBA(renderer,x,y,rad,r,g,b,a))
        return false;
    return aacircleRGBA(renderer,x,y,rad,r,g,b,a);
}

/**
 * Draws a Rect with rounded corners.
 */
bool XSDL_RenderFillRectRound(SDL_Renderer* renderer, float radius, SDL_FRect* rect) {
    SDL_FRect rects[2] = {
        {rect->x,rect->y+radius,rect->w,rect->h-(radius*2)},
        {rect->x+radius,rect->y,rect->w-(radius*2),rect->h}
    };
    bool success = true;
    success &= SDL_RenderFillRects(renderer,rects,2);
    uint8_t r,g,b,a = 0;
    success &= SDL_GetRenderDrawColor(renderer,&r,&g,&b,&a);
    if (!success) return success;
    success &= XSDLGFX_aaFilledCircleRGBA(renderer,rect->x+radius,rect->y+radius,radius,r,g,b,a);
    if (!success) return success;
    success &= XSDLGFX_aaFilledCircleRGBA(renderer,rect->x+radius,rect->y+rect->h-radius-1,radius,r,g,b,a);
    success &= XSDLGFX_aaFilledCircleRGBA(renderer,rect->x+rect->w-radius-1,rect->y+radius,radius,r,g,b,a);
    success &= XSDLGFX_aaFilledCircleRGBA(renderer,rect->x+rect->w-radius-1,rect->y+rect->h-radius-1,radius,r,g,b,a);
    return success;
}