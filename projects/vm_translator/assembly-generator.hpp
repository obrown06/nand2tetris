#ifndef VM_TRANSLATOR_ASSEMBLY_GENERATOR_HPP_
#define VM_TRANSLATOR_ASSEMBLY_GENERATOR_HPP_

#include "./assembly_instructions/base-assembly-instruction.hpp"
#include "./assembly_instructions/label-instruction.hpp"
#include "./vm_instructions/vm-instruction.hpp"

#include <memory>
#include <vector>

typedef std::vector<std::shared_ptr<BaseAssemblyInstruction> > AssemblyInstructionSet;

// Class that builds a Hack assembly program from a provided sequence of Hack
// VMInstruction objects.
//
// Usage:
//   AssemblyGenerator generator;
//
//   VMInstruction add(VMInstruction::VMInstructionType::ADD);
//   generator.TranslateAndStoreAssemblyFor(add);
//   AssemblyInstructionSet add_instructions
//     = generator.GetCurrentInstructionSet();   // Contains ADD
//
//   VMInstruction subtract(VMInstruction::VMInstructionType::SUBTRACT);
//   generator.TranslateAndStoreAssemblyFor(subtract);
//   AssemblyInstructionSet add_subtract_instructions
//     = generator.GetCurrentInstructionSet();   // Contains ADD, SUBTRACT
class AssemblyGenerator {
 public:
  AssemblyGenerator(const std::string& program_name)
    : program_name_(program_name) {}

  // Returns a copy of the current set of assembly
  // instructions stored by the class.
  AssemblyInstructionSet GetCurrentInstructionSet() const { return instructions_; }

  // Translates the provided VMInstruction to assembly
  // and stores the generated instructions.
  void TranslateAndStoreAssemblyFor(const VMInstruction& vm_instruction);

 protected:

  std::string
  MakeStaticSymbol(size_t seed);

  AssemblyInstructionSet
  GenerateArithmeticInstructionSet(
    VMInstruction::VMInstructionType instruction_type);

  AssemblyInstructionSet
  GenerateMemAccessInstructionSet(
    VMInstruction::VMInstructionType instruction_type,
    VMInstruction::MemorySegmentType memory_segment_type,
    size_t memory_segment_address);

  AssemblyInstructionSet
  GeneratePopMemAccessInstructionSet(
    VMInstruction::MemorySegmentType memory_segment_type,
    size_t memory_segment_address);

  AssemblyInstructionSet
  GeneratePushMemAccessInstructionSet(
    VMInstruction::MemorySegmentType memory_segment_type,
    size_t memory_segment_address);

  AssemblyInstructionSet
  GetLoadMemorySegmentAddressToARegisterInstructionSet(
    VMInstruction::MemorySegmentType memory_segment_type,
    size_t memory_segment_address);

  std::shared_ptr<LabelInstruction>
  MakeNextLabelInstructionAndIncrementSeed();

 private:
  AssemblyInstructionSet instructions_;
  size_t next_label_seed_;
  std::string program_name_;
};

#endif
