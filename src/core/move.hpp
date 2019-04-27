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
        void search(
                std::vector<Move> &moves, const Factory &factory, const Field &field, const PieceType &pieceType,
                int validHeight
        );
    }
}

#endif //CORE_MOVE_HPP
