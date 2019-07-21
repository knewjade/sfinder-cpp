#include <cassert>

#include "marker.hpp"

namespace sfinder {
    namespace {
        Marker::FlagType getBit(Marker::FlagType value, int index) {
            auto slide = value >> static_cast<unsigned int>(index * Marker::kFlagValueSize);
            return slide & Marker::kFlagBit;
        }
    }

    bool Marker::calculated(int index) const {
        assert(0 <= index && index < static_cast<int>(flags_.size()) * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        auto bits = flags_[flagIndex];
        return getBit(bits, index % kFlagItemSize) != 0ULL;
    }

    void Marker::set(int index, bool succeed) {
        assert(0 <= index && index < static_cast<int>(flags_.size()) * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        Marker::FlagType v = succeed ? 0b11ULL : 0b01ULL;
        flags_[flagIndex] |= v << ((index % kFlagItemSize) * Marker::kFlagValueSize);
    }

    bool Marker::succeed(int index) const {
        assert(0 <= index && index < static_cast<int>(flags_.size()) * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        auto bits = flags_[flagIndex];
        return getBit(bits, index % kFlagItemSize) == 0b11ULL;
    }
}