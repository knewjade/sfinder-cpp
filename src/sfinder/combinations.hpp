#ifndef FINDER_COMBINATIONS_HPP
#define FINDER_COMBINATIONS_HPP

#include <vector>

namespace sfinder {
    template<typename Container, typename Op>
    void combination(const Container &container, int pop, Op op) {
        using valueType = typename Container::value_type;

        const int size = container.size();

        if (pop == size) {
            auto elements = std::vector<valueType>(container.cbegin(), container.cend());
            op(elements);
            return;
        }

        if (size - pop < pop) {
            combinationReverse(container, size - pop, op);
            return;
        }

        auto indexes = std::vector<int>(pop);
        auto elements = std::vector<valueType>(pop);

        for (int i = 0; i < pop; ++i) {
            int selectedIndex = pop - i - 1;
            indexes[i] = selectedIndex;
            elements[i] = container[selectedIndex];
        }

        op(elements);

        while (true) {
            int i = 0;
            for (i = 0; i < pop; ++i) {
                auto nextIndex = indexes[i] + 1;
                if (size - i <= nextIndex) {
                    continue;
                }

                // Next state decided

                indexes[i] = nextIndex;
                elements[i] = container[nextIndex];

                for (int j = i - 1; 0 <= j; --j) {
                    int selectedIndex = indexes[j + 1] + 1;
                    indexes[j] = selectedIndex;
                    elements[j] = container[selectedIndex];
                }

                op(elements);
                break;
            }

            if (i == pop) {
                break;
            }
        }
    }

    template<typename Container, typename Op>
    void combinationReverse(const Container &container, int pop, Op op) {
        using valueType = typename Container::value_type;

        const int size = container.size();

        auto indexes = std::vector<int>(pop);
        auto elements = std::vector<valueType>(size - pop);

        for (int i = 0; i < pop; ++i) {
            int selectedIndex = pop - i - 1;
            indexes[i] = selectedIndex;
            elements[i] = container[selectedIndex];
        }

        for (int i = pop; i < size; ++i) {
            elements[i - pop] = container[i];
        }

        op(elements);

        while (true) {
            int i = 0;
            for (i = 0; i < pop; ++i) {
                {
                    auto nextIndex = indexes[i] + 1;
                    if (size - i <= nextIndex) {
                        continue;
                    }

                    // Next state decided

                    indexes[i] = nextIndex;
                }

                for (int j = i - 1; 0 <= j; --j) {
                    int selectedIndex = indexes[j + 1] + 1;
                    indexes[j] = selectedIndex;
                }

                {
                    int j = pop - 1;
                    int selectedIndex = indexes[j];
                    for (int k = 0, nextIndex = 0; k < size; ++k) {
                        if (k == selectedIndex) {
                            j -= 1;
                            selectedIndex = indexes[j];

                            if (j < 0) {
                                for (int j = k + 1; j < size; ++j) {
                                    auto &value = container[j];
                                    if (elements[nextIndex] == value) {
                                        break;
                                    }
                                    elements[nextIndex] = value;
                                    nextIndex += 1;
                                }

                                break;
                            }

                            continue;
                        }

                        elements[nextIndex] = container[k];
                        nextIndex += 1;
                    }
                }

                op(elements);
                break;
            }

            if (i == pop) {
                break;
            }
        }
    }
}

#endif //FINDER_COMBINATIONS_HPP
