#ifndef CORE_MOVE_HPP
#define CORE_MOVE_HPP

#include <vector>
#include <cassert>

#include "field.hpp"
#include "srs.hpp"

namespace core {
    struct Move {
        RotateType rotateType;
        int x;
        int y;
    };

    inline bool operator==(const Move &lhs, const Move &rhs) {
        return lhs.rotateType == rhs.rotateType && lhs.x == rhs.x && lhs.y == rhs.y;
    }

    inline bool operator!=(const Move &lhs, const Move &rhs) {
        return !(lhs == rhs);
    }

    class Cache {
    public:
        void visit(int x, int y, RotateType rotateType);

        bool isVisit(int x, int y, RotateType rotateType) const;

        void found(int x, int y, RotateType rotateType);

        bool isFound(int x, int y, RotateType rotateType) const;

        void resetTrail();

        void clear();

    private:
        Bitboard visitedBoard[4 * 4];
        Bitboard foundBoard[4 * 4];
    };

    namespace harddrop {
        class MoveGenerator {
        public:
            MoveGenerator(const Factory &factory) : factory(factory) {
            }

            void search(std::vector<Move> &moves, const Field &field, const PieceType pieceType, int validHeight);

        private:
            const Factory &factory;
        };
    }

    namespace srs {
        enum From {
            None,
            Right,
            Left,
        };

        struct TargetObject {
            const Field &field;
            const Piece &piece;
        };

        class MoveGenerator {
        public:
            MoveGenerator(const Factory &factory) : factory(factory), cache(Cache()), appearY(-1) {
            }

            void search(std::vector<Move> &moves, const Field &field, const PieceType pieceType, int validHeight);

        private:
            const Factory &factory;

            Cache cache;
            int appearY;

            bool checkLeftRotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY);

            bool checkRightRotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY);

            bool check(const TargetObject &targetObject, const Blocks &blocks, int x, int y, From from);
        };
    }
}

#endif //CORE_MOVE_HPP
