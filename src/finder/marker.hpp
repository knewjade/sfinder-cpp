#ifndef CORE_MARKER_HPP
#define CORE_MARKER_HPP

#include <cstdint>
#include <vector>
#include <cmath>

namespace finder {
    class Marker {
    public:
        using FlagType = uint64_t;

        static constexpr int kFlagValueSize = 2;
        static constexpr int kFlagItemSize = 64 / kFlagValueSize;

        static constexpr FlagType kFlagBit = (1ULL << kFlagValueSize) - 1;

        static Marker create(int max) {
            int length = ceil(static_cast<double>(max) / kFlagItemSize);
            auto flags_ = std::vector<FlagType>(length);
            return Marker(flags_);
        }

        explicit Marker(std::vector<FlagType> flags) : flags_(std::move(flags)) {
        }

        bool calculated(int index) const;

        bool succeed(int index) const;

        void set(int index, bool succeed);

    private:
        std::vector<FlagType> flags_{};
    };
}

#endif //CORE_MARKER_HPP
