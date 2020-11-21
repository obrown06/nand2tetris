#ifndef NAND2TETRIS_PROJECTS_06_ASSEMBLER_AINSTRUCTION_H
#define NAND2TETRIS_PROJECTS_06_ASSEMBLER_AINSTRUCTION_H

#include "./instruction-interface.h"

#include <string>

class AInstruction : InstructionInterface {
  public:
    AInstruction(const std::string& value) : value_(value) {}
    std::string GetValue() const;
    std::string ToBinary() const;
    void ReplaceSymbols(const SymbolTable& table);
    bool HoldsSymbol() const;
  private:
    std::string value_;
};

#endif
