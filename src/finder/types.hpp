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
        bool alwaysRegularAttack;
        uint8_t lastHoldPriority;  // 0bEOZSJLIT // 0b11000000 -> Give high priority to solutions that last hold is Empty,O
    };

    struct Operation {
        core::PieceType pieceType;
        core::RotateType rotateType;
        int x;
        int y;
    };

    template<typename C>
    struct PreOperation {
        C candidate;
        core::PieceType pieceType;
        core::RotateType rotateType;
        int x;
        int y;
    };

    using Solution = std::vector<Operation>;
    inline const Solution kNoSolution = std::vector<Operation>();

    // For fast search
    struct FastCandidate {
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
        // new
        Solution solution;
        core::PieceType hold;
        int holdPriority;  // Priority is given when the least significant bit is 1  // 0bEOZSJLIT

        // from candidate
        int currentIndex;
        int holdIndex;
        int leftLine;
        int depth;
        int softdropCount;
        int holdCount;
        int lineClearCount;
        int currentCombo;
        int maxCombo;
    };

    // For T-Spin search
    struct TSpinCandidate {
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
        // new
        Solution solution;
        core::PieceType hold;
        int holdPriority;  // Priority is given when the least significant bit is 1  // 0bEOZSJLIT

        // from candidate
        int currentIndex;
        int holdIndex;
        int leftLine;
        int depth;
        int softdropCount;
        int holdCount;
        int lineClearCount;
        int currentCombo;
        int maxCombo;
        int tSpinAttack;
        bool b2b;
        int leftNumOfT;
    };

    // For all spins search
    struct AllSpinsCandidate {
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
        // new
        Solution solution;
        core::PieceType hold;
        int holdPriority;  // Priority is given when the least significant bit is 1  // 0bEOZSJLIT

        // candidate
        int currentIndex;
        int holdIndex;
        int leftLine;
        int depth;
        int softdropCount;
        int holdCount;
        int lineClearCount;
        int currentCombo;
        int maxCombo;
        int spinAttack;
        bool b2b;
    };
}

#endif //FINDER_TYPES_HPP
