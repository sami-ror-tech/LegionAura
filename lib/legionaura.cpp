#include "legionaura.h"
#include <libusb-1.0/libusb.h>
#include <algorithm>
#include <cctype>

LegionAura::LegionAura(uint16_t vid, uint16_t pid) : vid_(vid), pid_(pid) {}
LegionAura::~LegionAura(){ close(); }

bool LegionAura::open() {
    if (ctx_) return true;
    if (libusb_init(&ctx_) != 0) return false;

    dev_ = libusb_open_device_with_vid_pid(ctx_, vid_, pid_);
    if (!dev_) { libusb_exit(ctx_); ctx_=nullptr; return false; }

    if (libusb_kernel_driver_active(dev_, iface_) == 1)
        libusb_detach_kernel_driver(dev_, iface_);

    if (libusb_claim_interface(dev_, iface_) != 0) {
        libusb_close(dev_);
        dev_=nullptr;
        libusb_exit(ctx_);
        ctx_=nullptr;
        return false;
    }
    return true;
}

void LegionAura::close() {
    if (!ctx_) return;
    if (dev_) {
        libusb_release_interface(dev_, iface_);
        libusb_close(dev_);
        dev_ = nullptr;
    }
    libusb_exit(ctx_);
    ctx_ = nullptr;
}

bool LegionAura::apply(const LAParams& p) {
    auto payload = buildPayload(p);
    return ctrlSendCC(payload);
}

bool LegionAura::off() {
    LAParams p{LAEffect::Static,1,1,
               {LAColor{0,0,0},LAColor{0,0,0},LAColor{0,0,0},LAColor{0,0,0}},
               LAWaveDir::None};
    return apply(p);
}

std::vector<uint8_t> LegionAura::buildPayload(const LAParams& p) {

    std::vector<uint8_t> d;
    d.reserve(32);

    d.push_back(0xCC);
    d.push_back(0x16);

    // --------------------------------------
    // BRIGHTNESS ONLY (effect = None)
    // --------------------------------------
    if (p.effect == LAEffect::None) {
        d.push_back(0x01); // static (doesn't change)
        d.push_back(0x01); // speed ignored
        d.push_back(p.brightness);
        d.insert(d.end(), 12+1+2, 0x00);
        if (d.size() < 32) d.insert(d.end(), 32 - d.size(), 0x00);
        return d;
    }

    d.push_back((uint8_t)p.effect);
    d.push_back(p.speed);
    d.push_back(p.brightness);

    bool needsColors = (p.effect == LAEffect::Static || p.effect == LAEffect::Breath);

    if (needsColors) {
        for (int i=0;i<4;i++){
            d.push_back(p.zones[i].r);
            d.push_back(p.zones[i].g);
            d.push_back(p.zones[i].b);
        }
    } else {
        d.insert(d.end(), 12, 0x00);
    }

    d.push_back(0x00);
    uint8_t rtl=0,ltr=0;

    if (p.effect == LAEffect::Wave) {
        if (p.waveDir == LAWaveDir::RTL) rtl = 1;
        else if (p.waveDir == LAWaveDir::LTR) ltr = 1;
    }

    d.push_back(rtl);
    d.push_back(ltr);

    if (d.size() < 32) d.insert(d.end(), 32 - d.size(), 0x00);
    return d;
}

bool LegionAura::ctrlSendCC(const std::vector<uint8_t>& data) {
    if (!dev_) return false;
    int r = libusb_control_transfer(
        dev_,
        0x21,
        0x09,
        0x03CC,
        0x0000,
        (unsigned char*)data.data(),
        (uint16_t)data.size(),
        1000
    );
    return r == (int)data.size();
}

std::optional<LAColor> LegionAura::parseHexRGB(const std::string& s) {
    if (s.size()!=6) return std::nullopt;

    auto hex = [&](char a)->int{
        if (std::isdigit((unsigned char)a)) return a - '0';
        if (std::isxdigit((unsigned char)a))
            return 10 + (std::tolower((unsigned char)a) - 'a');
        return -1;
    };

    int rhi=hex(s[0]), rlo=hex(s[1]);
    int ghi=hex(s[2]), glo=hex(s[3]);
    int bhi=hex(s[4]), blo=hex(s[5]);

    if (rhi<0||rlo<0||ghi<0||glo<0||bhi<0||blo<0) return std::nullopt;

    return LAColor{
        (uint8_t)((rhi<<4)|rlo),
        (uint8_t)((ghi<<4)|glo),
        (uint8_t)((bhi<<4)|blo)
    };
}
