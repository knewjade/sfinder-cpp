#ifndef FINDER_PERFECT_CLEAR_TYPES_HPP
#define FINDER_PERFECT_CLEAR_TYPES_HPP

namespace sfinder::perfect_clear {
    struct Configure {
        const std::vector<core::PieceType> &pieces;
        std::vector<std::vector<core::Move>> &movePool;
        const int maxDepth;
        const int pieceSize;
    };

    inline bool validate(const core::Field &field, int maxLine) {
        int sum = maxLine - field.getBlockOnX(0, maxLine);
        for (int x = 1; x < core::kFieldWidth; x++) {
            int emptyCountInColumn = maxLine - field.getBlockOnX(x, maxLine);
            if (field.isWallBetween(x, maxLine)) {
                if (sum % 4 != 0)
                    return false;
                sum = emptyCountInColumn;
            } else {
                sum += emptyCountInColumn;
            }
        }

        return sum % 4 == 0;
    }
}

#endif //FINDER_PERFECT_CLEAR_TYPES_HPP
