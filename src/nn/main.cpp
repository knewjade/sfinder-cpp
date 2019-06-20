#include <iostream>
#include <array>
#include <random>
#include <cmath>

#include "perceptron.hpp"
#include "activations.hpp"

namespace nn {
    template<int N>
    void show(const std::array<double, N> &array) {
        for (const auto &item : array) {
            std::cout << item << ",";
        }
        std::cout << std::endl;
    }

    int main() {
        std::mt19937 mt19937{std::random_device{}()};

        // Defines
        const int INPUT = 2;
        const int MID = 6;
        const int OUTPUT = 1;

        // Create train data
        const int N = 50;
        std::uniform_real_distribution<double> dist(-1.0, 1.0);
        std::array<std::array<double, INPUT + 1>, N> xbList{};
        std::array<std::array<double, OUTPUT>, N> yList{};

        for (int index = 0; index < N; ++index) {
            auto &x = xbList[index];
            auto sum = 0.0;
            for (int i = 0; i < INPUT; ++i) {
                auto v = dist(mt19937);
                x[i] = v;
                sum += v * v;
            }
            x[INPUT] = 1.0;  // Bias

            auto &y = yList[index];
            y[0] = sum;
        }

        // Create network
        auto perceptron1 = Perceptron<INPUT + 1, MID, Tanh, true>::create(mt19937);
        auto perceptron2 = Perceptron<MID + 1, OUTPUT, Linear, false>::create(mt19937);

        // Train
        const double learningRate = 0.1;

        for (int epoch = 0; epoch < 100000; ++epoch) {
            for (int n = 0; n < N; ++n) {
                auto &xb = xbList[n];
                auto &y = yList[n];

                const auto forward1b = perceptron1.forward(xb);
                const auto output = perceptron2.forward(forward1b);

                // -----------------------------

                std::array<double, OUTPUT> error2{};
                for (int index = 0; index < OUTPUT; ++index) {
                    error2[index] = output[index] - y[index];
                }

                perceptron2.deactivation(error2, output);

                std::array<double, MID + 1> error1b = perceptron2.error(error2);

                perceptron2.backword(forward1b, error2, learningRate);

                perceptron1.deactivation(error1b, forward1b);

                // 上の層がないので必要なし
                // std::array<double, 8> next = perceptron1.error(error2);

                perceptron1.backword(xb, error1b, learningRate);
            }

            double sqsum = 0.0;
            for (int n = 0; n < N; ++n) {
                auto &xb = xbList[n];
                auto &y = yList[n];

                const auto forward1b = perceptron1.forward(xb);
                const auto output = perceptron2.forward(forward1b);

                for (int index = 0; index < OUTPUT; ++index) {
                    auto v = output[index] - y[index];
                    sqsum += v * v / 2.0;
                }
            }

//        std::cout << epoch << " / sqsum = " << sqsum << std::endl;
        }

        // Predict
        {
            std::array<double, INPUT + 1> xb{0.3, 0.8, 1.0};
            const auto forward1b = perceptron1.forward(xb);
            const auto output = perceptron2.forward(forward1b);
            std::cout << "predict = ";
            show<OUTPUT>(output);
        }

        return 0;
    }
}