#include <iostream>
#include <string>

#include "core/field.hpp"

int main() {
    using namespace std::literals::string_literals;
    using namespace core;

    auto field = Field();
    auto factory = Factory::create();

    field.put(factory.get(PieceType::S, RotateType::Right), 0, 1);
    field.put(factory.get(PieceType::O, RotateType::Spawn), 3, 0);
    field.put(factory.get(PieceType::Z, RotateType::Spawn), 4, 2);
    field.put(factory.get(PieceType::L, RotateType::Reverse), 6, 1);
    field.put(factory.get(PieceType::J, RotateType::Reverse), 7, 2);
    field.put(factory.get(PieceType::I, RotateType::Left), 9, 1);

    {
        auto piece = factory.get(PieceType::T);
        RotateType rotateType = RotateType::Left;
        RotateType nextRotate = static_cast<RotateType>((rotateType + 3) % 4);
        int fromX = 2;
        int fromY = 2;

        int index = rotateLeft(field, piece, rotateType, nextRotate, fromX, fromY);

        auto offset = piece.leftOffsets[index];
        field.put(piece.blocks[nextRotate], fromX + offset.x, fromY + offset.y);
    }

    std::cout << field.toString(20) << std::endl;
    std::cout << "========="s << std::endl;

    field.clearLine();

    field.put(factory.get(PieceType::J, RotateType::Right), 0, 1);
    field.put(factory.get(PieceType::L, RotateType::Left), 3, 3);
    field.put(factory.get(PieceType::O, RotateType::Spawn), 7, 0);
    field.put(factory.get(PieceType::S, RotateType::Right), 5, 1);
    field.put(factory.get(PieceType::Z, RotateType::Right), 4, 3);
    field.put(factory.get(PieceType::I, RotateType::Spawn), 7, 2);

    {
        auto piece = factory.get(PieceType::T);
        RotateType rotateType = RotateType::Right;
        RotateType nextRotate = static_cast<RotateType>((rotateType + 3) % 4);

        int fromX = 0;
        int fromY = 4;

        int index = rotateLeft(field, piece, rotateType, nextRotate, fromX, fromY);

        auto offset = piece.leftOffsets[index];
        int nextX = offset.x;
        int nextY = fromY + offset.y;

        {
            RotateType rotateType = nextRotate;
            RotateType nextRotate = static_cast<RotateType>((rotateType + 3) % 4);

            int fromX = nextX;
            int fromY = nextY;

            int index = rotateLeft(field, piece, rotateType, nextRotate, fromX, fromY);

            auto offset = piece.leftOffsets[index];
            field.put(piece.blocks[nextRotate], fromX + offset.x, fromY + offset.y);
        }
    }

    field.clearLine();

    std::cout << field.toString(20) << std::endl;
    std::cout << "========="s << std::endl;
}