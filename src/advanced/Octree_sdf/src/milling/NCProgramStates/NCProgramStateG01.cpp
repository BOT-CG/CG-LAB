#include "NCProgramStateG01.h"

#include "Parameter.h"

#include <iostream>

bool NCProgramStateG01::execute(const std::string& ncCodeLine)
{
    updateFeedRate(ncCodeLine);
    const auto lastPosture = this->context->currentPosture;

    if (Parameter::getInstance()->getUseSweptVolume()) {
        updatePosture(ncCodeLine);
        if (lastPosture.equal(this->context->currentPosture)) {
            return false;
        }

        this->context->cutterSweptVolume.setLine(lastPosture, this->context->currentPosture);

    } else {
        updatePostureByStepLine(ncCodeLine);
        this->context->cutterSweptVolume.setScatter(this->context->currentPosture);
    }

    this->context->currentPathLength = lastPosture.center.distanceToPoint(this->context->currentPosture.center);
    this->context->millingTime += this->context->currentPathLength / this->context->currentFeedRate * 60;
    // std::cout << "G1: " << this->context->currentPosture.center << " " << this->context->currentPosture.direction << std::endl;
    return true;
}
