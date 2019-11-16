#ifndef FINDER_TYPES_HPP
#define FINDER_TYPES_HPP

#include "../core/moves.hpp"
#include "../core/types.hpp"

namespace finder {
    struct Configure {
        const std::vector<core::PieceType> &pieces;
        std::vector<std::vector<core::Move>> &movePool;
        const int maxDepth;
        const int pieceSize;
        const bool leastLineClears;
    };

    struct Operation {
        core::PieceType pieceType;
        core::RotateType rotateType;
        int x;
        int y;
    };

    using Solution = std::vector<Operation>;
    inline const Solution kNoSolution = std::vector<Operation>();

    struct TSpinCandidate {
        const core::Field &field;
        const int currentIndex;
        const int holdIndex;
        const int leftLine;
        const int depth;
        const int softdropCount;
        const int holdCount;
        const int lineClearCount;
        const int currentCombo;
        const int maxCombo;
        const int tSpinAttack;
        const bool b2b;
        const int leftNumOfT;
    };

    struct TSpinRecord {
        Solution solution;
        int softdropCount;
        int holdCount;
        int lineClearCount;
        int maxCombo;
        int tSpinAttack;
    };
}

#endif //FINDER_TYPES_HPP
