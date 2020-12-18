#ifndef VMTRANSLATOR_VM_INSTRUCTIONS_VM_INSTRUCTION_HPP_
#define VMTRANSLATOR_VM_INSTRUCTIONS_VM_INSTRUCTION_HPP_

#include <boost/optional.hpp>
#include <string>

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
      POP,
      LABEL,
      GOTO,
      IFGOTO,
      CALL,
      FUNCTION,
      RETURN
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
                  const std::string& label)
      : VMInstruction(instruction_type) {
      label_ = label;
    }

    VMInstruction(VMInstructionType instruction_type,
                  const std::string& function_name,
                  size_t n_elems)
      : VMInstruction(instruction_type) {
      function_name_ = function_name;
      n_elems_ = n_elems;
    }

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

    boost::optional<std::string> GetLabel() const {
      return label_;
    }

    boost::optional<std::string> GetFunctionName() const {
      return function_name_;
    }

    boost::optional<size_t> GetNArgs() const {
      return n_elems_;
    }

    boost::optional<size_t> GetNVars() const {
      return n_elems_;
    }

  private:
    VMInstructionType instruction_type_;
    boost::optional<MemorySegmentType> memory_segment_type_;
    boost::optional<size_t> memory_segment_address_;
    boost::optional<std::string> label_;
    boost::optional<std::string> function_name_;
    boost::optional<size_t> n_elems_;
};

#endif
