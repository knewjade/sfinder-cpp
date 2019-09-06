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
        bool harddrop;
    };

    enum MoveResults {
        No = 0,
        Harddrop = 1,
        Softdrop = 2,
    };

    struct TargetObject {
        const Field &field;
        const Piece &piece;
    };

    inline bool operator==(const Move &lhs, const Move &rhs) {
        return lhs.rotateType == rhs.rotateType && lhs.x == rhs.x && lhs.y == rhs.y && lhs.harddrop == rhs.harddrop;
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

        void push(int x, int y, RotateType rotateType);

        bool isPushed(int x, int y, RotateType rotateType) const;

        void resetTrail();

        void clear();

    private:
        Bitboard visitedBoard[4 * 4];
        Bitboard foundBoard[4 * 4];
        Bitboard pushedBoard[4 * 4];
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

        class MoveGenerator {
        public:
            MoveGenerator(const Factory &factory) : factory(factory), cache(Cache()), appearY(-1) {
            }

            void search(std::vector<Move> &moves, const Field &field, const PieceType pieceType, int validHeight);

        private:
            const Factory &factory;

            Cache cache;
            int appearY;

            MoveResults checkLeftRotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY);

            MoveResults checkRightRotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY);

            MoveResults check(
                    const TargetObject &targetObject, const Blocks &blocks, int x, int y, From from, bool isFirstCall
            );
        };
    }

    namespace srs_rotate_end {
        enum From {
            None,
            Right,
            Left,
        };

        class Reachable {
        public:
            Reachable(const Factory &factory) : factory(factory), cache(Cache()), appearY(-1) {
            }

            bool checks(const Field &field, PieceType pieceType, RotateType rotateType, int x, int y, int validHeight);

        private:
            const Factory &factory;

            Cache cache;
            int appearY;

            MoveResults checkLeftRotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY);

            MoveResults checkRightRotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY);

            MoveResults firstCheck(const TargetObject &targetObject, const Blocks &blocks, int x, int y);

            MoveResults check(const TargetObject &targetObject, const Blocks &blocks, int x, int y, From from);
        };
    }
}

#endif //CORE_MOVE_HPP
