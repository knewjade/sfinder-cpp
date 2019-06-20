#ifndef NN_ACTIVATIONS_HPP
#define NN_ACTIVATIONS_HPP

namespace nn {
    struct Sigmoid {
        static constexpr double a = 1.0;

        static double f(double e) {
            return 1.0 / (1.0 + exp(-a * e));
        }

        static double df(double e) {
            return a * e * (1.0 - e);
        }
    };

    struct Tanh {
        static double f(double e) {
            return std::tanh(e);
        }

        static double df(double e) {
            return 1.0 - e * e;
        }
    };

    struct Linear {
        static double f(double e) {
            return e;
        }

        static double df(double e) {
            return 1.0;
        }
    };
}

#endif //NN_ACTIVATIONS_HPP
