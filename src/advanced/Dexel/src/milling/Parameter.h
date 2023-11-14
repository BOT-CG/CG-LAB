#pragma once

#include "utils.h"

#include <atomic>
#include <mutex>

class ParameterInstance {
public:
    ParameterInstance() = default;
    ~ParameterInstance() = default;

    inline float getCenterStep() const
    {
        return centerStep.load(std::memory_order_acquire);
    }

    inline void setCenterStep(float value)
    {
        centerStep.store(value, std::memory_order_release);
    }

    inline float getDirectionStep() const
    {
        return directionStep.load(std::memory_order_acquire);
    }

    inline void setDirectionStep(float value)
    {
        directionStep.store(degreeToRadian(value), std::memory_order_release);
    }

    inline bool getUseSweptVolume() const
    {
        return useSweptVolume.load(std::memory_order_acquire);
    }

    inline void setUseSweptVolume(bool value)
    {
        useSweptVolume.store(value, std::memory_order_release);
    }

    inline int getMaxLinePerExecute() const
    {
        return maxLinePerExecute.load(std::memory_order_acquire);
    }

    inline void setMaxLinePerExecute(int value)
    {
        maxLinePerExecute.store(value, std::memory_order_release);
    }

private:
    std::atomic<float> centerStep;
    std::atomic<float> directionStep;
    std::atomic<bool> useSweptVolume;
    std::atomic<int> maxLinePerExecute;
};

class Parameter {
public:
    static ParameterInstance* getInstance()
    {
        static std::once_flag flag;
        std::call_once(flag, []() {
            instance = new ParameterInstance();
        });
        return instance;
    }

protected:
    Parameter() = default;
    ~Parameter() = default;

private:
    static ParameterInstance* instance;
};

inline ParameterInstance* Parameter::instance = nullptr;
