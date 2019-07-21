#ifndef FINDER_PERMUTATIONS_HPP
#define FINDER_PERMUTATIONS_HPP

#include <cassert>
#include <array>
#include <vector>
#include <queue>
#include <algorithm>

#include "../core/types.hpp"

namespace sfinder {
    class Permutation {
    public:
        template<int N>
        static Permutation create(const std::array<core::PieceType, N> &pieces, int pop) {
            return Permutation::create(std::vector<core::PieceType>(pieces.begin(), pieces.end()), pop);
        }

        static Permutation create(const std::vector<core::PieceType> &pieces, int pop) {
            std::vector<int> indexMap(7);
            std::vector<bool> usingFlags(7);
            for (int index = 0, size = pieces.size(); index < size; ++index) {
                auto piece = pieces.at(index);
                assert(0 <= piece && piece < static_cast<int>(indexMap.size()));
                indexMap[piece] = index;
                assert(!usingFlags[piece]);
                usingFlags[piece] = true;
            }

            std::vector<int> scales(pop);
            scales[pop - 1] = 1;
            for (int index = pop - 2, N = pieces.size(); 0 <= index; --index) {
                int scale = scales[index + 1] * (N - index - 1);
                scales[index] = scale;
            }

            return Permutation(pieces, indexMap, usingFlags, scales, pop);
        }

        Permutation(
                std::vector<core::PieceType> pieces, std::vector<int> indexMap, std::vector<bool> usingFlags,
                std::vector<int> scales, int pop
        ) : pieces_(std::move(pieces)), indexMap_(std::move(indexMap)), usingFlags_(std::move(usingFlags)),
            scales_(std::move(scales)), pop_(pop) {
            assert(0 < pop_);
            assert(pop_ <= static_cast<int>(pieces_.size()));
            assert(indexMap_.size() == 7);
            assert(static_cast<int>(scales_.size()) == pop_);
        }

        int indexSize() const;

        int depth() const;

        std::vector<core::PieceType> pieces(int index) const;

        void piecesAndPush(std::vector<core::PieceType> &vector, int index) const;

        int numberifyAndPop(std::queue<core::PieceType> &pieces) const;

    private:
        const std::vector<core::PieceType> pieces_;
        const std::vector<int> indexMap_;
        const std::vector<bool> usingFlags_;
        const std::vector<int> scales_;
        const int pop_;
    };

    class Permutations {
    public:
        static Permutations createCycle(int number) {
            auto permutations = std::vector<Permutation>{
                    Permutation::create<7>(core::kAllPieceType, 7),
                    Permutation::create<7>(core::kAllPieceType, 4)
            };

            return Permutations::create(permutations);
        }

        static Permutations create(
                std::vector<Permutation> &permutations
        ) {
            auto size = permutations.size();
            std::vector<int> scales(size);
            scales[size - 1] = 1;
            for (int index = size - 2; 0 <= index; --index) {
                int scale = scales[index + 1] * permutations[index + 1].indexSize();
                scales[index] = scale;
            }

            return Permutations(permutations, scales);
        }

        std::vector<core::PieceType> pieces(int index) const;

        int numberify(const std::vector<core::PieceType> &pieces) const;

        int numberifyAndPop(std::queue<core::PieceType, std::deque<core::PieceType>> &queue) const;

        int size() const;

        int depth() const;

    private:
        const std::vector<Permutation> permutations_;
        const std::vector<int> scales_;

        Permutations(
                std::vector<Permutation> permutations, std::vector<int> scales
        ) : permutations_(std::move(permutations)), scales_(std::move(scales)) {
        }

        void push(std::vector<core::PieceType> &vector, int index, int depth) const;
    };
}

#endif //FINDER_PERMUTATIONS_HPP
