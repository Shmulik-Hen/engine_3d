#pragma once
#include <fstream>
#include <jsoncpp/json/json.h>
#include "config_document.h"

namespace config_ns
{

AST parse_json(const std::string&, const std::string&);

} // namespace config_ns
