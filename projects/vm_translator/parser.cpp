#include "./parser.hpp"
#include "./vm_instructions/vm-instruction.hpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace {
  constexpr char kStartOfComment = '/';
  constexpr char kSpaceChar = ' ';
  constexpr char kNewlineChar = '\n';
  constexpr char kCarriageReturnChar = '\r';

  const std::map<std::string, VMInstruction::VMInstructionType> kStringsToNoArgumentInstructionTypes {
    { "add", VMInstruction::VMInstructionType::ADD },
    { "sub", VMInstruction::VMInstructionType::SUB },
    { "neg", VMInstruction::VMInstructionType::NEG },
    { "eq", VMInstruction::VMInstructionType::EQ },
    { "gt", VMInstruction::VMInstructionType::GT },
    { "lt", VMInstruction::VMInstructionType::LT },
    { "and", VMInstruction::VMInstructionType::AND },
    { "or", VMInstruction::VMInstructionType::OR },
    { "not", VMInstruction::VMInstructionType::NOT },
    { "return", VMInstruction::VMInstructionType::RETURN }
  };

  const std::map<std::string, VMInstruction::VMInstructionType> kStringsToSingleArgumentInstructionTypes {
    { "label", VMInstruction::VMInstructionType::LABEL },
    { "goto", VMInstruction::VMInstructionType::GOTO },
    { "if-goto", VMInstruction::VMInstructionType::IFGOTO }
  };

  const std::map<std::string, VMInstruction::VMInstructionType> kStringsToMemoryInstructionTypes {
    { "push", VMInstruction::VMInstructionType::PUSH },
    { "pop", VMInstruction::VMInstructionType::POP },
  };

  const std::map<std::string, VMInstruction::VMInstructionType> kStringsToFunctionInstructionTypes {
    { "function", VMInstruction::VMInstructionType::FUNCTION },
    { "call", VMInstruction::VMInstructionType::CALL },
  };

  const std::map<std::string, VMInstruction::MemorySegmentType> kStringsToMemorySegmentTypes {
    { "local", VMInstruction::MemorySegmentType::LOCAL },
    { "argument", VMInstruction::MemorySegmentType::ARGUMENT },
    { "this", VMInstruction::MemorySegmentType::THIS },
    { "that", VMInstruction::MemorySegmentType::THAT },
    { "constant", VMInstruction::MemorySegmentType::CONSTANT },
    { "static", VMInstruction::MemorySegmentType::STATIC },
    { "pointer", VMInstruction::MemorySegmentType::POINTER },
    { "temp", VMInstruction::MemorySegmentType::TEMP },
  };
}

boost::optional<VMInstruction> ParseLine(const std::string& line) {
  std::cout << "Parsing: " << line << std::endl;
  if (line.empty() ||
      line.front() == kStartOfComment ||
      line.front() == kNewlineChar ||
      line.front() == kCarriageReturnChar) {
    return boost::none;
  }

  auto advance_next_word = [](std::string::const_iterator& it) -> std::string {
    std::stringstream ss;
    while (*it != kSpaceChar && *it != kNewlineChar && *it != kCarriageReturnChar) {
      ss << *it;
      it++;
    }
    return ss.str();
  };

  auto it = line.begin();
  std::string command = advance_next_word(it);

  if (kStringsToNoArgumentInstructionTypes.find(command)
      != kStringsToNoArgumentInstructionTypes.end()) {
    return VMInstruction(kStringsToNoArgumentInstructionTypes.at(command));
  }

  it++;
  std::string first_arg = advance_next_word(it);

  if (kStringsToSingleArgumentInstructionTypes.find(command)
      != kStringsToSingleArgumentInstructionTypes.end()) {
    return VMInstruction(kStringsToSingleArgumentInstructionTypes.at(command),
                         first_arg);
  }

  it++;
  std::string second_arg = advance_next_word(it);

  if (kStringsToMemoryInstructionTypes.find(command)
      != kStringsToMemoryInstructionTypes.end()) {
    return VMInstruction(kStringsToMemoryInstructionTypes.at(command),
                         kStringsToMemorySegmentTypes.at(first_arg),
                         std::atoi(second_arg.c_str()));
  } else {
    return VMInstruction(kStringsToFunctionInstructionTypes.at(command),
                         first_arg,
                         std::atoi(second_arg.c_str()));
  }
}
