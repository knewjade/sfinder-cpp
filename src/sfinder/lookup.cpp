#include "lookup.hpp"

namespace sfinder {
    std::vector<std::vector<int>> ReverseLookup::reverse(int toDepth, int fromDepth) {
        assert(1 <= toDepth);
        assert(toDepth <= fromDepth);

        std::vector<int> indexes(toDepth);
        for (int index = 0; index < toDepth; ++index) {
            indexes[index] = index;
        }

        std::vector<StackOrder> candidates{
                StackOrder{},
        };
        for (int depth = 0; depth < fromDepth; depth++) {
            int indexSize = indexes.size();
            int number = depth < indexSize ? indexes[depth] : -1;

            int size = candidates.size();
            if (depth < fromDepth - 1) {
                for (int index = 0; index < size; index++) {
                    auto &candidate = candidates[index];
                    auto freeze = StackOrder{candidate};

                    candidate.addLast(number);
                    freeze.stock(number);

                    candidates.emplace_back(freeze);
                }
            } else {
                for (auto &candidate : candidates)
                    candidate.stock(number);
            }
        }

        std::vector<std::vector<int>> indexesList{};
        indexesList.reserve(candidates.size());
        for (auto &candidate : candidates) {
            const std::vector<int> &args = candidate.vector();
            indexesList.emplace_back(args);
        }

        return indexesList;
    }

    std::vector<std::vector<core::PieceType>> ReverseLookup::parse(std::vector<core::PieceType> pieces) const {
        assert(pieces.size() <= indexesList_.at(0).size());

        std::vector<std::vector<core::PieceType>> pieceList{};
        pieceList.reserve(indexesList_.size());
        for (auto &indexes : indexesList_) {
            int size = indexes.size();
            std::vector<core::PieceType> list(size);
            for (int i = 0; i < size; ++i) {
                auto index = indexes[i];
                list[i] = static_cast<core::PieceType>(index != -1 ? pieces[index] : -1);
            }
            pieceList.emplace_back(list);
        }

        return pieceList;
    }

    std::vector<std::vector<int>> ForwardOrderLookUp::forward(int toDepth, bool isOverBlock) {
        assert(1 < toDepth);

        std::vector<int> indexes(toDepth);
        for (int index = 0; index < toDepth; ++index) {
            indexes[index] = index;
        }

        std::vector<StackOrder> candidates{};

        {
            auto stack = StackOrder{};
            stack.addLast(indexes[0]);
            stack.addLast(indexes[1]);
            candidates.emplace_back(stack);
        }

        {
            auto stack = StackOrder{};
            stack.addLast(indexes[1]);
            stack.addLast(indexes[0]);
            candidates.emplace_back(stack);
        }

        for (int depth = 2; depth < toDepth; depth++) {
            int number = indexes[depth];
            int size = candidates.size();
            for (int index = 0; index < size; index++) {
                auto &candidate = candidates[index];
                auto freeze = StackOrder{candidate};

                candidate.addLastTwo(number);
                freeze.addLast(number);

                candidates.emplace_back(freeze);
            }
        }

        if (isOverBlock) {
            auto number = toDepth;
            int size = candidates.size();
            for (int index = 0; index < size; index++) {
                auto &candidate = candidates[index];
                auto freeze = StackOrder{candidate};

                candidate.addLastTwoAndRemoveLast(number);

                candidates.emplace_back(freeze);
            }
        }

        std::vector<std::vector<int>> indexesList{};
        indexesList.reserve(candidates.size());
        for (auto &candidate : candidates) {
            const std::vector<int> &args = candidate.vector();
            indexesList.emplace_back(args);
        }

        return indexesList;
    }

    std::vector<std::vector<core::PieceType>> ForwardOrderLookUp::parse(std::vector<core::PieceType> pieces) const {
        assert(fromDepth <= pieces.size());

        std::vector<std::vector<core::PieceType>> pieceList{};
        pieceList.reserve(indexesList_.size());
        for (auto &indexes : indexesList_) {
            int size = indexes.size();
            std::vector<core::PieceType> list(size);
            for (int i = 0; i < size; ++i) {
                auto index = indexes[i];
                assert(0 <= index);
                list[i] = static_cast<core::PieceType>(pieces[index]);
            }
            pieceList.emplace_back(list);
        }

        return pieceList;
    }

    void StackOrder::addLast(int number) {
        blocks_.push_back(number);
    }

    void StackOrder::addLastTwo(int number) {
        blocks_.insert(blocks_.end() - 1, number);
    }

    void StackOrder::addLastTwoAndRemoveLast(int number) {
        blocks_.insert(blocks_.end() - 1, number);
        blocks_.erase(blocks_.end() - 1);
    }

    void StackOrder::stock(int number) {
        blocks_.insert(blocks_.begin() + stockIndex_, number);
        stockIndex_ = blocks_.size();
    }

    std::vector<int> StackOrder::vector() {
        return blocks_;
    }
}
