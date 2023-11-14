#pragma once

#include "CutterSweptVolume.h"
#include "DexelMilling.h"
//#include "DexelGrid.h"
#include "Vector3D.h"

#include <vector>

struct filterMRR {
    std::vector<float> MRRWindow;
    std::vector<float> MillingTimeWindow;
    std::vector<float> MRR_Weight;

    int windowSize = 10;
    float averageMRR = 0;
    float averageWeight = 100000;
    //构造
    filterMRR() = default;
    bool appendMRR(float mrr, float time)
    {
        if (MRRWindow.size() == 0) {
            MRRWindow.push_back(mrr);
            return true;
        }
        if (((mrr / time) > averageWeight * 3) || mrr > averageMRR * 5) {
            return false;
        }

        MRRWindow.push_back(mrr);
        MillingTimeWindow.push_back(time);
        MRR_Weight.push_back(mrr / time);
        if (MRRWindow.size() >= windowSize) {
            MRRWindow.erase(MRRWindow.begin());
            MillingTimeWindow.erase(MillingTimeWindow.begin());
        }
        //计算MillingMRRWindow的平均值
        averageMRR = std::accumulate(MRRWindow.begin(), MRRWindow.end(), 0.0) / MRRWindow.size();
        averageWeight = std::accumulate(MRR_Weight.begin(), MRR_Weight.end(), 0.0) / MRR_Weight.size();
        return true;
    }
};
class Workpiece {
public:
    Workpiece() = default;
    explicit Workpiece(float length, float width, float height, const Vector3Df& position);

    ~Workpiece() = default;

    inline Vector3Df getPosition() const
    {
        return position;
    }
    inline float getScale() const
    {
        return scale;
    }
    void subtract(const DexelCutterSweptVolume& cutterSweptVolume);
    void subtract(std::vector<DexelCutterSweptVolume>& cutterSweptVolumes);
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;

public:
    MRR_Dexel mrrDexel;
    std::vector<float> CurrentTime;
    std::vector<float> CurrentVolume;
    std::vector<float> MillingMRRWindow;
    float TotalTime = 0;
    std::vector<float> MillingVolume;
    std::vector<float> MillingTime;
    // filterMRR filterMRR;

private:
    // static constexpr size_t DEXEL_GRID_SIZE = 1024;
    // DexelGrid<DEXEL_GRID_SIZE, DEXEL_GRID_SIZE> dexelGrid;

    Vector3Df position;
    float scale;
};
