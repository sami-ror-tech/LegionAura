#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "legionaura.h"

static void usage(const char* prog){
    std::cerr <<
      "Usage:\n"
      "  " << prog << " static <Z1> <Z2> <Z3> <Z4> [--brightness 1|2]\n"
      "  " << prog << " breath <Z1> <Z2> <Z3> <Z4> [--speed 1..4] [--brightness 1|2]\n"
      "  " << prog << " wave <ltr|rtl> [--speed 1..4] [--brightness 1|2]\n"
      "  " << prog << " hue [--speed 1..4] [--brightness 1|2]\n"
      "  " << prog << " off\n"
      "Notes: Z1..Z4 are hex colors RRGGBB.\n";
}

int main(int argc, char** argv){
    if (argc < 2){ usage(argv[0]); return 1; }

    std::string cmd = argv[1];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

    uint8_t speed = 1, brightness = 1;
    LAWaveDir wdir = LAWaveDir::None;
    std::array<LAColor,4> zones{LAColor{0,0,0},LAColor{0,0,0},LAColor{0,0,0},LAColor{0,0,0}};
    LAEffect eff;

    int i = 2;
    auto needColor4 = [&](){
        if (i+3 >= argc){ std::cerr << "Need four colors (RRGGBB).\n"; exit(2);}
        for (int k=0;k<4;k++){
            auto c = LegionAura::parseHexRGB(argv[i+k]);
            if (!c){ std::cerr << "Invalid color: " << argv[i+k] << "\n"; exit(2); }
            zones[k] = *c;
        }
        i += 4;
    };

    if (cmd == "static") {
        eff = LAEffect::Static;
        needColor4();
    } else if (cmd == "breath") {
        eff = LAEffect::Breath;
        needColor4();
    } else if (cmd == "wave") {
        eff = LAEffect::Wave;
        if (i >= argc){ std::cerr << "wave requires direction: ltr|rtl\n"; return 2; }
        std::string dir = argv[i++];
        if (dir == "ltr") wdir = LAWaveDir::LTR;
        else if (dir == "rtl") wdir = LAWaveDir::RTL;
        else { std::cerr << "Invalid wave direction: " << dir << "\n"; return 2; }
    } else if (cmd == "hue") {
        eff = LAEffect::Hue;
    } else if (cmd == "off") {
        LegionAura kb;
        if (!kb.open()){ std::cerr << "Device open failed. Are udev rules installed?\n"; return 3; }
        bool ok = kb.off();
        std::cout << (ok ? "OK\n" : "FAIL\n");
        return ok ? 0 : 4;
    } else {
        usage(argv[0]); return 1;
    }

    // parse optional flags
    while (i < argc){
        std::string f = argv[i++];
        if (f == "--speed" && i<argc) {
            speed = (uint8_t)std::stoi(argv[i++]);
            if (speed<1 || speed>4){ std::cerr << "speed must be 1..4\n"; return 2; }
        } else if (f == "--brightness" && i<argc) {
            brightness = (uint8_t)std::stoi(argv[i++]);
            if (brightness<1 || brightness>2){ std::cerr << "brightness must be 1 or 2\n"; return 2; }
        } else {
            std::cerr << "Unknown arg: " << f << "\n"; return 2;
        }
    }

    LAParams p{eff, speed, brightness, zones, wdir};

    LegionAura kb;
    if (!kb.open()){ std::cerr << "Device open failed. Are udev rules installed?\n"; return 3; }
    bool ok = kb.apply(p);
    std::cout << (ok ? "OK\n" : "FAIL\n");
    return ok ? 0 : 4;
}
