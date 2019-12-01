#ifndef FINDER_PERFECT_CLEAR_TYPES_HPP
#define FINDER_PERFECT_CLEAR_TYPES_HPP

#include "../../core/moves.hpp"
#include "../../core/types.hpp"

namespace sfinder::perfect_clear {
    struct Configure {
        const std::vector<core::PieceType> &pieces;
        std::vector<std::vector<core::Move>> &movePool;
        const int maxDepth;
        const int pieceSize;
        const bool leastLineClears;
        bool alwaysRegularAttack;
    };

    struct Operation {
        core::PieceType pieceType;
        core::RotateType rotateType;
        int x;
        int y;
    };

    using Solution = std::vector<Operation>;
    inline const Solution kNoSolution = std::vector<Operation>();

    // For PC-able search
    struct PCableCandidate {
        const core::Field &field;
        const int currentIndex;
        const int holdIndex;
        const int leftLine;
        const int depth;
    };

    struct PCableRecord {
        Solution solution;
    };

    // For fast search
    struct FastCandidate {
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
    };

    struct FastRecord {
        Solution solution;
        int softdropCount;
        int holdCount;
        int lineClearCount;
        int maxCombo;
    };

    // For T-Spin search
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

    // For all spins search
    struct AllSpinsCandidate {
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
        const int spinAttack;
        const bool b2b;
    };

    struct AllSpinsRecord {
        Solution solution;
        int softdropCount;
        int holdCount;
        int lineClearCount;
        int maxCombo;
        int spinAttack;
    };
}

#endif //FINDER_PERFECT_CLEAR_TYPES_HPP
