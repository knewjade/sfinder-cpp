#ifndef CORE_CPP_PERCENTAGE_HPP
#define CORE_CPP_PERCENTAGE_HPP

#include "../finder/permutations.hpp"
#include "../finder/perfect_clear.hpp"
#include "../core/moves.hpp"
#include "../finder/lookup.hpp"

namespace finder {
    template<class T = core::srs::MoveGenerator>
    class Percentage {
    public:
        explicit Percentage(
                perfect_clear::Finder<T> &finder,
                const Permutations &permutations,
                const ReverseLookup &reverseLookup
        ) : finder_(finder), permutations_(permutations), reverseLookup_(reverseLookup) {}

        int run(const core::Field &field, int maxDepth, int maxLine);

    private:
        perfect_clear::Finder<T> &finder_;
        const Permutations &permutations_;
        const ReverseLookup &reverseLookup_;
    };
}

#endif //CORE_CPP_PERCENTAGE_HPP
