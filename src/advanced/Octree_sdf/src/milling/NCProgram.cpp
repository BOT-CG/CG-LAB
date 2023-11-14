#include "NCProgram.h"

#include "NCProgramStateG0.h"
#include "NCProgramStateG01.h"
#include "NCProgramStateG02.h"
#include "NCProgramStateG03.h"
#include "NCProgramStateNone.h"
#include "Parameter.h"

#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>

void NCProgramState::load(const std::string& ncCodeLine)
{
    if (ncCodeLine.empty()) {
        return;
    }

    std::smatch match;

    std::regex regexCutter("T([0-9]+)"), regexM6("M6|M06");
    if (std::regex_search(ncCodeLine, match, regexCutter) && std::regex_search(ncCodeLine, regexM6)) {
        int cutterIndex = std::stoi(match[1]);
        this->context->currentCutterIndex = std::clamp(cutterIndex - 1, 0, (int)this->context->cutters.size() - 1);
        this->context->cutterSweptVolume.setCutter(this->context->getCurrentCutter());
        this->context->cutterChanged = true;
    }

    std::regex regexGCode("G([0-9]+)");
    if (std::regex_search(ncCodeLine, match, regexGCode)) {
        int gCode = std::stoi(match[1]);
        this->context->changeStateGCode(gCode);
        this->context->changePlane(gCode);
    }

    // TODO: may cause bug

    // std::sregex_iterator it(ncCodeLine.begin(), ncCodeLine.end(), regexGCode);
    // std::sregex_iterator end;
    // while (it != end) {
    //     std::smatch match = *it;
    //     int gCode = std::stoi(match[1]);
    //     this->context->changeStateGCode(gCode);
    //     this->context->changePlane(gCode);
    //     it++;
    // }
}

Posture NCProgramState::getNextPosture(const std::string& ncCodeLine) const
{
    Posture nextPosture = this->context->currentPosture;
    std::regex regexXYZ[3] = {
        std::regex("X(-?[0-9]+\\.?[0-9]*)"),
        std::regex("Y(-?[0-9]+\\.?[0-9]*)"),
        std::regex("Z(-?[0-9]+\\.?[0-9]*)")
    };
    std::smatch match;
    for (int i = 0; i < 3; i++) {
        if (std::regex_search(ncCodeLine, match, regexXYZ[i])) {
            nextPosture.center[i] = std::stof(match[1]);
        }
    }

    if (typeid(*this) == typeid(NCProgramStateG02) || typeid(*this) == typeid(NCProgramStateG03)) {
        return nextPosture;
    }

    std::regex regexIJK[3] = {
        std::regex("I(-?[0-9]+\\.?[0-9]*)"),
        std::regex("J(-?[0-9]+\\.?[0-9]*)"),
        std::regex("K(-?[0-9]+\\.?[0-9]*)")
    };
    for (int i = 0; i < 3; i++) {
        if (std::regex_search(ncCodeLine, match, regexIJK[i])) {
            nextPosture.direction[i] = std::stof(match[1]);
        }
    }
    return nextPosture;
}

void NCProgramState::updatePosture(const std::string& ncCodeLine)
{
    this->context->currentPosture = getNextPosture(ncCodeLine);
    this->context->currentLine++;
}

void NCProgramState::updatePostureByStepLine(const std::string& ncCodeLine)
{
    Posture nextPosture = getNextPosture(ncCodeLine);
    if (this->context->postureLineInterpolator == nullptr) {
        const float centerStep = Parameter::getInstance()->getCenterStep();
        const float directionStep = Parameter::getInstance()->getDirectionStep();
        this->context->postureLineInterpolator = std::make_unique<PostureLineInterpolator>(this->context->currentPosture, nextPosture, centerStep, directionStep);
    }

    if (this->context->postureLineInterpolator->getPostureByStep(this->context->currentPosture)) {
        this->context->currentLine++;
        this->context->postureLineInterpolator = nullptr;
    }
}

void NCProgramState::updatePostureByStepArc(const std::string& ncCodeLine)
{
    Posture nextPosture = getNextPosture(ncCodeLine);
    if (this->context->postureArcInterpolator == nullptr) {
        const float centerStep = Parameter::getInstance()->getCenterStep();
        const float directionStep = Parameter::getInstance()->getDirectionStep();
        const float radius = getRadius(ncCodeLine);
        bool isClockwise;
        if (typeid(*this) == typeid(NCProgramStateG02)) {
            isClockwise = true;
        } else if (typeid(*this) == typeid(NCProgramStateG03)) {
            isClockwise = false;
        } else {
            std::cerr << "Error: NCProgramState::updatePostureByStepArc: Invalid state." << std::endl;
            return;
        }

        this->context->postureArcInterpolator = std::make_unique<PostureArcInterpolator>(this->context->currentPosture, nextPosture, this->context->currentPlane, radius, isClockwise, centerStep, directionStep);
    }

    if (this->context->postureArcInterpolator->getPostureByStep(this->context->currentPosture)) {
        this->context->currentLine++;
        this->context->postureArcInterpolator = nullptr;
    }
}

