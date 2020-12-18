#include "./label-instruction.hpp"

#include <sstream>

namespace {
  constexpr char kFromSeedPrefix[] = "FROM_SEED_";
}

LabelInstruction::LabelInstruction(size_t seed) {
  std::stringstream label;
  label << kFromSeedPrefix << std::to_string(seed);
  label_ = label.str();
}

std::string LabelInstruction::ToString() const {
  std::stringstream assembly;
  assembly << "(" << GetSymbol() << ")";
  return assembly.str();
}

std::string LabelInstruction::GetSymbol() const {
  return label_;
}
