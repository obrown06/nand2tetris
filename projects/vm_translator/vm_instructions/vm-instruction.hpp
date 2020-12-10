#ifndef VMTRANSLATOR_VM_INSTRUCTIONS_VM_INSTRUCTION_HPP_
#define VMTRANSLATOR_VM_INSTRUCTIONS_VM_INSTRUCTION_HPP_

#include <boost/optional.hpp>

// Class representing a single VM Instruction.
class VMInstruction {
  public:
    enum class VMInstructionType {
      TYPE_UNSPECIFIED,
      ADD,
      SUB,
      NEG,
      EQ,
      GT,
      LT,
      AND,
      OR,
      NOT,
      PUSH,
      POP
    };

    enum class MemorySegmentType {
      TYPE_UNSPECIFIED,
      LOCAL,
      ARGUMENT,
      THIS,
      THAT,
      CONSTANT,
      STATIC,
      POINTER,
      TEMP
    };

    VMInstruction(VMInstructionType instruction_type)
      : instruction_type_(instruction_type) {}

    VMInstruction(VMInstructionType instruction_type,
                  MemorySegmentType memory_segment_type,
                  size_t memory_segment_address)
      : VMInstruction(instruction_type) {
          memory_segment_type_ = memory_segment_type;
          memory_segment_address_ = memory_segment_address;
        }

    VMInstructionType GetInstructionType() const { return instruction_type_; }

    boost::optional<MemorySegmentType> GetMemorySegmentType() const {
      return memory_segment_type_;
    }

    boost::optional<size_t> GetMemorySegmentAddress() const {
      return memory_segment_address_;
    }

  private:
    VMInstructionType instruction_type_;
    boost::optional<MemorySegmentType> memory_segment_type_;
    boost::optional<size_t> memory_segment_address_;
};

#endif
