#ifndef NAND2TETRIS_PROJECTS_06_ASSEMBLER_INSTRUCTION_INTERFACE_H
#define NAND2TETRIS_PROJECTS_06_ASSEMBLER_INSTRUCTION_INTERFACE_H

#include "./symbol-table.h"

#include <map>
#include <string>

class InstructionInterface {
  public:
    virtual std::string ToBinary() const = 0;
};

#endif
