#include <iostream>
#include <string>
#include <random>
#include <chrono>

#include "core/moves.hpp"
#include "core/field.hpp"
#include "core/types.hpp"
#include "finder/perfect_clear.hpp"

template<int N>
std::array<core::PieceType, N> toPieces(int value) {
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

void benchmark() {
    using namespace std::literals::string_literals;

    std::cout << "# Initialize" << std::endl;

    auto start = std::chrono::system_clock::now();

    auto field = core::createField(
            "XX________"s +
            "XX________"s +
            "XXX______X"s +
            "XXXXXXX__X"s +
            "XXXXXX___X"s +
            "XXXXXXX_XX"s +
            ""
    );

    auto factory = core::Factory::create();
    auto moveGenerator = core::srs::MoveGenerator(factory);
    auto finder = finder::PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

    {
        auto elapsed = std::chrono::system_clock::now() - start;
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout << time << " micro seconds" << std::endl;
    }

    std::cout << "# Measuring" << std::endl;

    const int maxDepth = 7;
    const int maxLine = 6;

    int success = 0;
    long long int totalTime = 0L;
    int max = 5040;
    for (int value = 0; value < max; ++value) {
        auto arr = toPieces<maxDepth>(value);
        auto pieces = std::vector(arr.begin(), arr.end());

        auto start2 = std::chrono::system_clock::now();

        auto result = finder.run(field, pieces, maxDepth, maxLine, false);

        if (!result.empty()) {
            success += 1;
//            std::cout << value << std::endl;
//            for (const auto &item : result) {
//                std::cout << item.pieceType << "," << item.rotateType << "," << item.x << "," << item.y << " ";
//            }
//            std::cout << "" << std::endl;
        } else {
            // 975, 2295
//            std::cout << value << std::endl;
//            for (int i = 0; i < pieces.size(); ++i) {
//                std::cout << pieces[i];
//            }
//            std::cout << "" << std::endl;
        }

        auto elapsed = std::chrono::system_clock::now() - start2;
        totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    }

    std::cout << "success: " << success << std::endl;

    std::cout << totalTime / static_cast<double>(max) << " milli seconds" << std::endl;
}

void sample() {
    using namespace std::literals::string_literals;

    auto factory = core::Factory::create();
    auto moveGenerator = core::srs::MoveGenerator(factory);
    auto finder = finder::PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

    auto field = core::createField(
            "_XXXXXX___"s +
            "_XXXXXXXXX"s +
            "XXXXXX_XXX"s +
            ""
    );

    auto pieces = std::vector{
            core::PieceType::I, core::PieceType::J, core::PieceType::J, core::PieceType::S, core::PieceType::O,
            core::PieceType::L, core::PieceType::Z, core::PieceType::T, core::PieceType::I, core::PieceType::Z
    };

    const int maxDepth = 9;
    const int maxLine = 6;
    const bool holdEmpty = false;  // If true, hold is empty at start

    auto result = finder.run(
            field, pieces, maxDepth, maxLine, holdEmpty, false, 0, finder::SearchTypes::TSpin
    );

    if (!result.empty()) {
        std::cout << "PC: success" << std::endl;

        for (const auto &item : result) {
            std::cout << item.pieceType << ","  // enum PieceType in src/core/types.hpp
                      << item.rotateType << "," // enum RotateType in src/core/types.hpp
                      << item.x << ","
                      << item.y << " "
                      << std::endl;

            field.put(factory.get(item.pieceType, item.rotateType), item.x, item.y);
            field.clearLine();

            std::cout << field.toString(maxLine) << std::endl;
        }
    } else {
        std::cout << "PC: failed" << std::endl;
    }
}

int main() {
//    benchmark();
    sample();

    return 0;
}