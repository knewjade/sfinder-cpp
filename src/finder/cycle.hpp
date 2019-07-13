#ifndef CORE_CYCLE_HPP
#define CORE_CYCLE_HPP

#include "permutation.hpp"

namespace finder {
    class Cycle {
    public:
        static Cycle create(int number) {
            auto permutations = std::vector<Permutation>{
                    Permutation::create<7>(core::kAllPieceType, 7),
                    Permutation::create<7>(core::kAllPieceType, 4)
            };
            return Cycle(permutations);
        }

        explicit Cycle(std::vector<Permutation> permutations) : permutations_(std::move(permutations)) {
        }

        std::vector<core::PieceType> pieces(int index);

        int size();

    private:
        std::vector<Permutation> permutations_;

        void push(std::vector<core::PieceType> &vector, int index, int depth);
    };
}

#endif //CORE_CYCLE_HPP
