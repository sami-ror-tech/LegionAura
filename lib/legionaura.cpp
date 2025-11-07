#include "legionaura.h"
#include <libusb-1.0/libusb.h>
#include <algorithm>
#include <cctype>

static uint8_t clampByte(int v){ return (uint8_t)std::max(0,std::min(255,v)); }

LegionAura::LegionAura(uint16_t vid, uint16_t pid) : vid_(vid), pid_(pid) {}
LegionAura::~LegionAura(){ close(); }

bool LegionAura::open() {
    if (ctx_) return true;
    if (libusb_init(&ctx_) != 0) return false;
    dev_ = libusb_open_device_with_vid_pid(ctx_, vid_, pid_);
    if (!dev_) { libusb_exit(ctx_); ctx_=nullptr; return false; }
    if (libusb_kernel_driver_active(dev_, iface_) == 1) libusb_detach_kernel_driver(dev_, iface_);
    if (libusb_claim_interface(dev_, iface_) != 0) { libusb_close(dev_); dev_=nullptr; libusb_exit(ctx_); ctx_=nullptr; return false; }
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
    LAParams p{LAEffect::Static, 1, 1, {}, LAWaveDir::None};
    // zero all colors to turn off
    p.zones = {LAColor{0,0,0}, LAColor{0,0,0}, LAColor{0,0,0}, LAColor{0,0,0}};
    return apply(p);
}

std::vector<uint8_t> LegionAura::buildPayload(const LAParams& p) {
    // Mirrors the Python script:
    // [0]=0xCC, [1]=0x16, [2]=effect, [3]=speed, [4]=brightness,
    // [5..16]=zone1..zone4 RGB (only for static/breath), else 12 zeros,
    // [17]=unused 0, [18]=RTL (1/0), [19]=LTR (0/1), [20..] pad zeros to 32 bytes total payload
    std::vector<uint8_t> d;
    d.reserve(32);
    d.push_back(0xCC);
    d.push_back(0x16);
    d.push_back(static_cast<uint8_t>(p.effect));
    d.push_back(std::max<uint8_t>(1, std::min<uint8_t>(4, p.speed)));
    d.push_back(std::max<uint8_t>(1, std::min<uint8_t>(2, p.brightness)));

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

    d.push_back(0x00); // unused
    // wave direction flags
    uint8_t rtl = 0, ltr = 0;
    if (p.effect == LAEffect::Wave) {
        if (p.waveDir == LAWaveDir::RTL) rtl = 1;
        else if (p.waveDir == LAWaveDir::LTR) ltr = 1;
    }
    d.push_back(rtl);
    d.push_back(ltr);

    // pad to 32 bytes payload (the control transfer will send exactly these)
    if (d.size() < 32) d.insert(d.end(), 32 - d.size(), 0x00);
    return d;
}

bool LegionAura::ctrlSendCC(const std::vector<uint8_t>& data) {
    if (!dev_) return false;
    // This replicates: bmRequestType=0x21, bRequest=0x09, wValue=0x03CC, wIndex=0
    // Send exactly the payload (no extra padding by host)
    int r = libusb_control_transfer(
        dev_,
        0x21,      // Host-to-device | Class | Interface
        0x09,      // SET_REPORT
        0x03CC,    // wValue = (Feature << 8) | ReportID(0xCC)
        0x0000,    // wIndex = interface 0
        const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(data.data())),
        (uint16_t)data.size(),
        1000
    );
    return r == (int)data.size();
}

std::optional<LAColor> LegionAura::parseHexRGB(const std::string& s) {
    if (s.size()!=6) return std::nullopt;
    auto hex2 = [&](char a, char b)->std::optional<uint8_t>{
        auto hv=[&](char c)->int{
            if (std::isdigit((unsigned char)c)) return c - '0';
            if (std::isxdigit((unsigned char)c)) return 10 + (std::tolower((unsigned char)c) - 'a');
            return -1;
        };
        int hi=hv(a), lo=hv(b);
        if (hi<0 || lo<0) return std::nullopt;
        return (uint8_t)((hi<<4)|lo);
    };
    auto r=hex2(s[0],s[1]); auto g=hex2(s[2],s[3]); auto b=hex2(s[4],s[5]);
    if (!r||!g||!b) return std::nullopt;
    return LAColor{*r,*g,*b};
}
