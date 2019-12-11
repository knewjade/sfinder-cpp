#ifndef FINDER_ALL_PCS_FRONTIER_HPP
#define FINDER_ALL_PCS_FRONTIER_HPP

#include "core/factory.hpp"
#include "core/moves.hpp"

namespace sfinder::all_pcs {
    struct IndexNode {
        unsigned int id;
        unsigned int type;
    };

    struct ItemNode {
        int minoIndex;
        unsigned int id;
    };

    constexpr auto uintMax = std::numeric_limits<unsigned int>::max();

    class Nodes {
    public:
        explicit Nodes(unsigned int logIndexSize, unsigned int logItemSize)
                : indexNodes_(std::vector<IndexNode>(1U << logIndexSize)),
                  itemNodes_(std::vector<ItemNode>(1U << logItemSize)),
                  logIndexSize_(logIndexSize) {
        }

        void jump(unsigned int nextItemId, unsigned int itemSize) {
            assert(indexSerial_ < indexNodes_.size());
            auto &node = indexNodes_[indexSerial_];
            node.id = nextItemId;
            node.type = itemSize;
            indexSerial_ += 1;
        }

        void skip(unsigned int nextIndexId) {
            assert(indexSerial_ < indexNodes_.size());
            auto &node = indexNodes_[indexSerial_];
            node.id = nextIndexId;
            node.type = uintMax;
            indexSerial_ += 1;
        }

        void put(int minoIndex, unsigned int nextIndexId) {
            assert(itemSerial_ < itemNodes_.size());
            auto &node = itemNodes_[itemSerial_];
            node.minoIndex = minoIndex;
            node.id = nextIndexId;
            itemSerial_ += 1;
        }

        void complete(int minoIndex) {
            assert(itemSerial_ < itemNodes_.size());
            auto &node = itemNodes_[itemSerial_];
            node.minoIndex = minoIndex;
            node.id = 1U;
            itemSerial_ += 1;
        }

        [[nodiscard]] unsigned int itemNodeSerial() const {
            return itemSerial_;
        }

        [[nodiscard]] unsigned int indexSerial() const {
            return indexSerial_;
        }

        [[nodiscard]] unsigned long long indexNodeSize() const {
            return indexNodes_.size();
        }

        [[nodiscard]] unsigned int logIndexSize() const {
            return logIndexSize_;
        }

        IndexNode &indexNode(unsigned int index) {
            return indexNodes_[index];
        }

        ItemNode &itemNode(unsigned int index) {
            return itemNodes_[index];
        }

    private:
        std::vector<IndexNode> indexNodes_;
        std::vector<ItemNode> itemNodes_;
        const unsigned int logIndexSize_;
        unsigned int indexSerial_ = 2;
        unsigned int itemSerial_ = 0;
    };

    class Hashes {
    public:
        explicit Hashes(unsigned int logTableSize, unsigned int logValueSize)
                : table_(std::vector<unsigned int>(1U << logTableSize)),
                  logValueSize_(logValueSize), itemMask_((1U << logValueSize) - 1U) {
            rotate();
        }

        void rotate() {
            id_ += 1;
            prefix_ = id_ << logValueSize_;
            if (prefix_ == 0U) {
                // Overflow
                std::fill(table_.begin(), table_.end(), 0U);
                id_ = 1;
                prefix_ = id_ << logValueSize_;
            }
        }

        void set(unsigned int hash, unsigned int value) {
            assert(hash < table_.size());
            assert(value <= itemMask_);
            table_[hash] = prefix_ | value;
        }

        const unsigned int &operator[](unsigned int hash) const {
            assert(hash < table_.size());
            return table_[hash];
        }

        [[nodiscard]] bool registered(unsigned int value) const {
            return prefix_ <= value;
        }

        [[nodiscard]] unsigned int pop(unsigned int value) const {
            assert(registered(value));
            return value & itemMask_;
        }

    private:
        std::vector<unsigned int> table_;
        const unsigned int logValueSize_;
        const unsigned int itemMask_;
        unsigned int id_ = 0U;
        unsigned int prefix_ = 0U;
    };

    class Frontier {
    public:
        Frontier(
                const std::vector<Mino> &minos, Nodes &nodes, int width, int height
        ) : minos_(minos), nodes_(nodes), width_(width), height_(height),
            hashes_(Hashes(height * 3, nodes.logIndexSize())),
            frontiers_(std::vector<uint64_t>(nodes.indexNodeSize())),
            minoCounts_(std::vector<int>(height * width)) {

            for (const auto &mino : minos) {
                int index = mino.minVerticalIndex;
                minoCounts_[index] += 1;
                if (maxVerticalIndex_ < index) {
                    maxVerticalIndex_ = index;
                }
            }
        }

