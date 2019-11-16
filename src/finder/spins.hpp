#ifndef FINDER_SPINS_HPP
#define FINDER_SPINS_HPP

#include "../core/piece.hpp"
#include "../core/moves.hpp"
#include "../core/types.hpp"

namespace finder {
    enum TSpinShapes {
        NoShape,
        RegularShape,
        MiniOrTSTShape,
    };

    TSpinShapes getTSpinShape(const core::Field &field, int x, int y, core::RotateType rotateType);

    int getAttackIfTSpin(
            core::srs_rotate_end::Reachable &reachable, const core::Factory &factory, const core::Field &field,
            core::PieceType pieceType, const core::Move &move, int numCleared, bool b2b
    );
}

#endif //FINDER_SPINS_HPP
