#ifndef VM_TRANSLATOR_PARSER_HPP
#define VM_TRANSLATOR_PARSER_HPP

#include "./vm_instructions/vm-instruction.hpp"

#include <boost/optional.hpp>
#include <string>

boost::optional<VMInstruction> ParseLine(const std::string& line);

#endif
