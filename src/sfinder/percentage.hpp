#ifndef FINDER_PERCENTAGE_HPP
#define FINDER_PERCENTAGE_HPP

#include "../core/moves.hpp"
#include "perfect_clear/full_finder.hpp"
#include "permutations.hpp"
#include "spins.hpp"
#include "lookup.hpp"

namespace sfinder {
    template<class T = core::srs::MoveGenerator>
    class Percentage {
    public:
        Percentage(
                perfect_clear::PerfectClearFinder<T> &finder,
                const Permutations &permutations,
                const ReverseOrderLookup &reverseLookup
        ) : finder_(finder), permutations_(permutations), reverseLookup_(reverseLookup) {}

        int run(const core::Field &field, int maxDepth, int maxLine);

    private:
        perfect_clear::PerfectClearFinder<T> &finder_;
        const Permutations &permutations_;
        const ReverseOrderLookup &reverseLookup_;
    };
}

#endif //FINDER_PERCENTAGE_HPP
