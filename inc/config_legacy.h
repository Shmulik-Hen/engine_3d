#pragma once
#include <fstream>
#include "config_document.h"

namespace config_ns
{

AST parse_legacy(const std::string&, const std::string&);

} // namespace config_ns
