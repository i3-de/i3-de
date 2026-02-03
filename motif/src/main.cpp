#include <argparse/argparse.hpp>
#include <Magick++.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <spawn.h>
#define VERSION "0.1.0-dev"
#define SFACTOR 1.25
#define LFACTOR 1.5

using namespace Magick;

extern char **environ;

double asym_l(Magick::Color color, float Lc) {

    double r = color.quantumRed()/QuantumRange;
    double g = color.quantumGreen()/QuantumRange;
    double b = color.quantumBlue()/QuantumRange;
    
    double maxc = std::max({r,g,b});
    double minc = std::min({r,g,b});
    double C = maxc - minc;
    double segment = (r-g)/C;
    double RYGCP = segment+4;
    double H = RYGCP*60;
    double L = (maxc + minc) * 0.5;
    double Lt = L;
    if (Lc > 1) {
        Lt = L + (1.0-L) * (Lc/100);
    } else if (Lc < 1) {
        Lt = L * (1.0-Lc);
    }
    double S = C/(1-abs(2*L-1));
    double r2, g2, b2;
    if (S == 0) {
        r2 = g2 = b2 = Lt;
    }
    double Llin = (L > 0.0) ? (Lt/L)*100.0 : 100.0;
    return Llin;
}

int main(int argc, char* argv[]) {
    std::string home(std::getenv("HOME"));
    std::string color_path(home + "/.motif.colors");
    std::string fehbg_path(home + "/.fehbg");
    argparse::ArgumentParser parser("motif",VERSION);
    parser.add_description("\
Motif is a program made to take a picture,\n\
set it as the wallpaper, pull colors from it,\n\
and write those to a file.");
    parser.add_argument("file_path")
        .help("The path to the new wallpaper.")
        .default_value("");
    parser.add_argument("--restore")
        .help("Restores the previous wallpaper. (functional)")
        .flag();
    
    try {
        parser.parse_args(argc,argv);
        if (!parser.is_used("file_path") and !parser.is_used("restore")) {
            throw std::runtime_error("Expected either file_path or a functional flag.");
        }
    } catch (const std::exception& err) {
        std::cerr << parser;
        std::cerr << "\n [!] Why didn't my command run?\n      " << err.what() << std::endl;
        return 1;
    }
    pid_t pid;
    if (parser.is_used("restore")) {
        char* feh_argv[] = {
            (char*)fehbg_path.c_str(),
            nullptr
        };
        posix_spawnp(&pid,fehbg_path.c_str(),nullptr,nullptr,feh_argv,environ);
        return 0;
    }
    std::string file_path = parser.get<std::string>("file_path");
    Magick::Image image(file_path);
    image.quantizeColors(5);
    image.quantizeDither(false);
    image.quantize();
    image = image.uniqueColors();
    Magick::Image activeImage = image;
    Magick::Color basecolor = activeImage.pixelColor(3,1); // get the primary color

    // now make the light accents
    Magick::Color lowbright = activeImage.pixelColor(4,1);
    activeImage.modulate(100*LFACTOR,100/SFACTOR,100);
    activeImage.clamp();
    Magick::Color highbright = activeImage.pixelColor(5,1);
    // reset
    activeImage = image;
    // now make the low accents
    Magick::Color highdark = activeImage.pixelColor(2,1);
    Magick::Color lowdark = activeImage.pixelColor(1,1);
    // now output!

    std::ofstream file(color_path);
    if (!file.is_open()) {
        throw std::runtime_error("could not open ~/.motif.colors");
    }
    file
    << "(" << std::to_string((int)(lowdark.quantumRed()/QuantumRange*255)) << "," << std::to_string((int)(lowdark.quantumGreen()/QuantumRange*255)) << "," << std::to_string((int)(lowdark.quantumBlue()/QuantumRange*255)) << ")\n"
    << "(" << std::to_string((int)(highdark.quantumRed()/QuantumRange*255)) << "," << std::to_string((int)(highdark.quantumGreen()/QuantumRange*255)) << "," << std::to_string((int)(highdark.quantumBlue()/QuantumRange*255)) << ")\n"
    << "(" << std::to_string((int)(basecolor.quantumRed()/QuantumRange*255)) << "," << std::to_string((int)(basecolor.quantumGreen()/QuantumRange*255)) << "," << std::to_string((int)(basecolor.quantumBlue()/QuantumRange*255)) << ")\n"
    << "(" << std::to_string((int)(lowbright.quantumRed()/QuantumRange*255)) << "," << std::to_string((int)(lowbright.quantumGreen()/QuantumRange*255)) << "," << std::to_string((int)(lowbright.quantumBlue()/QuantumRange*255)) << ")\n"
    << "(" << std::to_string((int)(highbright.quantumRed()/QuantumRange*255)) << "," << std::to_string((int)(highbright.quantumGreen()/QuantumRange*255)) << "," << std::to_string((int)(highbright.quantumBlue()/QuantumRange*255)) << ")\n";
    file.close();

    char* pk_argv[] = {
        "pkill",
        "feh",
        nullptr
    };
    posix_spawnp(&pid,"pkill",nullptr,nullptr,pk_argv,environ);
    char* feh_argv[] = {
        "feh",
        "--bg-fill",
        (char*)file_path.c_str(),
        nullptr
    };
    posix_spawnp(&pid,"feh",nullptr,nullptr,feh_argv,environ);
    return 0;
}