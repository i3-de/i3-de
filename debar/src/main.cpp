#include <SDL3_gfx/SDL3_gfxPrimitives.h>
#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <cstdint>
#include "xsdl.h"
#include "const.h"

struct deColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a = 0;
};

struct Config {
    // Colors

    // What the base color of the bar is.
    deColor barColor;
    // What the background color of the widgets will be.
    deColor widgetColor;
    // What the text color will be.
    deColor textColor;
    // What color will be used in the case of a widget failing.
    //
    // (e.g. no WiFi, urgent notifications)
    deColor failureColor;
    // What color will be used in the case of a widget processing something.
    //
    // (e.g. progress notification)
    deColor progressColor;

    // Numbers

    // how much the outside of the bar is rounded
    uint16_t outerRound;
    // how much the inside of the bar is rounded
    uint16_t innerRound;
    // outer gaps set in i3wm config
    uint16_t outerGaps;
    // height of the bar in pixels
    uint16_t barHeight;
};

deColor parseJsonColor(nlohmann::json object) {
    return {object[0],object[1],object[2],object[3]};
}

void SDL_SetRenderDrawColor(SDL_Renderer* renderer, deColor color) {
    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
}

int main(int argc, char* argv[]) {
    bool running = false;
    SDL_Window* wnd;
    SDL_Renderer* ren;
    SDL_Event e;
    SDL_Rect bounds;
    int trueBarHeight;
    int barWidth;
    int barHeight;
    SDL_PropertiesID props;
    Display* xdisplay;
    Window xwindow;
    long strut[12] = {0};
    Atom net_wm_strut_partial;
    Atom wm_type;
    Atom wm_dock;
    Config configuration = {};
    std::ifstream config_file;
    nlohmann::json config_data;


    std::string homedir(std::getenv("HOME"));
    argparse::ArgumentParser parser("motif",VERSION);
    parser.add_description("\
Debar is a drop-in replacement for most bar\n\
applications, such as i3bar and polybar.\n\
\n\
It currently only supports X11, however, \n\
because I (cookiiq) find wayland too... \n\
incomplete. Lots of stuff do not work on\n\
wayland, or are impossible, or are far more\n\
difficult to use. Sorry if you like wayland!");
    parser.add_argument("-c","--config")
        .default_value(homedir + "/.config/debar.conf")
        .help("Sets the config location.");

    try {
        parser.parse_args(argc,argv);
    } catch (const std::exception& err) {
        std::cerr << parser;
        std::cerr << "\n [!] Why didn't my command run?\n      " << err.what() << std::endl;
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        // no need to `goto safeExit` here; nothing is initialized.
        return 1;
    }
    config_file = std::ifstream(parser.get("config"));
    config_data = nlohmann::json::parse(config_file);
    configuration.barColor = parseJsonColor(config_data["barColor"]);
    configuration.widgetColor = parseJsonColor(config_data["widgetColor"]);
    configuration.textColor = parseJsonColor(config_data["textColor"]);
    configuration.failureColor = parseJsonColor(config_data["failureColor"]);
    configuration.progressColor = parseJsonColor(config_data["progressColor"]);
    configuration.outerRound = config_data["round"]["outer"];
    configuration.innerRound = config_data["round"]["inner"];
    configuration.outerGaps = config_data["outerGaps"];
    configuration.barHeight = config_data["barHeight"];

    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    bounds = {};
    if (!SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(),&bounds)) {
        std::cerr << "SDL_GetDisplayBounds Error: " << SDL_GetError() << std::endl;
        goto safeExit;
        return 1;
    }
    barWidth = bounds.w;
    trueBarHeight = configuration.barHeight;
    barHeight = trueBarHeight+configuration.outerGaps+1;
    SDL_SetHint(SDL_HINT_X11_WINDOW_TYPE,"_NET_WM_WINDOW_TYPE_DOCK");
    wnd = SDL_CreateWindow(
       "debar",
       barWidth,barHeight,
       SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_HIDDEN | SDL_WINDOW_TRANSPARENT
    );
    if (!wnd) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        goto safeExit;
        return 2;
    }
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
    strut[2] = barHeight;
    strut[8] = 0;
    strut[9] = barWidth-1;
    net_wm_strut_partial = XInternAtom(xdisplay, "_NET_WM_STRUT_PARTIAL", False);
    XChangeProperty( // set strut ;; reserve space on the top for my bar
        xdisplay,
        xwindow,
        net_wm_strut_partial,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)strut,
        12
    );
    XSync(xdisplay, False);
    SDL_ShowWindow(wnd);
    // SDL_SetWindowPosition(wnd,0,25);
    // SDL_SyncWindow(wnd);
    ren = SDL_CreateRenderer(
        wnd,
        NULL
    );
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
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
        SDL_SetRenderDrawColor(ren,0,0,0,0);
        SDL_RenderClear(ren);
        SDL_FRect rect = {(20)/2,(float)configuration.outerGaps+1,barWidth-20,22};
        SDL_SetRenderDrawColor(ren,configuration.barColor);
        XSDL_RenderFillRectRound(ren,5,&rect);
        rect = {(float)(barWidth-154)/2,(float)configuration.outerGaps+1,154,22};
        SDL_SetRenderDrawColor(ren,configuration.widgetColor);
        XSDL_RenderFillRectRound(ren,5+2,&rect);
        
        SDL_RenderPresent(ren);
    }
safeExit:
    printf("Shutting down...\n");
    if (ren) SDL_DestroyRenderer(ren);
    if (wnd) SDL_DestroyWindow(wnd);
    SDL_Quit();
    return 0;
}