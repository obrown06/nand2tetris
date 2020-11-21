#ifndef NAND2TETRIS_PROJECTS_06_ASSEMBLER_CINSTRUCTION_H
#define NAND2TETRIS_PROJECTS_06_ASSEMBLER_CINSTRUCTION_H

#include <boost/optional.hpp>

#include "./instruction-interface.h"

#include <string>

class CInstruction : InstructionInterface {
  public:
    CInstruction(const std::string& comp,
                 const boost::optional<std::string>& dest,
                 const boost::optional<std::string>& jmp) :
                 comp_(comp),
                 dest_(dest),
                 jmp_(jmp) {}
    std::string ToBinary() const;
  private:
    std::string comp_;
    boost::optional<std::string> dest_;
    boost::optional<std::string> jmp_;
};

#endif
