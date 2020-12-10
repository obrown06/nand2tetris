#ifndef VMTRANSLATOR_ASSEMBLY_INSTRUCTIONS_A_INSTRUCTION_H_
#define VMTRANSLATOR_ASSEMBLY_INSTRUCTIONS_A_INSTRUCTION_H_

#include "./base-assembly-instruction.hpp"

#include <string>

class AInstruction : public BaseAssemblyInstruction {
 public:
   AInstruction(const std::string& address) : address_(address) {}
   std::string ToString() const override;
 private:
   std::string address_;
};

#endif
