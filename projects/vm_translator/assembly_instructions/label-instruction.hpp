#ifndef VMTRANSLATOR_ASSEMBLY_INSTRUCTIONS_LABEL_INSTRUCTION_H_
#define VMTRANSLATOR_ASSEMBLY_INSTRUCTIONS_LABEL_INSTRUCTION_H_

#include "./base-assembly-instruction.hpp"

#include <string>

class LabelInstruction : public BaseAssemblyInstruction {
 public:
   LabelInstruction(size_t seed) : seed_(seed) {}
   std::string ToString() const override;
   std::string GetSymbol() const;
 private:
   size_t seed_;
};

#endif
