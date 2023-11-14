#pragma once

#include "NCProgram.h"

#include <string>

class NCProgramStateG03 : public NCProgramState {
public:
    NCProgramStateG03() = default;
    ~NCProgramStateG03() override = default;

    bool execute(const std::string& ncCodeLine) override;
};
