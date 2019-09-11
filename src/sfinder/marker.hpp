#ifndef FINDER_MARKER_HPP
#define FINDER_MARKER_HPP

#include <cstdint>
#include <vector>
#include <cmath>

namespace sfinder {
    class Marker {
    public:
        using FlagType = uint64_t;

        static constexpr int kFlagValueSize = 2;
        static constexpr int kFlagItemSize = 64 / kFlagValueSize;

        static constexpr FlagType kFlagBit = (1ULL << static_cast<unsigned int>(kFlagValueSize)) - 1;

        static Marker create(int max) {
            auto length = static_cast<int>(ceil(static_cast<double>(max) / kFlagItemSize));
            auto flags_ = std::vector<FlagType>(length);
            return Marker(flags_);
        }

        explicit Marker(std::vector<FlagType> flags) : flags_(std::move(flags)) {
        }

        bool calculated(int index) const;

        bool succeed(int index) const;

        void set(int index, bool succeed);

        const std::vector<FlagType> &flags() const {
            return flags_;
        }

    private:
        std::vector<FlagType> flags_{};
    };

    class Bits {
    public:
        using FlagType = uint64_t;

        static constexpr int kFlagValueSize = 1;
        static constexpr int kFlagItemSize = 64 / kFlagValueSize;

        static constexpr FlagType kFlagBit = (1ULL << static_cast<unsigned int>(kFlagValueSize)) - 1;

        static int calculateLength(int max) {
            constexpr int size = kFlagItemSize;
            if (max % size == 0) {
                return max / size;
            }
            return (max / size) + 1;
        }

        static Bits create(int max) {
            auto length = calculateLength(max);
            auto flags_ = std::vector<FlagType>(length);
            return Bits(flags_);
        }

        explicit Bits(std::vector<FlagType> flags) : flags_(std::move(flags)) {
        }

        bool checks(int index) const;

        void set(int index, bool succeed);

        const std::vector<FlagType> &flags() const {
            return flags_;
        }

    private:
        std::vector<FlagType> flags_{};
    };
}

#endif //FINDER_MARKER_HPP
