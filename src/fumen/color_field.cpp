#include "color_field.hpp"

namespace fumen {
    ColorConverter ColorConverter::create() noexcept {
        std::array<ColorType, 7> pieceToColor{};
        pieceToColor[core::PieceType::T] = ColorType::T;
        pieceToColor[core::PieceType::I] = ColorType::I;
        pieceToColor[core::PieceType::O] = ColorType::O;
        pieceToColor[core::PieceType::S] = ColorType::S;
        pieceToColor[core::PieceType::Z] = ColorType::Z;
        pieceToColor[core::PieceType::L] = ColorType::L;
        pieceToColor[core::PieceType::J] = ColorType::J;

        std::array<core::PieceType, 9> colorToPiece{};
        colorToPiece[ColorType::T] = core::PieceType::T;
        colorToPiece[ColorType::I] = core::PieceType::I;
        colorToPiece[ColorType::O] = core::PieceType::O;
        colorToPiece[ColorType::S] = core::PieceType::S;
        colorToPiece[ColorType::Z] = core::PieceType::Z;
        colorToPiece[ColorType::L] = core::PieceType::L;
        colorToPiece[ColorType::J] = core::PieceType::J;

        return ColorConverter(pieceToColor, colorToPiece);
    }

    ColorType ColorConverter::parseToColorType(core::PieceType piece) const {
        return pieceToColor_[piece];
    }

    core::PieceType ColorConverter::parseToPieceType(ColorType color) const {
        return colorToPiece_[color];
    }

    void ColorField::setBlock(ColorType colorType, int x, int y) {
        field[y * 10 + x] = colorType;
    }

    int ColorField::at(int x, int y) const {
        return field[y * 10 + x];
    }

    bool ColorField::existsAt(int x, int y) const {
        return field[y * 10 + x] != ColorType::Empty;
    }

    void ColorField::put(const core::Blocks &blocks, int x, int y) {
        for (const auto &point : blocks.points) {
            int nx = x + point.x;
            int ny = y + point.y;
            field[ny * 10 + nx] = kDefaultConverter.parseToColorType(blocks.pieceType);
        }
    }

    void ColorField::clearLine() {
        int currentY = 0;
        for (int y = 0; y < height; ++y) {
            bool filled = true;
            for (int x = 0; x < 10; ++x) {
                if (!existsAt(x, y)) {
                    filled = false;
                    break;
                }
            }

            if (!filled) {
                if (currentY != y) {
                    for (int x = 0; x < 10; ++x) {
                        field[currentY * 10 + x] = field[y * 10 + x];
                    }
                }
                currentY += 1;
            }
        }

        for (int y = currentY; y < height; ++y) {
            for (int x = 0; x < 10; ++x) {
                field[currentY * 10 + x] = ColorType::Empty;
            }
        }
    }
}