        void build() {
            // Make `frontiers[i]` and `nodes.index[i]` represent the same state
            unsigned int tail = nodes_.indexSerial();
            unsigned int head = tail + 1;

            int startMinoIndex = 0;
            for (int index = 0; index <= maxVerticalIndex_; ++index) {
//                std::cout << "index=" << index << std::endl;

                auto endMinoIndex = startMinoIndex + minoCounts_[index];

                hashes_.rotate();

                // Number of remaining search blocks, including the block at `index`
                unsigned int rest = height_ * width_ - index;
                uint64_t fillBlockMask = -1ULL;
                if (rest <= 3 * height_ + 1) {
                    // All remaining blocks are filled, including the block at `index`
                    fillBlockMask = (1ULL << rest) - 1;
                }

                assert(tail == nodes_.indexSerial());
                auto boundary = head;

                while (tail < boundary) {
                    auto currentBits = frontiers_[tail];
                    if ((currentBits & 1ULL) == 0) {
                        // No block at `index`
                        auto startItemNodeIndex = nodes_.itemNodeSerial();
                        unsigned int itemSize = 0U;

                        for (int minoIndex = startMinoIndex; minoIndex < endMinoIndex; ++minoIndex) {
                            auto &mino = minos_[minoIndex];

                            if ((currentBits & mino.verticalRelativeBlock) == 0) {
                                // Can put mino

                                itemSize += 1;
                                auto nextBlock = currentBits | mino.verticalRelativeBlock;

                                if (nextBlock == fillBlockMask) {
                                    // Filled all
                                    nodes_.complete(minoIndex);
                                    continue;
                                }

                                auto hash = nextBlock >> 1U;
                                auto raw = hashes_[hash];
                                if (hashes_.registered(raw)) {
                                    nodes_.put(minoIndex, hashes_.pop(raw));
                                } else {
                                    // Found new state

                                    assert(head < frontiers_.size());
                                    frontiers_[head] = hash;

                                    // [Future reference] If `head` exceeds the `frontiers` size and rotate index, becomes nextIndexId != head
                                    auto nextIndexId = head;
                                    hashes_.set(hash, nextIndexId);

                                    nodes_.put(minoIndex, nextIndexId);

                                    head += 1;
                                }
                            }
                        }

                        assert(tail == nodes_.indexSerial());
                        nodes_.jump(startItemNodeIndex, itemSize);
                    } else {
                        // Filled block at `index`
                        auto hash = currentBits >> 1U;
                        auto raw = hashes_[hash];
                        if (hashes_.registered(raw)) {
                            assert(tail == nodes_.indexSerial());
                            nodes_.skip(hashes_.pop(raw));
                        } else {
                            // Found new state

                            assert(head < frontiers_.size());
                            frontiers_[head] = hash;

                            // [Future reference] If `head` exceeds the `frontiers` size and rotate index, becomes nextIndexId != head
                            auto nextIndexId = head;
                            hashes_.set(hash, nextIndexId);

                            assert(tail == nodes_.indexSerial());
                            nodes_.skip(nextIndexId);

                            head += 1;
                        }
                    }

                    tail += 1;
                }

                startMinoIndex = endMinoIndex;

                // Verify
                if (frontiers_.size() < head) {
                    throw std::runtime_error("frontiers is overflow");
                }
            }
        }

    private:
        const std::vector<Mino> &minos_;
        Nodes &nodes_;
        const int width_;
        const int height_;

        Hashes hashes_;
        std::vector<uint64_t> frontiers_;
        std::vector<int> minoCounts_;
        int maxVerticalIndex_;
    };

    class Builder {
    public:
        Builder(
                const core::Factory &factory, const std::vector<Mino> &minos, int width, int height, int maxDepth,
                core::srs::MoveGenerator &moveGenerator
        ) : factory_(factory), minos_(minos), height_(height), moveGenerator_(moveGenerator),
            visited_(Hashes(maxDepth, 1)), fields_(std::vector<core::Field>(maxDepth)),
            minoIndexes_(std::vector<int>(maxDepth)), bitToIndex_(std::vector<int>(1U << maxDepth)) {

            for (int iv = 0; iv < maxDepth; ++iv) {
                bitToIndex_[1U << iv] = iv;
            }

            for (int y = 0; y < height; ++y) {
                for (int x = width; x < core::kFieldWidth; ++x) {
                    initField_.setBlock(x, y);
                }
            }
        }

