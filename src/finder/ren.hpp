#ifndef CORE_REN_HPP
#define CORE_REN_HPP

#include "./types.hpp"
#include "../core/factory.hpp"
#include "../core/moves.hpp"

namespace finder::ren {
    struct Candidate {
        const core::Field &field;
        const int currentIndex;
        const int holdIndex;
        const int depth;
        const int softdropCount;
        const int holdCount;
        const int currentCombo;
    };

    struct Configure {
        const std::vector<core::PieceType> &pieces;
        std::vector<std::vector<core::Move>> &movePool;
        const int pieceSize;
    };

    struct Record {
        Solution solution;
        int softdropCount;
        int holdCount;
        int maxCombo;
    };

    template<class T = core::srs::MoveGenerator>
    class Finder {
    public:
        Finder<T>(const core::Factory &factory, T &moveGenerator) : factory(factory), moveGenerator(moveGenerator) {
        }

        int run(const core::Field &field, const std::vector<core::PieceType> &pieces, bool holdEmpty);

    private:
        const core::Factory &factory;
        T &moveGenerator;
        Record best;

        void search(const Configure &configure, const Candidate &candidate, Solution &solution);

        void move(
                const Configure &configure,
                const Candidate &candidate,
                Solution &solution,
                std::vector<core::Move> &moves,
                core::PieceType pieceType,
                int nextIndex,
                int nextHoldIndex,
                int nextHoldCount
        );

        void accept(const Configure &configure, const Record &record);
    };
}

#endif //CORE_REN_HPP
