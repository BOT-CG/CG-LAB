#pragma once

#include "NCProgram.h"

#include <string>

class NCProgramStateG02 : public NCProgramState {
public:
    NCProgramStateG02() = default;
    ~NCProgramStateG02() override = default;

    bool execute(const std::string& ncCodeLine) override;
};
