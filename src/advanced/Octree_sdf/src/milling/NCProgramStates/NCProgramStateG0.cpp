#include "NCProgramStateG0.h"

#include <iostream>

bool NCProgramStateG0::execute(const std::string& ncCodeLine)
{
    updatePosture(ncCodeLine);

    // std::cout << "G0: " << this->context->currentPosture.center << " " << this->context->currentPosture.direction << std::endl;
    return false;
}
