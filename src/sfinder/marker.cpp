#include <cassert>

#include "marker.hpp"

namespace sfinder {
    namespace {
        template<int size, uint64_t bit>
        Marker::FlagType getBit(Marker::FlagType value, int index) {
            auto slide = value >> static_cast<unsigned int>(index * size);
            return slide & bit;
        }
    }

    bool Marker::calculated(int index) const {
        assert(0 <= index && index < static_cast<int>(flags_.size()) * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        auto bits = flags_[flagIndex];
        return getBit<kFlagValueSize, kFlagBit>(bits, index % kFlagItemSize) != 0ULL;
    }

    void Marker::set(int index, bool succeed) {
        assert(0 <= index && index < static_cast<int>(flags_.size()) * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        Marker::FlagType v = succeed ? 0b11ULL : 0b01ULL;
        flags_[flagIndex] |= v << static_cast<unsigned int>((index % kFlagItemSize) * kFlagValueSize);
    }

    bool Marker::succeed(int index) const {
        assert(0 <= index && index < static_cast<int>(flags_.size()) * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        auto bits = flags_[flagIndex];
        return getBit<kFlagValueSize, kFlagBit>(bits, index % kFlagItemSize) == 0b11ULL;
    }

    bool Bits::checks(int index) const {
        assert(0 <= index && index < static_cast<int>(flags_.size()) * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        auto bits = flags_[flagIndex];
        return getBit<kFlagValueSize, kFlagBit>(bits, index % kFlagItemSize) != 0ULL;
    }

    void Bits::set(int index, bool succeed) {
        assert(0 <= index && index < static_cast<int>(flags_.size()) * kFlagItemSize);
        int flagIndex = index / kFlagItemSize;
        if (succeed) {
            flags_[flagIndex] |= 0b1ULL << static_cast<unsigned int>((index % kFlagItemSize) * kFlagValueSize);
        } else {
            flags_[flagIndex] &= ~(0b1ULL << static_cast<unsigned int>((index % kFlagItemSize) * kFlagValueSize));
        }
    }
}