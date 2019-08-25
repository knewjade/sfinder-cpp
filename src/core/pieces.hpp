#ifndef CORE_PIECES_HPP
#define CORE_PIECES_HPP

#include <cassert>
#include <vector>
#include <array>
#include <list>

#include "types.hpp"

namespace core {
    class Pieces {
    public:
        static Pieces create(const std::vector<core::PieceType> &pieces);

        static Pieces create(const std::list<core::PieceType> &pieces);

        Pieces(uint64_t value, int size) : value_(value), size_(size) {
        }

        std::vector<core::PieceType> vector() const;

        uint64_t value() const {
            return value_;
        }

        uint64_t size() const {
            return size_;
        }

    private:
        const uint64_t value_;
        const int size_;
    };

    constexpr std::array<uint64_t, 7> kSlideMask = std::array<uint64_t, 7>{
            1ULL << 8U * 0,
            1ULL << 8U * 1,
            1ULL << 8U * 2,
            1ULL << 8U * 3,
            1ULL << 8U * 4,
            1ULL << 8U * 5,
            1ULL << 8U * 6,
    };

    class PieceCounter {
    public:
        static PieceCounter create() {
            return PieceCounter(0ULL);
        }

        static PieceCounter create(core::PieceType piece) {
            return PieceCounter(kSlideMask[piece]);
        }

        static PieceCounter create(const std::vector<core::PieceType> &pieces) {
            auto value = 0ULL;
            for (auto piece : pieces) {
                value += kSlideMask[piece];
            }
            return PieceCounter(value);
        }

        template<int N>
        static PieceCounter create(const std::array<core::PieceType, N> &pieces) {
            auto value = 0ULL;
            for (auto piece : pieces) {
                value += kSlideMask[piece];
            }
            return PieceCounter(value);
        }

        explicit PieceCounter(uint64_t value) : value_(value) {
        }

        std::vector<core::PieceType> vector() const;

        bool containsAll(PieceCounter other) const;

        uint64_t value() const {
            return value_;
        }

        int operator[](PieceType pieceType) const {
            auto bit = value_ >> (8U * pieceType);
            return bit & 0b1111111U;
        }

        PieceCounter operator-(PieceCounter other) const {
            auto difference = value_ - other.value_;
            assert((difference & 0x80808080808080ULL) == 0L);
            return PieceCounter(difference);
        }

    private:
        const uint64_t value_;
    };
}

#endif //CORE_PIECES_HPP
