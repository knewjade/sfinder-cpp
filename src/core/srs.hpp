#ifndef CORE_SRS_HPP
#define CORE_SRS_HPP

#include "field.hpp"
#include "types.hpp"

namespace core::srs {
    int right(
            const Field &field, const Piece &piece, RotateType fromRotate, RotateType toRotate, int fromX, int fromY
    );

    int right(
            const Field &field, const Piece &piece, RotateType fromRotate, const Blocks &toBlocks, int fromX, int fromY
    );

    int left(
            const Field &field, const Piece &piece, RotateType fromRotate, RotateType toRotate, int fromX, int fromY
    );

    int left(
            const Field &field, const Piece &piece, RotateType fromRotate, const Blocks &toBlocks, int fromX, int fromY
    );
}

#endif //CORE_SRS_HPP
