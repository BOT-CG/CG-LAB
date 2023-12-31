#include "NCProgramStateG02.h"

#include "Parameter.h"

#include <iostream>

bool NCProgramStateG02::execute(const std::string& ncCodeLine)
{
    updateFeedRate(ncCodeLine);
    const auto lastPosture = this->context->currentPosture;

    if (Parameter::getInstance()->getUseSweptVolume()) {

        updatePosture(ncCodeLine);
        if (lastPosture.equal(this->context->currentPosture)) {
            return false;
        }

        const float radius = getRadius(ncCodeLine);
        // this->context->cutterSweptVolume.setArc(lastPosture, this->context->currentPosture, this->context->currentPlane, radius, true);
        this->context->cutterSweptVolume.setArcRange(lastPosture, this->context->currentPosture, this->context->currentPlane, radius, true);
        this->context->currentPathLength = this->context->cutterSweptVolume.getLength();

    } else {
        updatePostureByStepArc(ncCodeLine);
        // this->context->cutterSweptVolume.setScatter(this->context->currentPosture);
        this->context->cutterSweptVolume.setScatterRange(this->context->currentPosture);
        this->context->currentPathLength = lastPosture.center.distanceToPoint(this->context->currentPosture.center);
    }

    this->context->millingTime += this->context->currentPathLength / this->context->currentFeedRate * 60;

    // std::cout << "G02: " << this->context->currentPosture.center << " " << this->context->currentPosture.direction << std::endl;
    return true;
}
