#ifndef VMTRANSLATOR_UTIL_HPP_
#define VMTRANSLATOR_UTIL_HPP_

#include "./assembly_instructions/base-assembly-instruction.hpp"
#include "./assembly-generator.hpp"

#include <memory>
#include <vector>

void PushBackAll(const AssemblyInstructionSet& source,
                 AssemblyInstructionSet* dest);

#endif
