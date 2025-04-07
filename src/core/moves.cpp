#include <cassert>

#include "moves.hpp"

namespace core {
    namespace {
        constexpr unsigned int kFieldWidthUnsigned = FIELD_WIDTH;
        constexpr int kFieldWidth = FIELD_WIDTH;
    }

    namespace {
        uint64_t getXMask(int x, int y) {
            assert(0 <= x && x < kFieldWidth);
            assert(0 <= y && y < kMaxFieldHeight);

            return 1LLU << (x + y * kFieldWidthUnsigned);
        }
    }

    void Cache::visit(int x, int y, RotateType rotateType) {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        uint64_t mask = getXMask(x, y - 6 * index);

        int boardIndex = index + 4 * rotateType;
        visitedBoard[boardIndex] |= mask;
    }

    bool Cache::isVisit(int x, int y, core::RotateType rotateType) const {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        uint64_t mask = getXMask(x, y - 6 * index);

        int boardIndex = index + 4 * rotateType;
        return (visitedBoard[boardIndex] & mask) != 0;
    }

    void Cache::visitPartially(int x, int y, RotateType rotateType) {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        uint64_t mask = getXMask(x, y - 6 * index);

        int boardIndex = index + 4 * rotateType;
        visitedPartiallyBoard[boardIndex] |= mask;
    }

    bool Cache::isVisitPartially(int x, int y, core::RotateType rotateType) const {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        uint64_t mask = getXMask(x, y - 6 * index);

        int boardIndex = index + 4 * rotateType;
        return (visitedPartiallyBoard[boardIndex] & mask) != 0;
    }

    void Cache::found(int x, int y, RotateType rotateType) {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        uint64_t mask = getXMask(x, y - 6 * index);

        int boardIndex = index + 4 * rotateType;
        foundBoard[boardIndex] |= mask;
    }

    bool Cache::isFound(int x, int y, core::RotateType rotateType) const {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        auto mask = getXMask(x, y - 6 * index);

        int boardIndex = index + 4 * rotateType;
        return (foundBoard[boardIndex] & mask) != 0;
    }

    void Cache::push(int x, int y, RotateType rotateType) {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        uint64_t mask = getXMask(x, y - 6 * index);

        int boardIndex = index + 4 * rotateType;
        pushedBoard[boardIndex] |= mask;
    }

    bool Cache::isPushed(int x, int y, core::RotateType rotateType) const {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        uint64_t mask = getXMask(x, y - 6 * index);

        int boardIndex = index + 4 * rotateType;
        return (pushedBoard[boardIndex] & mask) != 0;
    }

    namespace harddrop {
        void MoveGenerator::search(
                std::vector<Move> &moves, const Field &field, const PieceType pieceType, int validHeight
        ) {
            auto &piece = factory.get(pieceType);

            auto bit = piece.uniqueRotateBit;
            assert(bit != 0);

            do {
                auto next = bit & (bit - 1U);
                RotateType rotateType = rotateBitToVal[bit & ~next];

                auto &blocks = factory.get(pieceType, rotateType);

                int y = validHeight - blocks.minY;
                int maxY = validHeight - blocks.maxY;
                for (int x = -blocks.minX, maxX = kFieldWidth - blocks.maxX; x < maxX; ++x) {
                    int harddropY = field.getYOnHarddrop(blocks, x, y);
                    if (harddropY < maxY) {
                        moves.push_back(Move{rotateType, x, harddropY, true});
                    }
                }

                bit = next;
            } while (bit != 0);
        }
    }
}