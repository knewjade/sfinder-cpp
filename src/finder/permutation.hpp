#include <utility>

#ifndef CORE_COMBINATIONS_HPP
#define CORE_COMBINATIONS_HPP

#include <array>
#include <vector>

#include "../core/types.hpp"

namespace finder {
    class Permutation {
    public:
        template<int N>
        static Permutation create(const std::array<core::PieceType, N> &pieces, int pop) {
            return Permutation(std::vector<core::PieceType>(pieces.begin(), pieces.end()), pop);
        }

        static Permutation create(const std::vector<core::PieceType> &pieces, int pop) {
            return Permutation(pieces, pop);
        }

        Permutation(std::vector<core::PieceType> pieces, int pop) : pieces_(std::move(pieces)), pop_(pop) {
            assert(0 < pop);
            assert(pop <= pieces_.size());
        }

        int size();

        std::vector<core::PieceType> pieces(int index);

        void push(std::vector<core::PieceType> &vector, int index);

    private:
        const std::vector<core::PieceType> pieces_;
        const int pop_;
    };
}

#endif //CORE_COMBINATIONS_HPP
