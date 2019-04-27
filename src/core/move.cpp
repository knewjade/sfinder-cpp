#include "move.hpp"

namespace core {
    void harddrop::search(
            std::vector<Move> &moves, const Factory &factory, const Field &field, const PieceType &pieceType,
            int validHeight
    ) {
        auto &piece = factory.get(pieceType);

        auto bit = piece.uniqueRotateBit;
        assert(bit != 0);

        do {
            auto next = bit & (bit - 1);
            RotateType rotateType = rotateBitToVal[bit & ~next];

            auto &blocks = factory.get(pieceType, rotateType);

            int y = validHeight - blocks.minY;
            int maxY = validHeight - blocks.maxY;
            for (int x = -blocks.minX, maxX = FIELD_WIDTH - blocks.maxX; x < maxX; ++x) {
                int harddropY = field.getYOnHarddrop(blocks, x, y);
                if (harddropY < maxY) {
                    moves.push_back(Move{rotateType, x, harddropY});
                }
            }

            bit = next;
        } while (bit != 0);
    }
}