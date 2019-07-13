#include "marker.hpp"

namespace finder {
    namespace {
        Marker::FlagType getBit(Marker::FlagType value, int index) {
            auto slide = value >> (index * Marker::kFlagValueSize);
            return slide & Marker::kFlagBit;
        }
    }

    bool Marker::calculated(int index) {
        int flagIndex = index / kFlagItemSize;
        auto bits = flags_[flagIndex];
        return getBit(bits, index % kFlagItemSize) != 0;
    }

    void Marker::set(int index, bool succeed) {
        int flagIndex = index / kFlagItemSize;
        int v = succeed ? 0b01 : 0b10;
        flags_[flagIndex] = v << (index * Marker::kFlagValueSize);
    }

    bool Marker::succeed(int index) {
        int flagIndex = index / kFlagItemSize;
        auto bits = flags_[flagIndex];
        return getBit(bits, index % kFlagItemSize) == 0b01;
    }

    bool Marker::failed(int index) {
        int flagIndex = index / kFlagItemSize;
        auto bits = flags_[flagIndex];
        return getBit(bits, index % kFlagItemSize) == 0b10;
    }
}