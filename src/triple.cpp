#include "triple.hpp"

namespace core {
    int TripleSample::add(int x, int y, int z) {
        auto s1 = Sample();
        auto s2 = DoubleSample();
        return s1.add(x) + s2.add(y, z);
    }
}
