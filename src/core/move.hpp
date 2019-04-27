#ifndef CORE_MOVE_HPP
#define CORE_MOVE_HPP

#include <vector>

#include "field.hpp"

namespace core {
    struct Move {
        RotateType rotateType;
        int x;
        int y;
    };

    namespace harddrop {
        class Searcher {
        public:
            void search(
                    std::vector<Move> &moves, const Factory &factory, const Field &field, const PieceType &pieceType,
                    int validHeight
            );
        };
    }

    namespace srs {
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

        enum From {
            None,
            Right,
            Left,
        };

        class Searcher {
        public:
            Searcher(Cache cache) : cache(cache), appearY(-1) {
            }

            void search(
                    std::vector<Move> &moves, const Factory &factory, const Field &field, const PieceType &pieceType,
                    int validHeight
            );

        private:
            Cache cache;
            int appearY;

            bool checkLeftRotation(const Field &field, const Piece &piece, const Blocks &toBlocks, int toX, int toY);

            bool checkRightRotation(const Field &field, const Piece &piece, const Blocks &toBlocks, int toX, int toY);

            bool check(const Field &field, const Piece &piece, const Blocks &blocks, int x, int y, From from);
        };
    }
}

#endif //CORE_MOVE_HPP
