#include "pieces.hpp"

namespace core {
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
        for (const auto &pieceType : core::kAllPieceType) {
            int size = operator[](pieceType);
            for (int count = 0; count < size; ++count) {
                vector.emplace_back(pieceType);
            }
        }
        return vector;
    }

    int PieceCounter::size() const {
        int size = 0;
        for (const auto &pieceType : core::kAllPieceType) {
            size += operator[](pieceType);
        }
        return size;
    }

    bool PieceCounter::containsAll(PieceCounter other) const {
        auto difference = value_ - other.value_;
        // True if the most significant bit of each block is 1 (carry-off) has not occurred
        return (difference & 0x80808080808080ULL) == 0L;
    }
}
