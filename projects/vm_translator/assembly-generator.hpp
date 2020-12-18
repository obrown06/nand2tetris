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
  AssemblyGenerator() {}

  // Returns a copy of the current set of assembly
  // instructions stored by the class.
  AssemblyInstructionSet GetCurrentInstructionSet() const { return instructions_; }

  // Translates the provided VMInstruction to assembly
  // and stores the generated instructions.
  void GenerateAssemblyFor(const VMInstruction& vm_instruction);

  void ResetModuleName(const std::string& module_name) {
    module_name_ = module_name;
  }

  void GenerateInitAssembly();

 private:
  AssemblyInstructionSet
  GenerateArithmeticInstructionSet(
    VMInstruction::VMInstructionType instruction_type);

  AssemblyInstructionSet
  GenerateCallInstructionSet(
    const std::string& function_name, size_t n_args);

  std::shared_ptr<LabelInstruction>
  MakeNextLabelInstructionAndIncrementSeed();

  AssemblyInstructionSet
  GenerateFunctionInstructionSet(
    const std::string& function_name, size_t n_vars) const;

  AssemblyInstructionSet
  GenerateReturnInstructionSet() const;

  std::string
  MakeStaticSymbol(size_t seed) const;

  AssemblyInstructionSet
  GenerateLabelInstructionSet(const std::string& label) const;

  AssemblyInstructionSet
  GenerateGotoInstructionSet(const std::string& label) const;

  AssemblyInstructionSet
  GenerateIfGotoInstructionSet(const std::string& label) const;

  AssemblyInstructionSet
  GeneratePopMemAccessInstructionSet(
    VMInstruction::MemorySegmentType memory_segment_type,
    size_t memory_segment_address) const;

  AssemblyInstructionSet
  GeneratePushMemAccessInstructionSet(
    VMInstruction::MemorySegmentType memory_segment_type,
    size_t memory_segment_address) const;

  AssemblyInstructionSet
  GetLoadMemorySegmentAddressToARegisterInstructionSet(
    VMInstruction::MemorySegmentType memory_segment_type,
    size_t memory_segment_address) const;

  AssemblyInstructionSet instructions_;
  size_t next_label_seed_;
  std::string module_name_;
};

#endif
