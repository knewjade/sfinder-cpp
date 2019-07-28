#ifndef FINDER_REVERSE_ORDER_LOOKUP_HPP
#define FINDER_REVERSE_ORDER_LOOKUP_HPP

#include <cassert>
#include <vector>

#include "../core/types.hpp"

namespace sfinder {
    // When it is possible to make a specified mino-sequence using “hold” from “one mino-sequence”,
    // all “one mino-sequence” is back-calculated and listed
    class ReverseLookup {
    public:
        static ReverseLookup create(int toDepth, int fromDepth) {
            assert(fromDepth == toDepth || fromDepth == toDepth + 1);
            auto indexesList = reverse(toDepth, fromDepth);
            return ReverseLookup(toDepth, fromDepth, indexesList, toDepth < fromDepth);
        }

        const int fromDepth;
        const int toDepth;
        const bool wildcard;

        [[nodiscard]] std::vector<std::vector<core::PieceType>> parse(std::vector<core::PieceType> pieces) const;

    private:
        static std::vector<std::vector<int>> reverse(int toDepth, int fromDepth);

        ReverseLookup(
                int toDepth, int fromDepth, std::vector<std::vector<int>> indexesList, bool wildcard
        ) : fromDepth(fromDepth), toDepth(toDepth), wildcard(wildcard), indexesList_(std::move(indexesList)) {
        }

        const std::vector<std::vector<int>> indexesList_;
    };

    // Enumerate all the mino-sequences that can be sorted using the hold from the specified mino-sequence
    class ForwardOrderLookUp {
    public:
        static ForwardOrderLookUp create(int toDepth, int fromDepth) {
            assert(toDepth <= fromDepth);
            auto isOverBlock = toDepth < fromDepth;
            auto indexesList = forward(toDepth, isOverBlock);
            return ForwardOrderLookUp(toDepth, fromDepth, indexesList);
        }

        const int fromDepth;
        const int toDepth;

        [[nodiscard]] std::vector<std::vector<core::PieceType>> parse(std::vector<core::PieceType> pieces) const;

    private:
        static std::vector<std::vector<int>> forward(int toDepth, bool isOverBlock);

        ForwardOrderLookUp(
                int toDepth, int fromDepth, std::vector<std::vector<int>> indexesList
        ) : fromDepth(fromDepth), toDepth(toDepth), indexesList_(std::move(indexesList)) {
        }

        const std::vector<std::vector<int>> indexesList_;
    };

    class StackOrder {
    public:
        void addLast(int number);

        void addLastTwo(int number);

        void addLastTwoAndRemoveLast(int number);

        void stock(int number);

        std::vector<int> vector();

    private:
        std::vector<int> blocks_{};
        int stockIndex_ = 0;
    };
}

#endif //FINDER_REVERSE_ORDER_LOOKUP_HPP
