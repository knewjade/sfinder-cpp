#ifndef FINDER_ALL_PCS_FRONTIER_HPP
#define FINDER_ALL_PCS_FRONTIER_HPP

#include "core/factory.hpp"

namespace sfinder::all_pcs {
    struct IndexNode {
        unsigned int id;
        unsigned int type;
    };

    struct ItemNode {
        int minoIndex;
        unsigned int id;
    };

    class Nodes {
    public:
        explicit Nodes(unsigned int indexSize, unsigned int itemSize)
                : indexNodes_(std::vector<IndexNode>(indexSize)),
                  itemNodes_(std::vector<ItemNode>(itemSize))  {
        }

        void jump(unsigned int nextItemId, unsigned int itemSize) {
            assert(indexSerial_ < indexNodes_.size());
            auto &node = indexNodes_[indexSerial_];
            node.id = nextItemId;
            node.type = itemSize;
            indexSerial_ += 1;
        }

        void skip(unsigned int nextIndexId) {
            constexpr auto uintMax = std::numeric_limits<unsigned int>::max();

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

        [[nodiscard]] unsigned int indexNodeSize() const {
            return indexNodes_.size();
        }

    private:
        std::vector<IndexNode> indexNodes_;
        std::vector<ItemNode> itemNodes_;
        unsigned int indexSerial_ = 2;
        unsigned int itemSerial_ = 0;
    };

    class Hashes {
    public:
        explicit Hashes(unsigned int logTableSize)
                : table_(std::vector<unsigned int>(1U << logTableSize)),
                  logTableSize_(logTableSize), itemMask_((1U << logTableSize_) - 1U) {
            rotate();
        }

        void rotate() {
            id_ += 1;
            prefix_ = id_ << logTableSize_;
            if (prefix_ == 0U) {
                // Overflow
                std::fill(table_.begin(), table_.end(), 0U);
                id_ = 1;
                prefix_ = id_ << logTableSize_;
            }
        }

        void set(unsigned int hash, unsigned int value) {
            assert(hash <= itemMask_);
            table_[hash] = prefix_ | value;
        }

        const unsigned int &operator[](unsigned int hash) const {
            assert(hash <= itemMask_);
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
        const unsigned int logTableSize_;
        const unsigned int itemMask_;
        unsigned int id_ = 0U;
        unsigned int prefix_ = 0U;
    };

    class Frontier {
    public:
        Frontier(
                const std::vector<Mino> &minos, Nodes &nodes, int width, int height
        ) : minos_(minos), nodes_(nodes), width_(width), height_(height),
            hashes_(Hashes(height * 3)),
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
}

#endif //FINDER_ALL_PCS_FRONTIER_HPP
