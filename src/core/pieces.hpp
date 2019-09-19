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
        template<class Container>
        static Pieces create(const Container &pieces) {
            return Pieces::create(pieces.cbegin(), pieces.cend());
        }

        template<class InputIterator>
        static Pieces create(
                InputIterator begin,
                typename is_input_iterator<InputIterator, core::PieceType>::type end
        ) {
            std::reverse_iterator<InputIterator> first(end);
            std::reverse_iterator<InputIterator> last(begin);

            auto value = 0ULL;
            auto size = 0;
            for (auto it = first; it != last; ++it) {
                value *= 7ULL;
                value += *it;
                size += 1;
            }
            return Pieces(value, size);
        }

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

    constexpr auto kSlideMask = std::array<uint64_t, 7>{
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
            return PieceCounter::create(pieces.cbegin(), pieces.cend());
        }

        template<int N>
        static PieceCounter create(const std::array<core::PieceType, N> &pieces) {
            return PieceCounter::create(pieces.cbegin(), pieces.cend());
        }

        template<class InputIterator>
        static PieceCounter create(
                InputIterator begin,
                typename is_input_iterator<InputIterator, core::PieceType>::type end
        ) {
            auto value = 0ULL;
            for (auto it = begin; it != end; ++it) {
                value += kSlideMask[*it];
            }
            return PieceCounter(value);
        }

        explicit PieceCounter(uint64_t value) : value_(value) {
            assert((value & 0x80808080808080ULL) == 0L);
        }

        std::vector<core::PieceType> vector() const;

        int size() const;

        bool containsAll(PieceCounter other) const;

        uint64_t value() const {
            return value_;
        }

        bool empty() const {
            return value_ == 0ULL;
        }

        int operator[](PieceType pieceType) const {
            auto bit = value_ >> (8U * pieceType);
            return bit & 0b1111111U;
        }

        PieceCounter operator-(PieceCounter other) const {
            auto difference = value_ - other.value_;
            return PieceCounter(difference);
        }

        PieceCounter operator+(PieceCounter other) const {
            auto difference = value_ + other.value_;
            return PieceCounter(difference);
        }

        PieceCounter operator+(PieceType pieceType) const {
            auto difference = value_ + kSlideMask[pieceType];
            return PieceCounter(difference);
        }

    private:
        uint64_t value_;  // const
    };
}

#endif //CORE_PIECES_HPP
