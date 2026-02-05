#include <SDL3/SDL.h>

/**
 * Extended variant of XSDL_RenderFillRectRound.
 * This allows you to specify radius per corner.
 */
void XSDL_RenderFillRectRoundEx(SDL_Renderer* renderer, float radiustl,float radiustr, float radiusbl, float radiusbr, SDL_FRect rect);

/**
 * Draws a Rect with rounded corners.
 */
bool XSDL_RenderFillRectRound(SDL_Renderer* renderer, float radius, SDL_FRect* rect);