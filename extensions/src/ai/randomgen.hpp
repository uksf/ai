#pragma once

class randomgen {
public:
    randomgen() = default;
    randomgen(std::mt19937::result_type seed) : eng(seed) {}
    size_t randomBetween(size_t min, size_t max) {
        return std::uniform_int_distribution<size_t>{min, max}(eng);
    }

private:
    std::mt19937 eng {
        std::random_device{}()
    };
};