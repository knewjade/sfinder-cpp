#ifndef CORE_PERFECT_HPP
#define CORE_PERFECT_HPP

#include "../core/piece.hpp"
#include "../core/moves.hpp"

namespace finder {
    struct Candidate {
        core::Field &field;
        int currentIndex;
        int holdIndex;
        int leftLine;
        int depth;
    };

    struct Configure {
        std::vector<core::PieceType> &pieces;
        std::vector<std::vector<core::Move>> &movePool;
        int maxDepth;
        int pieceSize;
    };

    // T: srs::MoveGenerator
    template<class T>
    class PerfectFinder {
    public:
        PerfectFinder<T>(const core::Factory &factory, T &moveGenerator)
                : factory(factory), moveGenerator(moveGenerator) {
        }

        bool run(
                const core::Field &field, std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty
        );

        bool search(Configure &configure, Candidate &candidate);

    private:
        const core::Factory &factory;
        T &moveGenerator;
    };
}

#endif //CORE_PERFECT_HPP
