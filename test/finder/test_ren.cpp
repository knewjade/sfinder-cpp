#include "gtest/gtest.h"

#include <chrono>
#include <vector>
#include <array>
#include <random>
#include <fstream>

#include "core/moves.hpp"
#include "core/field.hpp"
#include "finder/ren.hpp"
#include "finder/perfect_clear.hpp"

namespace finder {
    using namespace std::literals::string_literals;

    class RenTest : public ::testing::Test {
    };

    TEST_F(RenTest, case1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = ren::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XXXXX__XXX"s +
                "XXXX__XXXX"s +
                ""
        );

        {
            auto pieces = std::vector{core::PieceType::S};
            EXPECT_EQ(1, finder.run(field, pieces, false));
        }
    }

    TEST_F(RenTest, case2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = ren::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +

                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +

                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +

                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +
                "____XXXXXX"s +
                "_XXXXXXXXX"s +
                ""
        );

        auto start = std::chrono::system_clock::now();

        {
            auto pieces = std::vector{
                    core::PieceType::S, core::PieceType::Z, core::PieceType::O, core::PieceType::T,
                    core::PieceType::I, core::PieceType::L, core::PieceType::J,
                    core::PieceType::S, core::PieceType::O, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::I, core::PieceType::L, core::PieceType::J
            };
            EXPECT_EQ(14, finder.run(field, pieces, false));
        }

        auto elapsed = std::chrono::system_clock::now() - start;
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout << time << " micro seconds" << std::endl;
    }

    template<int N>
    std::array<core::PieceType, N> toPieces(int value) {
        static_assert(0 < N && N <= 7);

        int arr[N];

        for (int index = N - 1; 0 <= index; --index) {
            int scale = 7 - index;
            arr[index] = value % scale;
            value /= scale;
        }

        for (int select = N - 2; 0 <= select; --select) {
            for (int adjust = select + 1; adjust < N; ++adjust) {
                if (arr[select] <= arr[adjust]) {
                    arr[adjust] += 1;
                }
            }
        }

        std::array<core::PieceType, N> pieces = {};
        for (int index = 0; index < N; ++index) {
            pieces[index] = static_cast<core::PieceType>(arr[index]);
        }

        return pieces;
    }

    struct Serialize {
        uint64_t f1;
        uint64_t f2;
        uint64_t f3;
        uint64_t f4;
        double score;
    };

    class Randoms {
    public:
        Randoms(const std::mt19937 &mt19937) :
                mt19937_(mt19937),
                dist0to1_(std::uniform_real_distribution<double>(0.0, 1.0)),
                dist1to6_(std::uniform_int_distribution<int>(1, 6)),
                dist0to8_(std::uniform_int_distribution<int>(0, 8)),
                dist0to9_(std::uniform_int_distribution<int>(0, 9)),
                dist5to19_(std::uniform_int_distribution<int>(5, 19)),
                dist0to5039_(std::uniform_int_distribution<int>(0, 5039)),
                distM1to6_(std::uniform_int_distribution<int>(-1, 6)) {}

        bool nextBool(double truePercent) {
            return dist0to1_(mt19937_) < truePercent;
        }

        // Include max
        int nextLine(int max) {
            auto line = dist1to6_(mt19937_);
            if (max < line) {
                return max;
            }
            return line;
        }

        int firstX() {
            return dist0to9_(mt19937_);
        }

        int nextX(int currentX) {
            auto x = dist0to8_(mt19937_);
            return x < currentX ? x : x + 1;
        }

        int nextHeight() {
            return dist5to19_(mt19937_);
        }

        // -1 is empty
        core::PieceType nextPiece() {
            return static_cast<core::PieceType>(distM1to6_(mt19937_));
        }

        void pushPieces(std::vector<core::PieceType> &pieces) {
            auto newPieces = toPieces<7>(dist0to5039_(mt19937_));
            for (const auto &piece : newPieces) {
                pieces.emplace_back(piece);
            }
        }

    private:
        std::mt19937 mt19937_;
        std::uniform_real_distribution<double> dist0to1_;
        std::uniform_int_distribution<int> dist1to6_;
        std::uniform_int_distribution<int> dist0to8_;
        std::uniform_int_distribution<int> dist0to9_;
        std::uniform_int_distribution<int> dist5to19_;
        std::uniform_int_distribution<int> dist0to5039_;
        std::uniform_int_distribution<int> distM1to6_;
    };

    void fill(int emptyX, core::Field &field, int y) {
        for (int x = 0; x < 10; ++x) {
            if (x != emptyX) {
                field.setBlock(x, y);
            }
        }
    }

    void receiveAttacks(core::Field &field, Randoms &randoms, int maxY) {
        int x = randoms.firstX();
        int y = 0;
        while (y < maxY) {
            auto line = randoms.nextLine(maxY - y);

            if (randoms.nextBool(0.9)) {
                x = randoms.nextX(x);
            }

            fill(x, field, y);
            y++;

            for (int i = 0; i < line - 1; ++i) {
                if (randoms.nextBool(0.3)) {
                    x = randoms.nextX(x);
                }

                fill(x, field, y);
                y++;
            }
        }
    }

    TEST_F(RenTest, caseN) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = ren::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto randoms = Randoms(std::mt19937{std::random_device{}()});

        const int scoreSize = 100;
        const int lowScoreIndex = 20;
        const int highScoreIndex = 80;
        const int dataSize = 1000000;
        std::array<int, scoreSize> scores{};

        // Output
        const auto fileName = "file.bin";
        std::ofstream fout(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(fout);

        for (int resultIndex = 0; resultIndex < dataSize; ++resultIndex) {
            if (resultIndex % 10000 == 0) {
                std::cout << resultIndex << std::endl;
            }

            auto field = core::Field{};
            auto height = randoms.nextHeight();
            receiveAttacks(field, randoms, height);
//            std::cout << field.toString(20) << std::endl;

            // Challenge
            for (int challenge = 0; challenge < scoreSize; ++challenge) {
                auto pieces = std::vector<core::PieceType>{};

                auto firstPiece = randoms.nextPiece();
                if (0 <= firstPiece) {
                    pieces.emplace_back(firstPiece);
                }

                for (int i = 0; i < 3; ++i) {
                    randoms.pushPieces(pieces);
                }

                scores[challenge] = finder.run(field, pieces, false);
            }

            // Sum
            std::sort(scores.begin(), scores.end());
            double average = 0.0;
            for (int l = lowScoreIndex; l < highScoreIndex; ++l) {
                average += scores[l];
            }
            average /= (highScoreIndex - lowScoreIndex);
//            std::cout << average << std::endl;

            auto result = Serialize{
                    field.boards[0],
                    field.boards[1],
                    field.boards[2],
                    field.boards[3],
                    average
            };
            fout.write(reinterpret_cast<char *>(&result), sizeof(Serialize));
        }

        fout.close();
    }

    TEST_F(RenTest, caseNIn) {
        const int dataSize = 1000000;

        const auto fileName = "file.bin";
        std::ifstream istream(fileName, std::ios::binary);

        ASSERT_TRUE(istream.is_open());

        int count = 0;
        double max = 0.0;

        for (int index = 0; index < dataSize; ++index) {
            Serialize serialize{};
            istream.read(reinterpret_cast<char *>(&serialize), sizeof(Serialize));

            if (10.0 <= serialize.score) {
                count++;
            }

            if (max <= serialize.score) {
                max = serialize.score;
            }
        }

        std::cout << max << std::endl;

        istream.close();
    }

    core::LineKey getBitLineKey(int lowerY, int line) {
        core::LineKey key = 0ULL;
        for (int i = 0; i < line; ++i) {
            auto y = lowerY + i;
            key |= core::getBitKey(y);
        }
        return key;
    }

    struct Goal {
        const int fromX;
        const int fromY;
        const int rotateIndex;
    };

    struct Candidate {
        const core::Field blocks;
        const core::Field notAllowed;
    };

    class Searcher {
    public:
        Searcher(
                const core::Piece &piece, const core::Blocks &fromBlocks, const core::Blocks &toBlocks
        ) : piece_(piece), fromBlocks_(fromBlocks), toBlocks_(toBlocks) {
        }

        void nextLeftRotate(std::vector<Candidate> &results, const Candidate &candidate, const Goal goal) {
            assert(candidate.blocks.canMerge(candidate.notAllowed));
//            std::cout << candidate.blocks.toString(15) << std::endl;
//            std::cout << candidate.notAllowed.toString(15) << std::endl;
            next(results, candidate, goal, piece_.leftOffsets, -1);
        }

        void nextRightRotate(std::vector<Candidate> &results, const Candidate &candidate, const Goal goal) {
            assert(candidate.blocks.canMerge(candidate.notAllowed));
//            std::cout << candidate.blocks.toString(15) << std::endl;
//            std::cout << candidate.notAllowed.toString(15) << std::endl;
            next(results, candidate, goal, piece_.rightOffsets, -1);
        }

    private:
        const core::Piece &piece_;
        const core::Blocks &fromBlocks_;
        const core::Blocks &toBlocks_;

        void next(
                std::vector<Candidate> &results, const Candidate &candidate, const Goal goal,
                const std::array<core::Offset, 20> &offsets_, const int prevRotateIndex
        ) {
            int head = 5 * fromBlocks_.rotateType;
            for (int currentRotateIndex = head; currentRotateIndex < head + 5; ++currentRotateIndex) {
                auto &offsets = offsets_[currentRotateIndex];
                int toX = goal.fromX + offsets.x;
                int toY = goal.fromY + offsets.y;

                if (
                        0 <= toX + toBlocks_.minX
                        && toX + toBlocks_.maxX < core::kFieldWidth
                        && 0 <= toY + toBlocks_.minY
                        && toY + toBlocks_.maxY < core::kMaxFieldHeight
                        && candidate.blocks.canPut(toBlocks_, toX, toY)) {

                    if (currentRotateIndex == goal.rotateIndex ) {
                        auto f = core::Field{};
                        for (int iy = toY +toBlocks_.minY; iy <= toY +toBlocks_.maxY; ++iy) {
                            for (int ix = 0; ix < core::kFieldWidth; ++ix) {
                                f.setBlock(ix, iy);
                            }
                        }
                        f.reduce(candidate.notAllowed);
//                        f.merge(candidate.blocks);
                        f.put(toBlocks_, toX, toY);
                        auto c = f.clearLineReturnNum();

                        if (0 < c) {
                            std::cout << candidate.blocks.toString(15) << std::endl;
                            std::cout << candidate.notAllowed.toString(15) << std::endl;

                            results.emplace_back(Candidate{candidate.blocks, candidate.notAllowed});
                        }
                    } else if (prevRotateIndex < currentRotateIndex && currentRotateIndex < goal.rotateIndex) {
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, true, true, true, true);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, true, true, true, false);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, true, true, false, true);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, true, true, false, false);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, true, false, true, true);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, true, false, true, false);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, true, false, false, true);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, true, false, false, false);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, false, true, true, true);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, false, true, true, false);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, false, true, false, true);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, false, true, false, false);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, false, false, true, true);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, false, false, true, false);
                        fill(results, candidate, goal, offsets_,currentRotateIndex, toX, toY, false, false, false, true);
                    }

                    return;
                }
            }
        }

        void fill(
                std::vector<Candidate> &results, const Candidate &candidate, const Goal goal,
                const std::array<core::Offset, 20> &offsets,
                int currentRotateIndex, int toX, int toY, bool b1, bool b2, bool b3, bool b4
        ) {
            assert(b1 || b2 || b3 || b4);

            auto freezeBlocks = core::Field{candidate.blocks};
            auto freezeNotAllowed = core::Field{candidate.notAllowed};

            auto &points = toBlocks_.points;

            {
                auto point = points[0];
                auto x = toX + point.x;
                auto y = toY + point.y;

                if (b1) {
                    if (!freezeNotAllowed.isEmpty(x, y)) {
                        return;
                    }
                    freezeBlocks.setBlock(x, y);
                } else {
                    freezeNotAllowed.setBlock(x, y);
                }
            }

            {
                auto point = points[1];
                auto x = toX + point.x;
                auto y = toY + point.y;

                if (b2) {
                    if (!freezeNotAllowed.isEmpty(x, y)) {
                        return;
                    }
                    freezeBlocks.setBlock(x, y);
                } else {
                    freezeNotAllowed.setBlock(x, y);
                }
            }

            {
                auto point = points[2];
                auto x = toX + point.x;
                auto y = toY + point.y;

                if (b3) {
                    if (!freezeNotAllowed.isEmpty(x, y)) {
                        return;
                    }
                    freezeBlocks.setBlock(x, y);
                } else {
                    freezeNotAllowed.setBlock(x, y);
                }
            }

            {
                auto point = points[3];
                auto x = toX + point.x;
                auto y = toY + point.y;

                if (b4) {
                    if (!freezeNotAllowed.isEmpty(x, y)) {
                        return;
                    }
                    freezeBlocks.setBlock(x, y);
                } else {
                    freezeNotAllowed.setBlock(x, y);
                }
            }

            if (candidate.blocks != freezeBlocks) {
                assert(candidate.blocks.numOfAllBlocks() < freezeBlocks.numOfAllBlocks());
                auto freeze = Candidate{freezeBlocks, freezeNotAllowed};
                next(results, freeze, goal, offsets, currentRotateIndex);
            }
        }
    };

    void createMask2(
            const core::Factory &factory, const Candidate &candidate,
            const core::Piece &piece, const core::Blocks &toBlocks, int toX, int toY
    ) {
        std::cout << candidate.blocks.toString(12) << std::endl;

        auto results = std::vector<Candidate>{};

        // Rotate left
        {
            auto fromRotateType = static_cast<core::RotateType>((toBlocks.rotateType + 1) % 4);
            auto &fromBlocks = factory.get(core::PieceType::T, fromRotateType);

            int head = 5 * fromRotateType;
            for (int i = head; i < head + 5; ++i) {
                auto &offsets = piece.leftOffsets[i];
                int fromX = toX - offsets.x;
                int fromY = toY - offsets.y;

                assert(0 <= fromX + fromBlocks.minX);
                assert(fromX + fromBlocks.maxX < core::kFieldWidth);
                assert(0 <= fromY + fromBlocks.minY);
                assert(fromY + fromBlocks.maxY < core::kMaxFieldHeight);

                if (candidate.blocks.canPut(fromBlocks, fromX, fromY)) {
                    auto searcher = Searcher(piece, fromBlocks, toBlocks);

                    auto freezeNotAllowed = core::Field{candidate.notAllowed};
                    freezeNotAllowed.put(fromBlocks, fromX, fromY);

                    auto freezeCandidate = Candidate{candidate.blocks, freezeNotAllowed};
                    auto goal = Goal{fromX, fromY, i};
                    searcher.nextLeftRotate(results, freezeCandidate, goal);
                }
            }
        }

        // Rotate right
        {
            auto fromRotateType = static_cast<core::RotateType>((toBlocks.rotateType + 3) % 4);
            auto &fromBlocks = factory.get(core::PieceType::T, fromRotateType);

            int head = 5 * fromRotateType;
            for (int i = head; i < head + 5; ++i) {
                auto &offsets = piece.rightOffsets[i];
                int fromX = toX - offsets.x;
                int fromY = toY - offsets.y;

                assert(0 <= fromX + fromBlocks.minX);
                assert(fromX + fromBlocks.maxX < core::kFieldWidth);
                assert(0 <= fromY + fromBlocks.minY);
                assert(fromY + fromBlocks.maxY < core::kMaxFieldHeight);

                if (candidate.blocks.canPut(fromBlocks, fromX, fromY)) {
                    auto searcher = Searcher(piece, fromBlocks, toBlocks);

                    auto freezeNotAllowed = core::Field{candidate.notAllowed};
                    freezeNotAllowed.put(fromBlocks, fromX, fromY);

                    auto freezeCandidate = Candidate{candidate.blocks, freezeNotAllowed};
                    auto goal = Goal{fromX, fromY, i};
                    searcher.nextRightRotate(results, freezeCandidate, goal);
                }
            }
        }

        for (const auto &result : results) {
            std::cout << result.blocks.toString(12) << std::endl;
        }
    }

    void createMask(
            const core::Factory &factory, const Candidate &candidate
    ) {
        int x = 4;
        int y = 4;

        auto &piece = factory.get(core::PieceType::T);

        for (int toRotate = 0; toRotate < 4; ++toRotate) {
            auto toRotateType = static_cast<core::RotateType>(toRotate);
            auto &toBlocks = factory.get(core::PieceType::T, toRotateType);

            auto notAllowed = core::Field{candidate.notAllowed};
            notAllowed.put(toBlocks, x, y);

            {
                auto freezeBlocks = core::Field{candidate.blocks};
                auto freezeNotAllowed = core::Field{notAllowed};

                freezeNotAllowed.setBlock(x + 1, y + 1);
                freezeBlocks.setBlock(x - 1, y + 1);
                freezeBlocks.setBlock(x + 1, y - 1);
                freezeBlocks.setBlock(x - 1, y - 1);

                if (freezeBlocks != candidate.blocks) {
                    auto freeze = Candidate{freezeBlocks, freezeNotAllowed};
                    createMask2(factory, freeze, piece, toBlocks, x, y);
                }
            }

            {
                auto freezeBlocks = core::Field{candidate.blocks};
                auto freezeNotAllowed = core::Field{notAllowed};

                freezeBlocks.setBlock(x + 1, y + 1);
                freezeNotAllowed.setBlock(x - 1, y + 1);
                freezeBlocks.setBlock(x + 1, y - 1);
                freezeBlocks.setBlock(x - 1, y - 1);

                if (freezeBlocks != candidate.blocks) {
                    auto freeze = Candidate{freezeBlocks, freezeNotAllowed};
                    createMask2(factory, freeze, piece, toBlocks, x, y);
                }
            }

            {
                auto freezeBlocks = core::Field{candidate.blocks};
                auto freezeNotAllowed = core::Field{notAllowed};

                freezeBlocks.setBlock(x + 1, y + 1);
                freezeBlocks.setBlock(x - 1, y + 1);
                freezeNotAllowed.setBlock(x + 1, y - 1);
                freezeBlocks.setBlock(x - 1, y - 1);

                if (freezeBlocks != candidate.blocks) {
                    auto freeze = Candidate{freezeBlocks, freezeNotAllowed};
                    createMask2(factory, freeze, piece, toBlocks, x, y);
                }
            }

            {
                auto freezeBlocks = core::Field{candidate.blocks};
                auto freezeNotAllowed = core::Field{notAllowed};

                freezeBlocks.setBlock(x + 1, y + 1);
                freezeBlocks.setBlock(x - 1, y + 1);
                freezeBlocks.setBlock(x + 1, y - 1);
                freezeNotAllowed.setBlock(x - 1, y - 1);

                if (freezeBlocks != candidate.blocks) {
                    auto freeze = Candidate{freezeBlocks, freezeNotAllowed};
                    createMask2(factory, freeze, piece, toBlocks, x, y);
                }
            }

            {
                auto freezeBlocks = core::Field{candidate.blocks};

                freezeBlocks.setBlock(x + 1, y + 1);
                freezeBlocks.setBlock(x - 1, y + 1);
                freezeBlocks.setBlock(x + 1, y - 1);
                freezeBlocks.setBlock(x - 1, y - 1);

                if (freezeBlocks != candidate.blocks) {
                    auto freeze = Candidate{freezeBlocks, notAllowed};
                    createMask2(factory, freeze, piece, toBlocks, x, y);
                }
            }
        }
    }

    TEST_F(RenTest, sample) {
        auto factory = core::Factory::create();

        auto blocks = core::Field{};
        auto notAllowed = core::Field{};
        auto candidate = Candidate{blocks, notAllowed};

        createMask(factory, candidate);
    }
}
