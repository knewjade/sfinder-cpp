#include <random>

#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/moves.hpp"

namespace sfinder {
    class ReverseSearchTest : public ::testing::Test {
    };

    core::LineKey get4LineKey() {
        core::LineKey key = 0ULL;
        for (int y = 0; y < 4; ++y) {
            key |= core::getBitKey(y);
        }
        return key;
    }

    core::Field near(const core::Blocks &blocks, int x, int y) {
        auto field = core::Field{};
        field.put(blocks, x, y);

        auto blocksMask = field.boardLow();
        auto mask = 0ULL;

        // Left
        mask |= (blocksMask & core::getColumnMaskRightX(1)) >> 1ULL;

        // Right
        mask |= (blocksMask & core::getColumnMaskLeftX(9)) << 1ULL;

        // Down
        mask |= blocksMask >> 10ULL;

        // Up
        mask |= blocksMask << 10ULL;

        auto near = core::Field(mask & core::getRowMaskBelowY(4));
        near.remove(blocks, x, y);
        return near;
    }

    struct Operation {
        core::PieceType pieceType;
        core::RotateType rotateType;
        int x;
        int y;
    };

    struct Result {
        bool success;
        core::PieceType pieceType;
        core::Field next;
    };

    Result run(
            const core::Factory &factory, core::Field &field2, const core::Blocks &blocks, int x, int y, int height
    ) {
        auto field = core::Field{field2};

        field.remove(blocks, x, y);

//        std::cout << std::endl;
//        std::cout << field.toString(height) << std::endl;

        auto freeze = core::Field{field};

        auto deletedLine = field.clearLineReturnKey();
//        std::cout << std::hex << deletedLine << std::endl;

        auto deletedLineBelowPiece = deletedLine & core::getUsingKeyBelowY(y);
//        std::cout << std::hex << deletedLineBelowPiece << std::endl;

//        std::cout << std::endl;
//        std::cout << field.toString(height) << std::endl;

        auto reachable = core::srs::MoveGenerator(factory);
        auto flag = reachable.canReach(field, blocks.pieceType, blocks.rotateType, x,
                                     y - core::bitCount(deletedLineBelowPiece), 24);

//        std::cout << std::boolalpha << flag << std::endl;

        return {flag, blocks.pieceType, freeze};
    }

    void loop2(
            const core::Factory &factory, core::Field &field, core::Field &must, int height, int depth, std::mt19937 &mt
    ) {
        std::uniform_int_distribution<> rand08(0, 6);
        std::uniform_int_distribution<> rand03(0, 3);

        Result result;
        auto m = core::Field{must};

        do {
            Operation operation{core::PieceType::T, core::RotateType::Spawn, -1, -1};

            while (operation.x == -1) {
                auto piece = static_cast<core::PieceType>(rand08(mt));
                auto rotate = static_cast<core::RotateType>(rand03(mt));

                const auto &blocks = factory.get(piece, rotate);

                std::uniform_int_distribution<> randX(-blocks.minX, core::kFieldWidth - blocks.maxX - 1);
                auto x = randX(mt);

                std::uniform_int_distribution<> randY(-blocks.minY, height - blocks.maxY - 1);
                auto y = randY(mt);

                auto mino = core::Field{};
                mino.put(blocks, x, y);

                if (!field.contains(mino)) {
                    continue;
                }

                if (must.canMerge(mino)) {
                    continue;
                }

                operation = {piece, rotate, x, y};
            }

            {
                const auto &blocks = factory.get(operation.pieceType, operation.rotateType);
                result = run(factory, field, blocks, operation.x, operation.y, height);

                if (result.success) {
                    auto n = near(blocks, operation.x, operation.y);
                    m.merge(n);
                }
            }
        } while (!result.success);

        if (depth < 3) {
            loop2(factory, result.next, m, height, depth + 1, mt);
        } else {
            result.next.clearLine();
            std::cout << std::endl;
            std::cout << result.next.toString(height) << std::endl;
            std::cout << "↓ミノ順：ホールドなし" << std::endl;
        }

        std::cout << factory.get(result.pieceType).name << std::endl;
    }

    void loop(
            const core::Factory &factory, core::Field &field, int height, std::mt19937 &mt
    ) {
        std::uniform_int_distribution<> rand08(0, 6);
        std::uniform_int_distribution<> rand03(0, 3);

        Result result;
        auto m = core::Field{};

        do {
            Operation operation{core::PieceType::T, core::RotateType::Spawn, -1, -1};

            while (operation.x == -1) {
                auto piece = static_cast<core::PieceType>(rand08(mt));
                auto rotate = static_cast<core::RotateType>(rand03(mt));

                const auto &blocks = factory.get(piece, rotate);

                std::uniform_int_distribution<> randX(-blocks.minX, core::kFieldWidth - blocks.maxX - 1);
                auto x = randX(mt);

                std::uniform_int_distribution<> randY(-blocks.minY, height - blocks.maxY - 1);
                auto y = randY(mt);

                auto mino = core::Field{};
                mino.put(blocks, x, y);

                if (!field.contains(mino)) {
                    continue;
                }

                operation = {piece, rotate, x, y};
            }

            {
                const auto &blocks = factory.get(operation.pieceType, operation.rotateType);
                result = run(factory, field, blocks, operation.x, operation.y, height);

                if (result.success) {
                    auto n = near(blocks, operation.x, operation.y);
                    m.merge(n);
                }
            }
        } while (!result.success);

        loop2(factory, result.next, m, height, 1, mt);

        std::cout << factory.get(result.pieceType).name << std::endl;
    }

    TEST_F(ReverseSearchTest, succeed) {
        const auto factory = core::Factory::create();

        auto rd = std::random_device{};
        auto mt = std::mt19937(rd());

        auto field = core::Field{};
        auto key = get4LineKey();
        field.insertBlackLineWithKey(key);

        loop(factory, field, 4, mt);
    }
}
