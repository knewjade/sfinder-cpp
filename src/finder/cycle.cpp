#include "cycle.hpp"

namespace finder {
    std::vector<core::PieceType> Cycle::pieces(int index) {
        std::vector<core::PieceType> vector = {};
        push(vector, index, permutations_.size() - 1);
        return vector;
    }

    void Cycle::push(std::vector<core::PieceType> &vector, int index, int depth) {
        assert(0 <= depth);

        auto permutation = permutations_[depth];
        auto size = permutation.size();

        auto k = index % size;
        index /= size;

        if (0 < depth) {
            push(vector, index, depth - 1);
        }

        permutation.push(vector, k);
    }

    int Cycle::size() {
        int size = 1;
        for (auto &permutation : permutations_) {
            size *= permutation.size();
        }
        return size;
    }
}
