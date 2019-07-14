#include "marker.hpp"

namespace finder {
    namespace {
        Marker::FlagType getBit(Marker::FlagType value, int index) {
            auto slide = value >> (index * Marker::kFlagValueSize);
            return slide & Marker::kFlagBit;
        }
    }

    bool Marker::calculated(int index) const {
        assert(0 <= index && index < flags_.size() * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        auto bits = flags_[flagIndex];
        return getBit(bits, index % kFlagItemSize) != 0;
    }

    void Marker::set(int index, bool succeed) {
        assert(0 <= index && index < flags_.size() * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        Marker::FlagType v = succeed ? 0b11 : 0b01;
        flags_[flagIndex] |= v << (index * Marker::kFlagValueSize);
    }

    bool Marker::succeed(int index) const {
        assert(0 <= index && index < flags_.size() * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        auto bits = flags_[flagIndex];
        return getBit(bits, index % kFlagItemSize) == 0b11;
    }
}