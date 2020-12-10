#ifndef VMTRANSLATOR_ASSEMBLY_INSTRUCTIONS_BASE_INSTRUCTION_H_
#define VMTRANSLATOR_ASSEMBLY_INSTRUCTIONS_BASE_INSTRUCTION_H_

#include <string>

// Pure virtual interface implemented by all Assembly instructions.
class BaseAssemblyInstruction {
 public:
   virtual ~BaseAssemblyInstruction() {}
   // Returns a string representation of the command.
   virtual std::string ToString() const = 0;
};
#endif
