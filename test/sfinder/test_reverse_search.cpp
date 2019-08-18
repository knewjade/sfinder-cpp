#include <random>
#include <cmath>

#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/moves.hpp"
#include "core/pieces.hpp"
#include "fumen/color_field.hpp"
#include "fumen/parser.hpp"

namespace main {
    class ReverseSearchTest : public ::testing::Test {
    };

    struct Configure {
        core::Factory &factory;
        core::srs::MoveGenerator &moveGenerator;
        std::mt19937 &mt19937;
        int height;
    };

    struct Operation {
        core::PieceType pieceType;
        core::RotateType rotateType;
        int x;
        int y;
        double score;
    };

    struct Result {
        bool success;
        core::PieceType pieceType;
        core::Field next;
    };

    auto elements = std::vector<fumen::Element>{};

    core::Field initField(int height) {
        auto field = core::Field{};
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < core::kFieldWidth; ++x) {
                field.setBlock(x, y);
            }
        }
        return field;
    }

    void k(core::Field &field, int x, int y, int height) {
        if (!field.isEmpty(x, y)) {
            field.removeBlock(x, y);

            if (0 <= x - 1) {
                k(field, x - 1, y, height);
            }

            if (x + 1 < core::kFieldWidth) {
                k(field, x + 1, y, height);
            }

            if (0 <= y - 1) {
                k(field, x, y - 1, height);
            }

            if (y + 1 < height) {
                k(field, x, y + 1, height);
            }
        }
    }

    void k2(core::Field &field, int x, int y, int height) {
        if (field.isEmpty(x, y)) {
            field.setBlock(x, y);

            if (0 <= x - 1) {
                k2(field, x - 1, y, height);
            }

            if (x + 1 < core::kFieldWidth) {
                k2(field, x + 1, y, height);
            }

            if (0 <= y - 1) {
                k2(field, x, y - 1, height);
            }

            if (y + 1 < height) {
                k2(field, x, y + 1, height);
            }
        }
    }

    double validate(
            const Configure &configure, const core::Field &field, const core::Blocks &blocks, int x, int y
    ) {
        auto mino = core::Field{};
        mino.put(blocks, x, y);

        if (!field.contains(mino)) {
            return -1;
        }

        auto freeze = core::Field{field};
        freeze.reduce(mino);

        auto deletedLine = freeze.clearLineReturnKey();
        auto deletedLineBelowPiece = deletedLine & core::getUsingKeyBelowY(y);

        auto canReach = configure.moveGenerator.canReach(
                freeze, blocks.pieceType, blocks.rotateType, x, y - core::bitCount(deletedLineBelowPiece), 24
        );

        if (!canReach) {
            return -1;
        }

        int deletedLineCount = core::bitCount(deletedLine);
        int height = configure.height - deletedLineCount;

        auto freeze2 = core::Field{freeze};
        for (int x = 0; x < core::kFieldWidth; ++x) {
            k(freeze2, x, 0, height);
        }

        auto floating = 0;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < core::kFieldWidth; ++x) {
                if (!freeze2.isEmpty(x, y)) {
                    floating += 1;
                    k(freeze2, x, y, height);
                }
            }
        }

        auto freeze3 = core::Field{freeze};
        auto space = 0;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < core::kFieldWidth; ++x) {
                if (freeze3.isEmpty(x, y)) {
                    space += 1;
                    k2(freeze3, x, y, height);
                }
            }
        }

        auto hole = 0;
        for (int x = 0; x < core::kFieldWidth; ++x) {
            auto isHole = false;
            for (int y = height - 1; 0 <= y; --y) {
                if (!freeze.isEmpty(x, y)) {
                    isHole = true;
                } else if (isHole) {
                    hole += 1;
                }
            }
        }

        auto square = 0;
        for (int x = 0; x < core::kFieldWidth; ++x) {
            auto count = 0;
            for (int y = 0; y < height; ++y) {
                if (x - 1 < 0 || !freeze.isEmpty(x - 1, y)) {
                    count++;
                }

                if (core::kFieldWidth <= x + 1 || !freeze.isEmpty(x + 1, y)) {
                    count++;
                }

                if (y - 1 < 0 || !freeze.isEmpty(x, y - 1)) {
                    count++;
                }

                if (height <= y + 1 || !freeze.isEmpty(x, y + 1)) {
                    count++;
                }

                if (3 < count) {
                    square++;
                }
            }
        }

        auto distance = 0.0;
        for (int x = 0; x < core::kFieldWidth; ++x) {
            for (int y = 0; y < height; ++y) {
                if (freeze.isEmpty(x, y)) {
                    for (int x2 = 0; x2 < core::kFieldWidth; ++x2) {
                        for (int y2 = 0; y2 < height; ++y2) {
                            if (freeze.isEmpty(x2, y2)) {
                                auto d = sqrt((x2 - x) * (x2 - x) + (y2 - y) * (y2 - y));
                                if (distance < d) {
                                    distance = d;
                                }
                            }
                        }
                    }
                }
            }
        }

        auto score = (10 - space - 1) * 10 - floating * 2000 - hole * 10 - deletedLineCount * 10 + square * 1 - distance * 10;

