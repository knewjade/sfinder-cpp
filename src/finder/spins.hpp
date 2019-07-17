#ifndef CORE_SPINS_HPP
#define CORE_SPINS_HPP

#include "./types.hpp"
#include "../core/factory.hpp"
#include "../core/moves.hpp"

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

    int getAttackIfTSpin(
            core::srs_rotate_end::Reachable &reachable, const core::Factory &factory, const core::Field &field,
            core::PieceType pieceType, core::RotateType rotateType, int x, int y, int numCleared, bool b2b
    );
}

#endif //CORE_SPINS_HPP
