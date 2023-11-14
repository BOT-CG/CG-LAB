#include "NCProgramStateNone.h"

#include <regex>

bool NCProgramStateNone::execute(const std::string& ncCodeLine)
{
    this->context->currentLine++;
    return false;
}