        bool checks(const std::vector<int> &results) {
            visited_.rotate();

            // ざっくりdeleteLineが0のものを前にだす
            // 成功時にざっくり早くする
            // ソートすると精度があがると思われるが、速度面で検証したほうがよさそう
            int l = 0;
            int l2 = 0;
            for (int result : results) {
                auto &mino = minos_[result];
                if (mino.deletedLine == 0) {
                    minoIndexes_[l2] = minoIndexes_[l];
                    l2 += 1;

                    minoIndexes_[l] = result;
                    l += 1;
                } else {
                    minoIndexes_[l2] = result;
                    l2 += 1;
                }
            }

            fields_[0] = initField_;

            return checks(
                    (1U << minoIndexes_.size()) - 1, 0U, 0
            );
        }

        bool checks(
                unsigned int rest, unsigned int used, int depth
        ) {
            //
            auto raw = visited_[used];
            if (visited_.registered(raw)) {
                return false;
            }
            visited_.set(used, 1);

            auto &beforeField = fields_[depth];

            auto field = core::Field(beforeField);
            auto deletedKey = field.clearLineReturnKey();

            auto rest2 = rest;
            while (rest2 != 0) {
                auto bit = rest2 & (-rest2);

                auto nextUsed = used | bit;
                int index = bitToIndex_[bit];
                auto &mino = minos_[minoIndexes_[index]];

                // ミノを置くためのラインがすべて削除されている
                if ((mino.deletedKey & deletedKey) == mino.deletedKey) {
                    int originalY = mino.y;
                    int deletedLines = core::bitCount((core::getUsingKeyBelowY(originalY) & deletedKey));

//                    std::cout << mino.minoField.toString(4) << std::endl;

                    int x = mino.x;
                    int y = originalY - deletedLines;
                    auto &blocks = factory_.get(mino.pieceType, mino.rotateType);
                    if (field.isOnGround(blocks, x, y) && field.canPut(blocks, x, y)
                        &&
                        moveGenerator_.canReach(field, mino.pieceType, mino.rotateType, x, y, height_ - blocks.minY)) {
                        auto nextRest = rest - bit;
                        if (nextRest == 0) {
                            return true;
                        }

                        auto &nextField = fields_[depth + 1];
                        nextField = core::Field(beforeField);
                        nextField.merge(mino.minoField);

//                        std::cout << nextField.toString(4) << std::endl;

                        auto exists = checks(nextRest, nextUsed, depth + 1);
                        if (exists)
                            return true;
                    }
                }

                rest2 -= bit;
            }

            return false;
        }

        bool checks2(const std::vector<int> &results) {
            visited_.rotate();

            // ざっくりdeleteLineが0のものを前にだす
            // 成功時にざっくり早くする
            // ソートすると精度があがると思われるが、速度面で検証したほうがよさそう
            int l = 0;
            int l2 = 0;
            for (int result : results) {
                auto &mino = minos_[result];
                if (mino.deletedLine == 0) {
                    minoIndexes_[l2] = minoIndexes_[l];
                    l2 += 1;

                    minoIndexes_[l] = result;
                    l += 1;
                } else {
                    minoIndexes_[l2] = result;
                    l2 += 1;
                }
            }

            fields_[0] = initField_;

            return checks2(
                    (1U << minoIndexes_.size()) - 1, 0U, 0
            );
        }

        bool checks2(
                unsigned int rest, unsigned int used, int depth
        ) {
            //
            auto raw = visited_[used];
            if (visited_.registered(raw)) {
                return false;
            }
            visited_.set(used, 1);

            auto &beforeField = fields_[depth];

            auto field = core::Field(beforeField);
            auto deletedKey = field.clearLineReturnKey();

            auto rest2 = rest;
            while (rest2 != 0) {
                auto bit = rest2 & (-rest2);

                auto nextUsed = used | bit;
                int index = bitToIndex_[bit];
                auto &mino = minos_[minoIndexes_[index]];

                // ミノを置くためのラインがすべて削除されている
                if ((mino.deletedKey & deletedKey) == mino.deletedKey) {
                    int originalY = mino.y;
                    int deletedLines = core::bitCount((core::getUsingKeyBelowY(originalY) & deletedKey));

//                    std::cout << mino.minoField.toString(4) << std::endl;

                    int x = mino.x;
                    int y = originalY - deletedLines;
                    auto &blocks = factory_.get(mino.pieceType, mino.rotateType);
                    if (field.canPut(blocks, x, y)) {
                        auto nextRest = rest - bit;
                        if (nextRest == 0) {
                            return true;
                        }

                        auto &nextField = fields_[depth + 1];
                        nextField = core::Field(beforeField);
                        nextField.merge(mino.minoField);

//                        std::cout << nextField.toString(4) << std::endl;

                        auto exists = checks2(nextRest, nextUsed, depth + 1);
                        if (exists)
                            return true;
                    }
                }

                rest2 -= bit;
            }

            return false;
        }

