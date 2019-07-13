#include "permutation.hpp"

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

    int Permutation::size() {
        int size = 1;
        int scale = pieces_.size();
        for (int count = 0; count < pop_; ++count) {
            size *= scale;
            scale -= 1;
        }
        return size;
    }

    std::vector<core::PieceType> Permutation::pieces(int index) {
        std::vector<core::PieceType> vector = {};
        pushPieces(vector, index, pieces_, pop_);
        return vector;
    }

    void Permutation::push(std::vector<core::PieceType> &vector, int index) {
        pushPieces(vector, index, pieces_, pop_);
    }
}
