#pragma once

#include "NCProgram.h"

#include <string>

class NCProgramStateG0 : public NCProgramState {
public:
    NCProgramStateG0() = default;
    ~NCProgramStateG0() override = default;

    bool execute(const std::string& ncCodeLine) override;
};
