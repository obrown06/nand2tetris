#include "./util.hpp"

#include "./assembly_instructions/a-instruction.hpp"
#include "./assembly_instructions/c-instruction.hpp"

void PushBackAll(const AssemblyInstructionSet& source,
                 AssemblyInstructionSet* dest) {
  std::for_each(source.begin(), source.end(),
  [&dest](const std::shared_ptr<BaseAssemblyInstruction>& instruction) {
    dest->push_back(std::move(instruction));
  });
}
