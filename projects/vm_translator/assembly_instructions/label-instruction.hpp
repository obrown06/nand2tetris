#ifndef VMTRANSLATOR_ASSEMBLY_INSTRUCTIONS_LABEL_INSTRUCTION_H_
#define VMTRANSLATOR_ASSEMBLY_INSTRUCTIONS_LABEL_INSTRUCTION_H_

#include "./base-assembly-instruction.hpp"

#include <string>

class LabelInstruction : public BaseAssemblyInstruction {
 public:
   LabelInstruction(size_t seed);
   LabelInstruction(const std::string& label) : label_(label) {}
   std::string ToString() const override;
   std::string GetSymbol() const;
 private:
   std::string label_;
};

#endif
