#include "./label-instruction.hpp"

#include <sstream>

namespace {
  constexpr char kLabelPrefix[] = "LABEL";
}

std::string LabelInstruction::ToString() const {
  std::stringstream assembly;
  assembly << "(" << GetSymbol() << ")";
  return assembly.str();
}

std::string LabelInstruction::GetSymbol() const {
  std::stringstream symbol;
  symbol << kLabelPrefix << std::to_string(seed_);
  return symbol.str();
}
