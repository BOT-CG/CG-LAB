#pragma once

#include "NCProgram.h"

#include <string>

class NCProgramStateNone : public NCProgramState {
public:
    NCProgramStateNone() = default;
    ~NCProgramStateNone() override = default;

    bool execute(const std::string& ncCodeLine) override;
};
