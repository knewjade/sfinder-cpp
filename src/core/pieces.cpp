#include "pieces.hpp"

namespace core {
    Pieces Pieces::create(const std::vector<core::PieceType> &pieces) {
        auto value = 0ULL;
        auto size = pieces.size();
        for (int index = size - 1; 0 <= index; index--) {
            value *= 7ULL;
            value += pieces[index];
        }
        return Pieces(value, size);
    }

    Pieces Pieces::create(const std::list<core::PieceType> &pieces) {
        auto value = 0ULL;
        auto size = pieces.size();
        for (auto it = pieces.crbegin(); it != pieces.crend(); ++it) {
            value *= 7ULL;
            value += *it;
        }
        return Pieces(value, size);
    }

    std::vector<core::PieceType> Pieces::vector() const {
        auto vector = std::vector<core::PieceType>(size_);
        auto value = value_;
        for (int index = 0; index < size_; index++) {
            vector[index] = static_cast<core::PieceType>(value % 7);
            value /= 7ULL;
        }
        assert(value == 0);
        return vector;
    }

    std::vector<core::PieceType> PieceCounter::vector() const {
        auto vector = std::vector<core::PieceType>{};
        for (int piece = 0; piece < 7; ++piece) {
            auto type = static_cast<core::PieceType>(piece);
            int size = operator[](type);
            for (int count = 0; count < size; ++count) {
                vector.emplace_back(type);
            }
        }
        return vector;
    }

    bool PieceCounter::containsAll(PieceCounter other) const {
        auto difference = value_ - other.value_;
        // True if the most significant bit of each block is 1 (carry-off) has not occurred
        return (difference & 0x80808080808080ULL) == 0L;
    }
}
