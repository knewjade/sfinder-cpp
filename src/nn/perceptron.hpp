#ifndef NN_PERCEPTRON_HPP
#define NN_PERCEPTRON_HPP

#include <array>

namespace nn {
    template<int F, int Bn, typename Act, bool Bias>
    class Perceptron {
    public:
        static constexpr int B = Bias ? Bn + 1 : Bn;

        static Perceptron<F, Bn, Act, Bias> create(std::mt19937 &mt19937) {
            std::array<double, F * Bn> weights{};
            std::uniform_real_distribution<double> dist(-1.0, 1.0);

            for (auto &weight : weights) {
                weight = dist(mt19937);
            }

            return Perceptron(weights);
        }

        std::array<double, F * Bn> weights_;

        explicit Perceptron(const std::array<double, F * Bn> &weights) : weights_(weights) {}

        std::array<double, B> forward(const std::array<double, F> &x) {
            std::array<double, B> output{};
            for (int backIndex = 0; backIndex < Bn; ++backIndex) {
                auto headIndex = F * backIndex;
                auto result = 0.0;
                for (int forwardIndex = 0; forwardIndex < F; ++forwardIndex) {
                    result += weights_[headIndex + forwardIndex] * x[forwardIndex];
                }
                output[backIndex] = Act::f(result);
            }

            if constexpr (Bias) {
                output[B - 1] = 1.0;
            }

            return output;
        }

        void deactivation(std::array<double, B> &e, const std::array<double, B> &o) {
            for (int index = 0; index < B; ++index) {
                e[index] *= Act::df(o[index]);
            }
        }

        void backword(const std::array<double, F> &x, const std::array<double, B> &e, double l) {
            for (int backIndex = 0; backIndex < Bn; ++backIndex) {
                auto headIndex = F * backIndex;
                for (int forwardIndex = 0; forwardIndex < F; ++forwardIndex) {
                    weights_[headIndex + forwardIndex] -= l * e[backIndex] * x[forwardIndex];
                }
            }
        }


        std::array<double, F> error(const std::array<double, B> &e) {
            std::array<double, F> output{};
            for (int backIndex = 0; backIndex < Bn; ++backIndex) {
                auto headIndex = F * backIndex;
                for (int forwardIndex = 0; forwardIndex < F; ++forwardIndex) {
                    output[forwardIndex] += weights_[headIndex + forwardIndex] * e[backIndex];
                }
            }

            return output;
        }
    };
}

#endif //NN_PERCEPTRON_HPP
