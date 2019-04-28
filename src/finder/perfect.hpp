#ifndef CORE_PERFECT_HPP
#define CORE_PERFECT_HPP

#include "../core/piece.hpp"
#include "../core/moves.hpp"
#include "../core/types.hpp"

namespace finder {
    struct Candidate {
        const core::Field &field;
        const int currentIndex;
        const int holdIndex;
        const int leftLine;
        const int depth;
    };

    struct Configure {
        const std::vector<core::PieceType> &pieces;
        const std::vector<std::vector<core::Move>> &movePool;
        const int maxDepth;
        const int pieceSize;
        int moveCounter;
    };

    // T: srs::MoveGenerator
    template<class T>
    class PerfectFinder {
    public:
        PerfectFinder<T>(const core::Factory &factory, T &moveGenerator)
                : factory(factory), moveGenerator(moveGenerator) {
        }

        bool run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty
        );

        bool search(Configure &configure, Candidate &candidate);

    private:
        const core::Factory &factory;
        T &moveGenerator;
    };
}

#endif //CORE_PERFECT_HPP
