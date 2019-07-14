#include  <numeric>

#include "permutations.hpp"

namespace finder {
    namespace {
        void pushPieces(
                std::vector<core::PieceType> &pieces, int value, const std::vector<core::PieceType> &usings, int pop
        ) {
            int N = usings.size();

            assert(0 < N && N <= 7);
            assert(0 < pop && pop <= N);

            int arr[pop];

            for (int index = pop - 1; 0 <= index; --index) {
                int scale = N - index;
                arr[index] = value % scale;
                value /= scale;
            }

            for (int select = pop - 2; 0 <= select; --select) {
                for (int adjust = select + 1; adjust < pop; ++adjust) {
                    if (arr[select] <= arr[adjust]) {
                        arr[adjust] += 1;
                    }
                }
            }

            for (const auto &item : arr) {
                pieces.push_back(usings[item]);
            }
        }
    }

    int Permutation::indexSize() const {
        return pieces_.size() * scales_[0];
    }

    int Permutation::depth() const {
        return pop_;
    }

    std::vector<core::PieceType> Permutation::pieces(int index) const {
        std::vector<core::PieceType> vector = {};
        pushPieces(vector, index, pieces_, pop_);
        return vector;
    }

    void Permutation::piecesAndPush(std::vector<core::PieceType> &vector, int index) const {
        pushPieces(vector, index, pieces_, pop_);
    }

    int Permutation::numberifyAndPop(std::queue<core::PieceType> &pieces) const {
        int indexes[pop_];
        std::vector<bool> flags(usingFlags_);

        for (int index = 0; index < pop_; ++index) {
            auto &piece = pieces.front();
            if (!flags[piece]) {
                return -1;
            }
            flags[piece] = false;
            indexes[index] = indexMap_[piece];
            pieces.pop();
        }

        for (int index = 0; index < pop_ - 1; ++index) {
            for (int j = index + 1; j < pop_; ++j) {
                if (indexes[index] < indexes[j]) {
                    indexes[j] -= 1;
                }
            }
        }

        int value = 0;
        for (int index = 0; index < pop_; ++index) {
            value += indexes[index] * scales_[index];
        }
        return value;
    }

    std::vector<core::PieceType> Permutations::pieces(int index) const {
        std::vector<core::PieceType> vector = {};
        push(vector, index, permutations_.size() - 1);
        return vector;
    }

    int Permutations::numberify(const std::vector<core::PieceType> &pieces) const {
        auto deque = std::deque<core::PieceType>(pieces.begin(), pieces.end());
        auto queue = std::queue<core::PieceType, std::deque<core::PieceType>>(deque);
        return numberifyAndPop(queue);
    }

    int Permutations::numberifyAndPop(std::queue<core::PieceType, std::deque<core::PieceType>> &queue) const {
        int value = 0;
        for (int index = 0; index < permutations_.size(); index++) {
            auto &permutation = permutations_[index];
            int number = permutation.numberifyAndPop(queue);
            if (number < 0) {
                return -1;
            }

            value += scales_[index] * number;
        }

        return value;
    }

    void Permutations::push(std::vector<core::PieceType> &vector, int index, int depth) const {
        assert(0 <= depth);

        auto permutation = permutations_[depth];
        auto size = permutation.indexSize();

        auto k = index % size;
        index /= size;

        if (0 < depth) {
            push(vector, index, depth - 1);
        }

        permutation.piecesAndPush(vector, k);
    }

    int Permutations::size() const {
        int size = 1;
        for (auto &permutation : permutations_) {
            size *= permutation.indexSize();
        }
        return size;
    }

    int Permutations::depth() const {
        int depth = 0;
        for (auto &permutation : permutations_) {
            depth += permutation.depth();
        }
        return depth;
    }
}