    private:
        const core::Factory &factory_;
        const std::vector<Mino> &minos_;
        const int height_;
        core::srs::MoveGenerator &moveGenerator_;
        Hashes visited_;
        std::vector<core::Field> fields_;
        std::vector<int> minoIndexes_;
        std::vector<int> bitToIndex_;
        core::Field initField_;
    };

    class Aggregator {
    public:
        Aggregator(
                const core::Factory &factory, const std::vector<Mino> &minos, int height, int maxDepth,
                const std::vector<LineBit> &usingLineEachY, Nodes &nodes, Builder &builder
        ) : factory_(factory), minos_(minos), height_(height), maxDepth_(maxDepth),
            usingLineEachY_(usingLineEachY), nodes_(nodes), builder_(builder),
            filled_(std::vector<unsigned int>((maxDepth + 1) * height)),
            results_(std::vector<int>(maxDepth)) {
        }

        const core::Factory &factory_;
        const std::vector<Mino> &minos_;
        const int height_;
        const int maxDepth_;
        const std::vector<LineBit> &usingLineEachY_;
        Nodes &nodes_;
        Builder &builder_;
        std::vector<unsigned int> filled_;
        std::vector<int> results_;

        unsigned long long sum = 0;

        void aggregate() {
            aggregate(2, 0);
        }

        void aggregate(unsigned int indexId, int depth) {
            assert(0 <= depth && depth < maxDepth_);
            auto &indexNode = nodes_.indexNode(indexId);
            if (indexNode.type == uintMax) {
                // jump
                aggregate(indexNode.id, depth);
            } else {
                // move
                for (unsigned int index = indexNode.id, end = indexNode.id + indexNode.type; index < end; ++index) {
                    auto &itemNode = nodes_.itemNode(index);

                    auto minoIndex = itemNode.minoIndex;
                    auto &mino = minos_[minoIndex];

                    //
                    auto s = 0U;
                    for (const auto &usingY : mino.usingYs) {
                        // 注目しているミノのブロックのあるyを取得
                        if (usingY < 0) {
                            break;
                        }

                        // 注目しているミノをおいたあとに初めて揃えられるライン
                        // 前の結果 + 今回の結果
                        s |= filled_[depth * height_ + usingY];
                    }

                    // 注目しているミノを置く前の絶対に揃えられないラインが削除されていないといけないか
                    if (0 < (s & mino.deletedLine)) {
                        // 使っている
                        continue;
                    }

                    results_[depth] = minoIndex;

                    auto nextId = itemNode.id;
                    if (nextId == 1U) {
//                        auto result = builder_.checks2(results_);
//                        if (result) {
//                            for (int j = 0; j < results_.size(); ++j) {
//                                auto &mino = minos_[results_[j]];
//                                std::cout << mino.minoField.toString(4) << std::endl;
//                            }

                        sum++;
                        if (sum % 5000000 == 0) {
                            std::cout << sum << std::endl;
                        }

//                        }
                    } else {
                        auto nextDepth = depth + 1;

                        auto ni = nextDepth * height_;
                        auto ci = depth * height_;
                        auto hi = minoIndex * height_;

                        // 揃えられないラインを更新
                        // temp = [y]ラインにブロックがあると、使用できないライン一覧が記録されている
                        // ミノXの[y]がdeletedKeyに指定されていると、Xのブロックのあるラインは先に揃えられなくなる
                        for (int j = 0; j < height_; ++j) {
                            filled_[ni + j] = filled_[ci + j] | usingLineEachY_[hi + j];
                        }

                        aggregate(nextId, nextDepth);
                    }
                }
            }
        }
    };
}

#endif //FINDER_ALL_PCS_FRONTIER_HPP
