#ifndef FINDER_TYPES_HPP
#define FINDER_TYPES_HPP

#include <vector>

#include "../core/types.hpp"

namespace finder {
    struct Operation {
        core::PieceType pieceType;
        core::RotateType rotateType;
        int x;
        int y;
    };

    using Solution = std::vector<Operation>;
    inline const Solution kNoSolution = std::vector<Operation>();
}

#endif //FINDER_TYPES_HPP
