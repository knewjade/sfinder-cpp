#include "percentage.hpp"
#include "marker.hpp"

namespace finder {
    template<>
    int Percentage<>::run(const core::Field &field, int maxDepth, int maxLine) {
        assert(permutations_.depth() == reverseLookup_.fromDepth);
        assert(maxDepth == reverseLookup_.toDepth);

        int max = permutations_.size();
        auto marker = Marker::create(max);

        for (int value = 0; value < max; ++value) {
            if (marker.calculated(value)) {
                continue;
            }

            auto pieces = permutations_.pieces(value);

            auto result = finder_.run(field, pieces, maxDepth, maxLine, false, 0);

            if (!result.empty()) {
                marker.set(value, true);

                // Extract piece in result
                auto resultPieces = std::vector<core::PieceType>{};
                for (auto &operation : result) {
                    resultPieces.emplace_back(operation.pieceType);
                }

                if (reverseLookup_.wildcard) {
                    // When wildcard contains

                    auto fromPiecesList = reverseLookup_.parse(resultPieces);
                    for (auto &fromPieces : fromPiecesList) {
                        auto find = std::find(fromPieces.begin(), fromPieces.end(), -1);
                        assert(find != fromPieces.end());
                        int wildcardIndex = find - fromPieces.begin();
                        assert(0 <= wildcardIndex && wildcardIndex < fromPieces.size());

                        for (const auto &pieceType : core::kAllPieceType) {
                            assert(pieces.size() == fromPieces.size());

                            fromPieces[wildcardIndex] = pieceType;
                            int number = permutations_.numberify(fromPieces);
                            if (0 <= number) {
                                marker.set(number, true);
                            }
                        }
                    }
                } else {
                    // When wildcard does not contain

                    auto fromPiecesList = reverseLookup_.parse(resultPieces);
                    for (auto &fromPieces : fromPiecesList) {
                        assert(pieces.size() == fromPieces.size());

                        int number = permutations_.numberify(fromPieces);
                        if (0 <= number) {
                            marker.set(number, true);
                        }
                    }
                }
            } else {
                marker.set(value, false);
            }
        }

        int success = 0;
        for (int value = 0; value < max; ++value) {
            assert(marker.calculated(value));
            if (marker.succeed(value)) {
                success += 1;
            }
        }

        return success;
    }
}