void NCProgramState::updateFeedRate(const std::string& ncCodeLine)
{
    std::smatch match;
    std::regex regexF("F([0-9]+\\.?[0-9]*)");
    if (std::regex_search(ncCodeLine, match, regexF)) {
        this->context->currentFeedRate = std::stof(match[1]);
    }
}

float NCProgramState::getRadius(const std::string& ncCodeLine) const
{
    float radius;
    std::smatch match;
    std::regex regexR("R(-?[0-9]+\\.?[0-9]*)");
    if (std::regex_search(ncCodeLine, match, regexR)) {
        radius = std::stof(match[1]);
    } else {
        std::regex regexIJK[3] = {
            std::regex("I(-?[0-9]+\\.?[0-9]*)"),
            std::regex("J(-?[0-9]+\\.?[0-9]*)"),
            std::regex("K(-?[0-9]+\\.?[0-9]*)")
        };
        Vector3Df center = this->context->currentPosture.center;
        for (int i = 0; i < 3; i++) {
            if (std::regex_search(ncCodeLine, match, regexIJK[i])) {
                center[i] += std::stof(match[1]);
            }
        }
        radius = (center - this->context->currentPosture.center).length();
    }
    return radius;
}

NCProgramContext::NCProgramContext()
    : state(nullptr)
{
    this->changeState(new NCProgramStateNone());
}

void NCProgramContext::changeState(NCProgramState* state)
{
    if (this->state != nullptr) {
        delete this->state;
    }
    this->state = state;
    this->state->setContext(this);
}

void NCProgramContext::changeStateGCode(int gCode)
{
    switch (gCode) {
    case 0:
        this->changeState(new NCProgramStateG0());
        break;
    case 1:
        this->changeState(new NCProgramStateG01());
        break;
    case 2:
        this->changeState(new NCProgramStateG02());
        break;
    case 3:
        this->changeState(new NCProgramStateG03());
        break;
    default:
        this->changeState(new NCProgramStateNone());
        break;
    }
}

void NCProgramContext::changePlane(int gCode)
{
    switch (gCode) {
    case 17:
        this->currentPlane = Plane::XY;
        break;
    case 18:
        this->currentPlane = Plane::ZX;
        break;
    case 19:
        this->currentPlane = Plane::YZ;
        break;
    default:
        break;
    }
}

void NCProgram::load(const std::string& ncCode)
{
    context.currentLine = 0;
    ncCodeLines.clear();
    std::istringstream iss(ncCode);
    std::string line;
    while (std::getline(iss, line)) {
        ncCodeLines.push_back(line);
    }
}

bool NCProgram::execute(bool& shouldSubtract, CutterSweptVolume& cutterSweptVolume)
{
    // std::cout << context.currentLine << ": ";
    if (context.currentLine >= ncCodeLines.size()) {
        return false;
    }
    // std::cout << ncCodeLines[context.currentLine] << std::endl;
    context.load(ncCodeLines[context.currentLine]);

    shouldSubtract = context.execute(ncCodeLines[context.currentLine]);
    if (shouldSubtract) {
        cutterSweptVolume = context.cutterSweptVolume;
    }

    return true;
}

bool NCProgram::execute(std::vector<CutterSweptVolume>& cutterSweptVolumes)
{
    if (context.currentLine >= ncCodeLines.size()) {
        return false;
    }

    context.cutterChanged = false;
    cutterSweptVolumes.clear();
    const int maxLinePerExecute = Parameter::getInstance()->getMaxLinePerExecute();
    for (int i = 0; i < maxLinePerExecute && context.currentLine < ncCodeLines.size(); i++) {
        context.load(ncCodeLines[context.currentLine]);
        bool shouldSubtract = context.execute(ncCodeLines[context.currentLine]);
        if (shouldSubtract) {
            cutterSweptVolumes.push_back(context.cutterSweptVolume);
        }
    }

    return true;
}

void NCProgram::clear()
{
    reset();
    context.cutters.clear();
    ncCodeLines.clear();
}

void NCProgram::reset()
{
    context.currentLine = 0;
    context.currentCutterIndex = 0;
    context.currentPosture = Posture { { 0, 0, 0 }, { 0, 0, 1.0f } };
    context.postureLineInterpolator = nullptr;
    context.postureArcInterpolator = nullptr;
    context.millingTime = 0;
    context.changeState(new NCProgramStateNone());
}