//        std::cout << freeze.toString(height) << std::endl;
//        std::cout << score << std::endl;

        if (score <= 0) {
            return -1.0;
        }

        double s = score / 50.0;
        return s * s;
    }

    Operation select(std::mt19937 &mt19937, const std::vector<Operation> &operations) {
        assert(!operations.empty());

        auto sum = 0.0;
        for (const auto &operation : operations) {
            sum += operation.score;
        }

        assert(0.0 < sum);
        std::uniform_real_distribution<> random(0, sum);
        auto selectedScore = random(mt19937);

        auto prev = 0.0;
        for (const auto &operation : operations) {
            if (prev + operation.score >= selectedScore) {
                return operation;
            }
            prev += operation.score;
        }

        assert(false);
        return operations.at(0);
    }

    template<int MAX_COUNT>
    Operation generateNextOperation(
            const Configure &configure, const core::PieceCounter pieceCounter, const core::Field &field, int count
    ) {
        if (MAX_COUNT <= count) {
            return Operation{core::PieceType::T, core::RotateType::Spawn, -1, -1};
        }

        std::vector<core::PieceType> pieceTypes{};
        for (const auto pieceType : core::kAllPieceType) {
            if (!pieceCounter.containsAll(core::PieceCounter::create(pieceType))) {
                continue;
            }
            pieceTypes.emplace_back(pieceType);
        }

        if (pieceTypes.empty()) {
            return generateNextOperation<MAX_COUNT>(configure, pieceCounter, field, count + 1);
        }

        assert(!pieceTypes.empty());
        std::uniform_int_distribution<> randomPiece(0, pieceTypes.size() - 1);

        auto pieceIndex = randomPiece(configure.mt19937);
        auto pieceType = pieceTypes.at(pieceIndex);
        auto piece = configure.factory.get(pieceType);

        auto candidates = std::vector<Operation>{};

        auto bit = piece.uniqueShapeRotate;
        do {
            auto next = bit & (bit - 1U);
            auto nextRotateType = core::rotateBitToVal[bit & ~next];

            auto &blocks = configure.factory.get(pieceType, nextRotateType);

            for (int y = -blocks.minY; y < core::kFieldWidth - blocks.maxY; ++y) {
                for (int x = -blocks.minX; x < core::kFieldWidth - blocks.maxX; ++x) {
                    auto score = validate(configure, field, blocks, x, y);
                    if (score <= 0.0) {
                        continue;
                    }

                    auto operation = Operation{
                            pieceType,
                            nextRotateType,
                            x,
                            y,
                            score
                    };
                    candidates.emplace_back(operation);
                }
            }

            bit = next;
        } while (bit != 0);

        if (candidates.empty()) {
            return generateNextOperation<MAX_COUNT>(configure, pieceCounter, field, count + 1);
        }

        return select(configure.mt19937, candidates);
    }

    template<int MAX_DEPTH, int MAX_COUNT>
    bool run(
            const Configure &configure, const core::Field field,
            std::vector<core::PieceType> &pieces, const core::PieceCounter pieceCounter, int depth
    ) {
        auto operation = generateNextOperation<MAX_COUNT>(configure, pieceCounter, field, 0);
        if (operation.x == -1) {
            return false;
        }

        auto blocks = configure.factory.get(operation.pieceType, operation.rotateType);

        auto freeze = core::Field{field};
        freeze.remove(blocks, operation.x, operation.y);

        pieces.emplace_back(operation.pieceType);

        if (depth < MAX_DEPTH - 1) {
            auto p = pieceCounter - core::PieceCounter::create(operation.pieceType);
            auto success = run<MAX_DEPTH, MAX_COUNT>(configure, freeze, pieces, p, depth + 1);
            if (!success) {
                pieces.pop_back();
                return run<MAX_DEPTH, MAX_COUNT>(configure, field, pieces, pieceCounter, depth);
            }
        } else {
            std::cout << std::endl;
            std::cout << freeze.toString(configure.height) << std::endl;

            freeze.clearLine();

            auto ss = std::stringstream{};
            ss << "#Q=[](" << configure.factory.get(pieces.at(pieces.size() - 1)).name << ")";
            for (int index = pieces.size() - 2; 0 <= index; --index) {
                ss << configure.factory.get(pieces.at(index)).name;
            }

            auto colorField = fumen::ColorField(24);
            for (int y = 0; y < configure.height; ++y) {
                for (int x = 0; x < core::kFieldWidth; ++x) {
                    if (!freeze.isEmpty(x, y)) {
                        colorField.setBlock(fumen::ColorType::Gray, x, y);
                    }
                }
            }

            auto comment = ss.str();
            elements.push_back(
                    fumen::Element(
                            fumen::ColorType::Empty, core::RotateType::Spawn, 0, 0, true, colorField, comment
                    )
            );
            
            std::cout << comment << std::endl;
        }

        return true;
    }

    TEST_F(ReverseSearchTest, succeed) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);

        auto randomDevice = std::random_device{};
        auto height = 4;

        auto mt19937 = std::mt19937(randomDevice());
        auto configure = Configure{
                factory,
                moveGenerator,
                mt19937,
                height
        };

        for (int count = 0; count < 15; ++count) {
            auto field = initField(height);
            auto pieceCounter = core::PieceCounter::create<7>(core::kAllPieceType);
            auto pieces = std::vector<core::PieceType>();
            run<4, 5>(configure, field, pieces, pieceCounter, 0);
        }

        auto converter = fumen::ColorConverter::create();
        auto parser = fumen::Parser(configure.factory, converter);
        std::cout << "https://knewjade.github.io/fumen-for-mobile/#?d=" << "v115@" << parser.encode(elements)
                  << std::endl;
    }
}
