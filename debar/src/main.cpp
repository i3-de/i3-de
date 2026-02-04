#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <SDL3/SDL.h>
#include <iostream>

int main() {
    bool running = false;
    SDL_Window* wnd;
    SDL_Renderer* ren;
    SDL_Event e;
    SDL_Rect bounds;
    int barWidth;
    int barHeight;
    SDL_PropertiesID props;
    Display* xdisplay;
    Window xwindow;
    long strut[12] = {0};
    Atom net_wm_strut;
    Atom wm_type;
    Atom wm_dock;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        // no need to `goto safeExit` here; nothing is initialized.
        return 1;
    }
    bounds = {};
    if (!SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(),&bounds)) {
        std::cerr << "SDL_GetDisplayBounds Error: " << SDL_GetError() << std::endl;
        goto safeExit;
        return 1;
    }
    barWidth = bounds.w;
    barHeight = 20;
    wnd = SDL_CreateWindow(
       "debar",
       barWidth,barHeight,
       SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP
    );
    if (!wnd) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        goto safeExit;
        return 2;
    }
    SDL_ShowWindow(wnd);
    props = SDL_GetWindowProperties(wnd);
    xdisplay = (Display*)SDL_GetPointerProperty(
        props,
        SDL_PROP_WINDOW_X11_DISPLAY_POINTER,
        NULL
    );
    xwindow = (Window)SDL_GetNumberProperty(
        props,
        SDL_PROP_WINDOW_X11_WINDOW_NUMBER,
        0
    );
    wm_type = XInternAtom(xdisplay, "_NET_WM_WINDOW_TYPE", False);
    wm_dock = XInternAtom(xdisplay, "_NET_WM_WINDOW_TYPE_DOCK", False);
    strut[2] = barHeight;
    net_wm_strut = XInternAtom(xdisplay, "_NET_WM_STRUT", False);
    XChangeProperty( // make it a dock window
        xdisplay,
        xwindow,
        wm_type,
        XA_ATOM,
        32,
        PropModeReplace,
        (unsigned char*)&wm_dock,
        1
    );
    XChangeProperty( // set strut ;; reserve space on the top for my bar
        xdisplay,
        xwindow,
        net_wm_strut,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)strut,
        4
    );
    XFlush(xdisplay);
    SDL_SetWindowPosition(wnd,0,0);
    SDL_SyncWindow(wnd);
    ren = SDL_CreateRenderer(
        wnd,
        NULL
    );
    if (!ren) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        goto safeExit;
        return 3;
    }
    running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        SDL_SetRenderDrawColor(ren,0,0,0,255);
        SDL_RenderClear(ren);
        SDL_FRect rect = {0,0,50,50};
        SDL_SetRenderDrawColor(ren,255,0,0,255);
        SDL_RenderFillRect(ren,&rect);
        SDL_RenderPresent(ren);
    }
safeExit:
    printf("Shutting down...\n");
    if (ren) SDL_DestroyRenderer(ren);
    if (wnd) SDL_DestroyWindow(wnd);
    SDL_Quit();
    return 0;
}