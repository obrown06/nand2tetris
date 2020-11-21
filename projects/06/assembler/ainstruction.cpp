#include "./ainstruction.h"

#include <algorithm>
#include <sstream>

namespace {
  constexpr char kInstructionPrefix[] = "0";
  constexpr char kOneStr[] = "1";
  constexpr char kZeroStr[] = "0";
  constexpr int kAddressLength = 15;
}

std::string AInstruction::GetValue() const {
  return value_;
}

bool AInstruction::HoldsSymbol() const {
  for (char c : value_) {
    if (!isdigit(c)) {
      return true;
    }
  }
  return false;
}

std::string AInstruction::ToBinary() const {
  std::stringstream binary;
  int int_val = std::atoi(value_.c_str());
  for (int i = 0; i < kAddressLength; i++) {
    std::string suffix = int_val & 1 ? kOneStr : kZeroStr;
    binary << suffix;
    int_val >>= 1;
  }
  binary << kInstructionPrefix;
  std::string str = binary.str();
  std::reverse(str.begin(), str.end());
  return str;
}

void AInstruction::ReplaceSymbols(const SymbolTable& table) {
  if (table.find(value_) != table.end()) {
    value_ = table.at(value_);
  }
}
