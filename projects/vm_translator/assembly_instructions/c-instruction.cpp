#include "./c-instruction.hpp"

#include <sstream>

namespace {
  constexpr char kAssignmentOperator = '=';
  constexpr char kJumpSeparator = ';';
}

std::string CInstruction::ToString() const {
  std::stringstream assembly;
  if (dest_) {
    assembly << *dest_ << kAssignmentOperator;
  }
  assembly << comp_;
  if (jump_) {
    assembly << kJumpSeparator << *jump_;
  }
  return assembly.str();
}
