#pragma once

#include <algorithm>
#include <numeric>
#include <vector>

template <typename T>
class Filter {
public:
    Filter() = default;
    ~Filter() = default;

    explicit Filter(size_t windowSize = 10, bool useZeroBeforeWindow = false)
        : windowSize(windowSize)
        , useZeroBeforeWindow(useZeroBeforeWindow)
    {
    }

    void append(T value)
    {
        values.push_back(value);
        if (values.size() > windowSize) {
            values.erase(values.begin());
        }
    }
    void clear()
    {
        values.clear();
    }
    T getMedian()
    {
        if (values.size() < windowSize) {
            if (useZeroBeforeWindow) {
                return 0;
            } else {
                return values.empty() ? 0 : values.back();
            }
        }
        std::vector<T> tempValues(values);
        std::sort(tempValues.begin(), tempValues.end(), [](T a, T b) { return a < b; });
        return tempValues[tempValues.size() / 2];
    }
    T getAverage()
    {
        if (values.size() < windowSize) {
            if (useZeroBeforeWindow) {
                return 0;
            } else {
                return values.empty() ? 0 : values.back();
            }
        }
        return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    }

private:
    std::vector<T> values;
    const size_t windowSize;
    const bool useZeroBeforeWindow;
};
