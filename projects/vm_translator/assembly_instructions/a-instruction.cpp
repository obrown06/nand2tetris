#include "./a-instruction.hpp"

#include <sstream>

namespace {
  constexpr char kAInstructionIdentifier = '@';
}

std::string AInstruction::ToString() const {
  std::stringstream assembly;
  assembly << kAInstructionIdentifier << address_;
  return assembly.str();
}
