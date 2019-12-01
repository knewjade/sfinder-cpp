#ifndef FINDER_REVERSE_ORDER_LOOKUP_HPP
#define FINDER_REVERSE_ORDER_LOOKUP_HPP

#include <cassert>
#include <vector>

#include "../core/types.hpp"

namespace sfinder {
    // When it is possible to make a specified mino-sequence using “hold” from “one mino-sequence”,
    // all “one mino-sequence” is back-calculated and listed
    class ReverseOrderLookup {
    public:
        static ReverseOrderLookup create(int toDepth, int fromDepth) {
            assert(fromDepth == toDepth || fromDepth == toDepth + 1);
            auto indexesList = reverse(toDepth, fromDepth);
            return ReverseOrderLookup(toDepth, fromDepth, indexesList, toDepth < fromDepth);
        }

        const int fromDepth;
        const int toDepth;
        const bool wildcard;

        [[nodiscard]] std::vector<std::vector<core::PieceType>> parse(std::vector<core::PieceType> pieces) const;

    private:
        static std::vector<std::vector<int>> reverse(int toDepth, int fromDepth);

        ReverseOrderLookup(
                int toDepth, int fromDepth, std::vector<std::vector<int>> indexesList, bool wildcard
        ) : fromDepth(fromDepth), toDepth(toDepth), wildcard(wildcard), indexesList_(std::move(indexesList)) {
        }

        const std::vector<std::vector<int>> indexesList_;
    };

    // Enumerate all the mino-sequences that can be sorted using the hold from the specified mino-sequence
    class ForwardOrderLookUp {
    public:
        static ForwardOrderLookUp create(int toDepth, int fromDepth) {
            return create(toDepth, fromDepth, true, false);
        }

        static ForwardOrderLookUp create(int toDepth, int fromDepth, bool holdEmpty, bool mustNotUseHoldAtFirst) {
            assert(fromDepth == toDepth || fromDepth == toDepth + 1);
            auto isOverBlock = toDepth < fromDepth;
            auto indexesList = forward(toDepth, isOverBlock, holdEmpty, mustNotUseHoldAtFirst);
            return ForwardOrderLookUp(toDepth, fromDepth, indexesList);
        }

        const int fromDepth;
        const int toDepth;

        [[nodiscard]] std::vector<std::vector<core::PieceType>> parse(std::vector<core::PieceType> pieces) const;

        template<class Operator>
        void foreachUntilTrue(std::vector<core::PieceType> pieces, Operator op) const {
            assert(fromDepth <= pieces.size());

            std::vector<core::PieceType> toPieces(toDepth);
            for (auto &indexes : indexesList_) {
                for (int i = 0; i < toDepth; ++i) {
                    auto index = indexes[i];
                    assert(0 <= index);
                    toPieces[i] = static_cast<core::PieceType>(pieces[index]);
                }

                auto result = op(toPieces);
                if (result) {
                    return;
                }
            }
        }

    private:
        static std::vector<std::vector<int>> forward(
                int toDepth, bool isOverBlock, bool holdEmpty, bool mustNotUseHoldAtFirst
        );

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