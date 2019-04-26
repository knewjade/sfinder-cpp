#include "double.hpp"

namespace core {
    int DoubleSample::add(int x, int y) {
        auto s = Sample();
        return s.add(x) + s.add(y);
    }
}