#pragma once

#include "NCProgram.h"

#include <string>

class NCProgramStateG01 : public NCProgramState {
public:
    NCProgramStateG01() = default;
    ~NCProgramStateG01() override = default;

    bool execute(const std::string& ncCodeLine) override;
};
