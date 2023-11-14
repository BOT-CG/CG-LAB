#pragma once

#include "Cutter.h"
#include "CutterFactory.h"
#include "CutterSweptVolume.h"
#include "Posture.h"
#include "Vector3D.h"

#include <memory>
#include <regex>
#include <string>
#include <vector>

class NCProgramContext;

class NCProgramState {
public:
    NCProgramState() = default;
    virtual ~NCProgramState() = default;

    void setContext(NCProgramContext* context)
    {
        this->context = context;
    }

    void load(const std::string& ncCodeLine);
    virtual bool execute(const std::string& ncCodeLine) = 0;

protected:
    NCProgramContext* context = nullptr;
    Posture getNextPosture(const std::string& ncCodeLine) const;
    void updatePosture(const std::string& ncCodeLine);
    void updatePostureByStepLine(const std::string& ncCodeLine);
    void updatePostureByStepArc(const std::string& ncCodeLine);
    void updateFeedRate(const std::string& ncCodeLine);
    float getRadius(const std::string& ncCodeLine) const;
};

class NCProgramContext {
public:
    NCProgramContext();
    explicit NCProgramContext(NCProgramState* state)
        : state(nullptr)
    {
        this->changeState(state);
    }
    ~NCProgramContext()
    {
        delete state;
    }

    void changeState(NCProgramState* state);
    void changeStateGCode(int gCode);
    void changePlane(int gCode);
    void load(const std::string& ncCodeLine)
    {
        state->load(ncCodeLine);
    }
    bool execute(const std::string& ncCodeLine)
    {
        return state->execute(ncCodeLine);
    }
    std::unique_ptr<Cutter> getCurrentCutter() const
    {
        return cutters[currentCutterIndex]->clone();
    }
    void generateCurrentCutterMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
    {
        cutters[currentCutterIndex]->generateMesh(vertices, normals);
    }

    using Plane = PostureArcInterpolator::Plane;
    Posture currentPosture;
    std::vector<std::unique_ptr<Cutter>> cutters;
    DexelCutterSweptVolume cutterSweptVolume;
    size_t currentCutterIndex = 0;
    size_t currentLine = 0;
    Plane currentPlane = Plane::XY;
    float currentFeedRate = 0.0f;
    float currentPathLength = 0.0f;
    float millingTime = 0.0f;
    bool cutterChanged = false;
    std::unique_ptr<PostureLineInterpolator> postureLineInterpolator;
    std::unique_ptr<PostureArcInterpolator> postureArcInterpolator;

private:
    NCProgramState* state = nullptr;
};

class NCProgram {
public:
    NCProgram() = default;
    ~NCProgram() = default;

    inline size_t getCurrentLine() const
    {
        return context.currentLine;
    }
    std::vector<std::string> getNCCodeLines() const
    {
        return ncCodeLines;
    }
    Posture getCurrentPosture() const
    {
        return context.currentPosture;
    }

    float getMillingTime() const
    {
        return context.millingTime;
    }
    float getCurrentMillingTimeCost() const
    {
        if (context.currentFeedRate == 0) {
            return 0;
        }

        return context.currentPathLength / context.currentFeedRate;
    }
    bool isCutterChanged() const
    {
        return context.cutterChanged;
    }
    void generateCurrentCutterMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
    {
        context.generateCurrentCutterMesh(vertices, normals);
    }
    void addCutter(float d, float r, float e, float f, float alpha, float beta, float h)
    {
        context.cutters.push_back(CutterFactory::create(d, r, e, f, alpha, beta, h));
    }
    void setWorkpieceOffset(const Vector3Df& offset)
    {
        context.cutterSweptVolume.setOffset(offset);
    }
    void setWorkpieceScale(float scale)
    {
        context.cutterSweptVolume.setScale(scale);
    }

    void load(const std::string& ncCode);
    bool execute(bool& shouldSubtract, DexelCutterSweptVolume& cutterSweptVolume);
    bool execute(std::vector<DexelCutterSweptVolume>& cutterSweptVolumes);
    void clear();
    void reset();

private:
    NCProgramContext context;
    std::vector<std::string> ncCodeLines;
};
