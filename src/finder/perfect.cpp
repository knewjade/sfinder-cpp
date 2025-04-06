#include "perfect.hpp"

namespace finder {
    TSpinShapes getTSpinShape(const core::Field &field, int x, int y, core::RotateType rotateType) {
        assert(0 <= x && x < FIELD_WIDTH);
        assert(0 <= y);

        auto b1 = isBlock(field, x - 1, y - 1);
        auto b2 = isBlock(field, x - 1, y + 1);
        auto b3 = isBlock(field, x + 1, y - 1);
        auto b4 = isBlock(field, x + 1, y + 1);

        auto shape = (b1 || b2) && (b1 || b3) && (b1 || b4) && (b2 || b3) && (b2 || b4) && (b3 || b4);
        if (!shape) {
            return TSpinShapes::NoShape;
        }

        switch (rotateType) {
            case core::RotateType::Spawn:
                return b1 && b3 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
            case core::RotateType::Right:
                return b1 && b2 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
            case core::RotateType::Reverse:
                return b2 && b4 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
            case core::RotateType::Left:
                return b3 && b4 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
        }

        assert(false);
    }
}
