#include "./assembly-generator.hpp"

#include "./assembly_instructions/c-instruction.hpp"
#include "./assembly_instructions/a-instruction.hpp"
#include "./util.hpp"

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

namespace {
  const std::string kDRegister = "D";
  const std::string kMRegister = "M";
  const std::string kARegister = "A";
  const std::string kStackPointerRAMLocation = "0";
  const std::string kDecrementMRegisterComp = "M-1";
  const std::string kIncrementMRegisterComp = "M+1";
  const std::string kIncrementARegisterComp = "A+1";
  const std::string kIncrementDRegisterComp = "D+1";
  const std::string kAPlusDComputation = "A+D";
  const std::string kDMinusAComp = "D-A";
  const std::string kMMinusAComp = "M-A";
  const std::string kFalseComp = "0";
  const std::string kTrueComp = "-1";
  const std::string kUnconditionalJumpComp = "0";
  const std::string kUnconditionalJumpJump = "JMP";
  const std::string kNEJump = "JNE";
  const std::string kSystemInitMethod = "Sys.init";
  const size_t kStackPointerInit = 256;

  const std::map<VMInstruction::VMInstructionType, std::string> kOperationTypesToComputations {
    { VMInstruction::VMInstructionType::ADD, "M+D" },
    { VMInstruction::VMInstructionType::SUB, "M-D" },
    { VMInstruction::VMInstructionType::NEG, "-D" },
    { VMInstruction::VMInstructionType::AND, "D&M" },
    { VMInstruction::VMInstructionType::OR, "D|M" },
    { VMInstruction::VMInstructionType::NOT, "!D" },
    { VMInstruction::VMInstructionType::EQ, "M-D" },
    { VMInstruction::VMInstructionType::GT, "M-D" },
    { VMInstruction::VMInstructionType::LT, "M-D" }
  };

  const std::map<VMInstruction::VMInstructionType, std::string> kLogicalOperationTypesToJmps {
    { VMInstruction::VMInstructionType::EQ, "JEQ" },
    { VMInstruction::VMInstructionType::GT, "JGT" },
    { VMInstruction::VMInstructionType::LT, "JLT" }
  };

  const std::set<VMInstruction::VMInstructionType> kUnaryOperationTypes {
    VMInstruction::VMInstructionType::NEG,
    VMInstruction::VMInstructionType::NOT
  };

  const std::set<VMInstruction::VMInstructionType> kArithmeticOperationTypes {
    VMInstruction::VMInstructionType::ADD,
    VMInstruction::VMInstructionType::SUB,
    VMInstruction::VMInstructionType::NEG,
    VMInstruction::VMInstructionType::AND,
    VMInstruction::VMInstructionType::OR,
    VMInstruction::VMInstructionType::NOT,
    VMInstruction::VMInstructionType::EQ,
    VMInstruction::VMInstructionType::GT,
    VMInstruction::VMInstructionType::LT
  };

  const std::map<VMInstruction::MemorySegmentType, std::string> kMemorySegmentTypesToRAMAddrs {
    { VMInstruction::MemorySegmentType::LOCAL, "1" },
    { VMInstruction::MemorySegmentType::ARGUMENT, "2" },
    { VMInstruction::MemorySegmentType::THIS, "3" },
    { VMInstruction::MemorySegmentType::THAT, "4" },
    { VMInstruction::MemorySegmentType::TEMP, "5" },
    { VMInstruction::MemorySegmentType::POINTER, "3" }
  };

  const std::set<VMInstruction::MemorySegmentType> kIndirectMemorySegmentAddrs {
    VMInstruction::MemorySegmentType::POINTER,
    VMInstruction::MemorySegmentType::TEMP
  };

  AssemblyInstructionSet
  GetLoadStackPointerToARegisterInstructionSet() {
    AssemblyInstructionSet instructions;
    instructions.push_back(std::make_shared<AInstruction>(kStackPointerRAMLocation));
    instructions.push_back(std::make_shared<CInstruction>(/*comp=*/kMRegister, /*dest=*/kARegister));
    return std::move(instructions);
  }

  AssemblyInstructionSet
  GetIncrementStackInstructionSet() {
    AssemblyInstructionSet instructions;
    instructions.push_back(std::make_shared<AInstruction>(kStackPointerRAMLocation));
    instructions.push_back(std::make_shared<CInstruction>(/*comp=*/kIncrementMRegisterComp,
                 /*dest=*/kMRegister));
    return instructions;
  }

  AssemblyInstructionSet
  GetDecrementStackInstructionSet() {
    AssemblyInstructionSet instructions;
    instructions.push_back(std::make_shared<AInstruction>(kStackPointerRAMLocation));
    instructions.push_back(std::make_shared<CInstruction>(/*comp=*/kDecrementMRegisterComp,
                 /*dest=*/kMRegister));
    return instructions;
  }

  AssemblyInstructionSet
  GetPushDRegisterToStackInstructionSet() {
    AssemblyInstructionSet assembly;
    PushBackAll(GetLoadStackPointerToARegisterInstructionSet(), &assembly);
    assembly.push_back(
     std::make_shared<CInstruction>(
      /*comp=*/kDRegister,
      /*dest=*/kMRegister)
    );
    PushBackAll(GetIncrementStackInstructionSet(), &assembly);
    return assembly;
  }

  AssemblyInstructionSet
  GetPushMRegisterToStackInstructionSet() {
    AssemblyInstructionSet assembly;
    assembly.push_back(
     std::make_shared<CInstruction>(
      /*comp=*/kMRegister,
      /*dest=*/kDRegister)
    );
    PushBackAll(GetPushDRegisterToStackInstructionSet(), &assembly);
    return assembly;
  }

  AssemblyInstructionSet
  GetPushSegmentPointerToStack(VMInstruction::MemorySegmentType segment_type) {
    AssemblyInstructionSet assembly;
    assembly.push_back(
      std::make_shared<AInstruction>(kMemorySegmentTypesToRAMAddrs.at(segment_type))
    );
    PushBackAll(GetPushMRegisterToStackInstructionSet(), &assembly);
    return assembly;
  }
}

void AssemblyGenerator::GenerateInitAssembly() {
  // Set SP = 256
  instructions_.push_back(std::make_shared<AInstruction>(std::to_string(kStackPointerInit)));
  instructions_.push_back(std::make_shared<CInstruction>(
    /*comp=*/kARegister, /*dest=*/kDRegister));
  instructions_.push_back(std::make_shared<AInstruction>(kStackPointerRAMLocation));
  instructions_.push_back(std::make_shared<CInstruction>(
    /*comp=*/kDRegister, /*dest=*/kMRegister));

  // Call Sys.init
  PushBackAll(GenerateCallInstructionSet(kSystemInitMethod, /*n_args=*/0), &instructions_);
}

void
AssemblyGenerator::GenerateAssemblyFor(const VMInstruction& instruction) {
  AssemblyInstructionSet assembly;
  VMInstruction::VMInstructionType instruction_type = instruction.GetInstructionType();
  switch (instruction_type) {
    case VMInstruction::VMInstructionType::ADD:
    case VMInstruction::VMInstructionType::SUB:
    case VMInstruction::VMInstructionType::NEG:
    case VMInstruction::VMInstructionType::AND:
    case VMInstruction::VMInstructionType::OR:
    case VMInstruction::VMInstructionType::NOT:
    case VMInstruction::VMInstructionType::EQ:
    case VMInstruction::VMInstructionType::GT:
    case VMInstruction::VMInstructionType::LT:
      assembly = GenerateArithmeticInstructionSet(instruction_type);
      break;
    case VMInstruction::VMInstructionType::PUSH:
      assembly = GeneratePushMemAccessInstructionSet(
        *instruction.GetMemorySegmentType(),
        *instruction.GetMemorySegmentAddress());
      break;
    case VMInstruction::VMInstructionType::POP:
      assembly = GeneratePopMemAccessInstructionSet(
        *instruction.GetMemorySegmentType(),
        *instruction.GetMemorySegmentAddress());
      break;
    case VMInstruction::VMInstructionType::LABEL:
      assembly = GenerateLabelInstructionSet(*instruction.GetLabel());
      break;
    case VMInstruction::VMInstructionType::GOTO:
      assembly = GenerateGotoInstructionSet(*instruction.GetLabel());
      break;
    case VMInstruction::VMInstructionType::IFGOTO:
      assembly = GenerateIfGotoInstructionSet(*instruction.GetLabel());
      break;
    case VMInstruction::VMInstructionType::CALL:
      assembly = GenerateCallInstructionSet(
        *instruction.GetFunctionName(),
        *instruction.GetNArgs());
      break;
    case VMInstruction::VMInstructionType::FUNCTION:
      assembly = GenerateFunctionInstructionSet(
        *instruction.GetFunctionName(),
        *instruction.GetNVars());
      break;
    case VMInstruction::VMInstructionType::RETURN:
      assembly = GenerateReturnInstructionSet();
      break;
  }
  return PushBackAll(assembly, &instructions_);
}

std::string
AssemblyGenerator::MakeStaticSymbol(size_t seed) const {
  std::stringstream symbol;
  symbol << module_name_ << "." << std::to_string(seed);
  return symbol.str();
}

AssemblyInstructionSet
AssemblyGenerator::GenerateLabelInstructionSet(const std::string& label) const {
  AssemblyInstructionSet assembly;
  assembly.push_back(
   std::make_shared<LabelInstruction>(label)
  );
  return assembly;
}

AssemblyInstructionSet
AssemblyGenerator::GenerateGotoInstructionSet(const std::string& label) const {
  AssemblyInstructionSet assembly;
  LabelInstruction label_instruction(label);
  assembly.push_back(
   std::make_shared<AInstruction>(label_instruction.GetSymbol())
  );
  assembly.push_back(
   std::make_shared<CInstruction>(
     /*comp=*/kUnconditionalJumpComp,
     /*dest=*/boost::none,
     /*jump=*/kUnconditionalJumpJump)
  );
  return assembly;
}

AssemblyInstructionSet
AssemblyGenerator::GenerateIfGotoInstructionSet(const std::string& label) const {
  AssemblyInstructionSet assembly;
  PushBackAll(GetDecrementStackInstructionSet(), &assembly);
  PushBackAll(GetLoadStackPointerToARegisterInstructionSet(), &assembly);
  assembly.push_back(
   std::make_shared<CInstruction>(
     /*comp=*/kMRegister,
     /*dest=*/kDRegister)
  );
  LabelInstruction label_instruction(label);
  assembly.push_back(
   std::make_shared<AInstruction>(label_instruction.GetSymbol())
  );
  assembly.push_back(
   std::make_shared<CInstruction>(
     /*comp=*/kDRegister,
     /*dest=*/boost::none,
     /*jump=*/kNEJump)
  );
  return assembly;
}

AssemblyInstructionSet
AssemblyGenerator::GetLoadMemorySegmentAddressToARegisterInstructionSet(
  VMInstruction::MemorySegmentType memory_segment_type,
  size_t memory_segment_address) const {
  AssemblyInstructionSet assembly;
  if (memory_segment_type == VMInstruction::MemorySegmentType::STATIC) {
    assembly.push_back(
     std::make_shared<AInstruction>(MakeStaticSymbol(memory_segment_address))
    );
  } else {
    assembly.push_back(
      std::make_shared<AInstruction>(
        kMemorySegmentTypesToRAMAddrs.at(memory_segment_type)));

    if (kIndirectMemorySegmentAddrs.find(memory_segment_type) != kIndirectMemorySegmentAddrs.end()) {
      assembly.push_back(
       std::make_shared<CInstruction>(
         /*comp=*/kARegister,
         /*dest=*/kDRegister)
       );
    } else {
      assembly.push_back(
       std::make_shared<CInstruction>(
         /*comp=*/kMRegister,
         /*dest=*/kDRegister)
       );
    }

    assembly.push_back(
      std::make_shared<AInstruction>(std::to_string(memory_segment_address))
    );

    assembly.push_back(
     std::make_shared<CInstruction>(
      /*comp=*/kAPlusDComputation,
      /*dest=*/kARegister)
    );
  }
  return assembly;
}

AssemblyInstructionSet
AssemblyGenerator::GeneratePushMemAccessInstructionSet(
  VMInstruction::MemorySegmentType memory_segment_type,
  size_t memory_segment_address
) const {
  AssemblyInstructionSet assembly;

  if (memory_segment_type == VMInstruction::MemorySegmentType::CONSTANT) {
    assembly.push_back(
     std::make_shared<AInstruction>(std::to_string(memory_segment_address))
    );
    assembly.push_back(
     std::make_shared<CInstruction>(
      /*comp=*/kARegister,
      /*dest=*/kDRegister)
    );
  } else {
    PushBackAll(
      GetLoadMemorySegmentAddressToARegisterInstructionSet(
        memory_segment_type,
        memory_segment_address), &assembly);
    assembly.push_back(
     std::make_shared<CInstruction>(
      /*comp=*/kMRegister,
      /*dest=*/kDRegister)
    );
  }
  PushBackAll(GetPushDRegisterToStackInstructionSet(), &assembly);
  return assembly;
}

AssemblyInstructionSet
AssemblyGenerator::GeneratePopMemAccessInstructionSet(
  VMInstruction::MemorySegmentType memory_segment_type,
  size_t memory_segment_address) const
{
  AssemblyInstructionSet assembly;
  PushBackAll(
    GetLoadMemorySegmentAddressToARegisterInstructionSet(
      memory_segment_type,
      memory_segment_address), &assembly);
  assembly.push_back(
   std::make_shared<CInstruction>(
    /*comp=*/kARegister,
    /*dest=*/kDRegister)
  );
  PushBackAll(GetLoadStackPointerToARegisterInstructionSet(), &assembly);
  assembly.push_back(
   std::make_shared<CInstruction>(
    /*comp=*/kDRegister,
    /*dest=*/kMRegister)
  );
  PushBackAll(GetDecrementStackInstructionSet(), &assembly);
  PushBackAll(GetLoadStackPointerToARegisterInstructionSet(), &assembly);
  assembly.push_back(
   std::make_shared<CInstruction>(
    /*comp=*/kMRegister,
    /*dest=*/kDRegister)
  );
  assembly.push_back(
   std::make_shared<CInstruction>(
    /*comp=*/kIncrementARegisterComp,
    /*dest=*/kARegister)
  );
  assembly.push_back(
   std::make_shared<CInstruction>(
    /*comp=*/kMRegister,
    /*dest=*/kARegister)
  );
  assembly.push_back(
   std::make_shared<CInstruction>(
    /*comp=*/kDRegister,
    /*dest=*/kMRegister)
  );
  return assembly;
}

std::shared_ptr<LabelInstruction>
AssemblyGenerator::MakeNextLabelInstructionAndIncrementSeed() {
  return std::make_shared<LabelInstruction>(next_label_seed_++);
}

AssemblyInstructionSet
AssemblyGenerator::GenerateReturnInstructionSet() const {
  AssemblyInstructionSet assembly;

  // Store endFrame in TEMP[3]
  assembly.push_back(
    std::make_shared<AInstruction>(kMemorySegmentTypesToRAMAddrs.at(
        VMInstruction::MemorySegmentType::LOCAL)));
  assembly.push_back(
    std::make_shared<CInstruction>(/*comp=*/kMRegister, /*dest=*/kDRegister));
  assembly.push_back(
    std::make_shared<AInstruction>(kMemorySegmentTypesToRAMAddrs.at(
        VMInstruction::MemorySegmentType::TEMP)));
  for (int i = 0; i < 3; i++) {
    assembly.push_back(
      std::make_shared<CInstruction>(
        /*comp=*/kIncrementARegisterComp, /*dest=*/kARegister));
  }
  assembly.push_back(
    std::make_shared<CInstruction>(/*comp=*/kDRegister, /*dest=*/kMRegister));

  // Store retAddr in TEMP[4]
  assembly.push_back(
    std::make_shared<AInstruction>(std::to_string(5)));
  assembly.push_back(std::make_shared<CInstruction>(
    /*comp=*/kDMinusAComp, /*dest=*/kARegister));
  assembly.push_back(std::make_shared<CInstruction>(
    /*comp=*/kMRegister, /*dest=*/kDRegister));
  assembly.push_back(
    std::make_shared<AInstruction>(kMemorySegmentTypesToRAMAddrs.at(
        VMInstruction::MemorySegmentType::TEMP)));
  for (int i = 0; i < 4; i++) {
    assembly.push_back(
      std::make_shared<CInstruction>(
        /*comp=*/kIncrementARegisterComp, /*dest=*/kARegister));
  }
  assembly.push_back(
    std::make_shared<CInstruction>(
      /*comp=*/kDRegister, /*dest=*/kMRegister));

  // *ARG = pop()
  PushBackAll(GeneratePopMemAccessInstructionSet(
    VMInstruction::MemorySegmentType::ARGUMENT,
    /*memory_segment_address=*/0), &assembly);

  // SP = ARG + 1
  assembly.push_back(
    std::make_shared<AInstruction>(kMemorySegmentTypesToRAMAddrs.at(
        VMInstruction::MemorySegmentType::ARGUMENT)));
  assembly.push_back(std::make_shared<CInstruction>(
    /*comp=*/kMRegister, /*dest=*/kDRegister));
  assembly.push_back(std::make_shared<AInstruction>(kStackPointerRAMLocation));
  assembly.push_back(std::make_shared<CInstruction>(
    /*comp=*/kIncrementDRegisterComp, /*dest=*/kMRegister));

  // Restore values of THAT, THIS, ARGUMENT, and LOCAL
  auto transfer_from_temp = [this, &assembly](size_t offset, VMInstruction::MemorySegmentType to) {
    PushBackAll(GetLoadMemorySegmentAddressToARegisterInstructionSet(
        VMInstruction::MemorySegmentType::TEMP, 3), &assembly);
    assembly.push_back(std::make_shared<CInstruction>(/*comp=*/kMRegister,
                                                      /*dest=*/kDRegister));
    assembly.push_back(std::make_shared<AInstruction>(std::to_string(offset)));
    assembly.push_back(std::make_shared<CInstruction>(
      /*comp=*/kDMinusAComp, /*dest=*/kARegister));
    assembly.push_back(std::make_shared<CInstruction>(
      /*comp=*/kMRegister, /*dest=*/kDRegister));
    assembly.push_back(std::make_shared<AInstruction>(
      kMemorySegmentTypesToRAMAddrs.at(to)));
    assembly.push_back(std::make_shared<CInstruction>(
      /*comp=*/kDRegister, /*dest=*/kMRegister));
  };

  transfer_from_temp(1, VMInstruction::MemorySegmentType::THAT);
  transfer_from_temp(2, VMInstruction::MemorySegmentType::THIS);
  transfer_from_temp(3, VMInstruction::MemorySegmentType::ARGUMENT);
  transfer_from_temp(4, VMInstruction::MemorySegmentType::LOCAL);

  // goto return address
  PushBackAll(GetLoadMemorySegmentAddressToARegisterInstructionSet(
      VMInstruction::MemorySegmentType::TEMP, 4), &assembly);
  assembly.push_back(std::make_shared<CInstruction>(
    /*comp=*/kMRegister, /*dest=*/kARegister));
  assembly.push_back(std::make_shared<CInstruction>(
    /*comp=*/kUnconditionalJumpComp, /*dest=*/boost::none,
    /*jump=*/kUnconditionalJumpJump));
  return assembly;
}

AssemblyInstructionSet
AssemblyGenerator::GenerateFunctionInstructionSet(
  const std::string& function_name, size_t n_vars) const {
  AssemblyInstructionSet assembly;
  assembly.push_back(
    std::make_shared<LabelInstruction>(function_name));

  for (size_t i = 0; i < n_vars; i++) {
    assembly.push_back(
      std::make_shared<AInstruction>(std::to_string(0)));
    assembly.push_back(
      std::make_shared<CInstruction>(/*comp=*/kARegister, /*dest=*/kDRegister));
    PushBackAll(GetPushDRegisterToStackInstructionSet(), &assembly);
  }
  return assembly;
}

AssemblyInstructionSet
AssemblyGenerator::GenerateCallInstructionSet(
  const std::string& function_name, size_t n_args) {
  AssemblyInstructionSet assembly;

  // Push return address
  auto return_address_label = MakeNextLabelInstructionAndIncrementSeed();
  assembly.push_back(std::make_shared<AInstruction>(return_address_label->GetSymbol()));
  assembly.push_back(std::make_shared<CInstruction>(
                      /*comp=*/kARegister,
                      /*dest=*/kDRegister));
  PushBackAll(GetPushDRegisterToStackInstructionSet(), &assembly);

  // Push Segment pointers
  PushBackAll(GetPushSegmentPointerToStack(VMInstruction::MemorySegmentType::LOCAL), &assembly);
  PushBackAll(GetPushSegmentPointerToStack(VMInstruction::MemorySegmentType::ARGUMENT), &assembly);
  PushBackAll(GetPushSegmentPointerToStack(VMInstruction::MemorySegmentType::THIS), &assembly);
  PushBackAll(GetPushSegmentPointerToStack(VMInstruction::MemorySegmentType::THAT), &assembly);

  // Set ARG = SP - 5 - nArgs
  PushBackAll(GetLoadStackPointerToARegisterInstructionSet(), &assembly);
  assembly.push_back(std::make_shared<CInstruction>(
                      /*comp=*/kARegister,
                      /*dest=*/kDRegister));

  assembly.push_back(
    std::make_shared<AInstruction>(std::to_string(5)));
  assembly.push_back(std::make_shared<CInstruction>(
                      /*comp=*/kDMinusAComp,
                      /*dest=*/kDRegister));

  assembly.push_back(
    std::make_shared<AInstruction>(std::to_string(n_args)));
  assembly.push_back(std::make_shared<CInstruction>(
                      /*comp=*/kDMinusAComp,
                      /*dest=*/kDRegister));

  assembly.push_back(
    std::make_shared<AInstruction>(
      kMemorySegmentTypesToRAMAddrs.at(
        VMInstruction::MemorySegmentType::ARGUMENT)));
  assembly.push_back(std::make_shared<CInstruction>(
                      /*comp=*/kDRegister,
                      /*dest=*/kMRegister));

  // Set LCL = SP
  PushBackAll(GetLoadStackPointerToARegisterInstructionSet(), &assembly);
  assembly.push_back(std::make_shared<CInstruction>(
                      /*comp=*/kARegister,
                      /*dest=*/kDRegister));
  assembly.push_back(std::make_shared<AInstruction>(
    kMemorySegmentTypesToRAMAddrs.at(
        VMInstruction::MemorySegmentType::LOCAL)));
  assembly.push_back(std::make_shared<CInstruction>(
                      /*comp=*/kDRegister,
                      /*dest=*/kMRegister));

  // goto function_name
  PushBackAll(GenerateGotoInstructionSet(function_name), &assembly);

  // Finally, insert the return_address label into the assembly
  assembly.push_back(return_address_label);
  return assembly;
}

AssemblyInstructionSet
AssemblyGenerator::GenerateArithmeticInstructionSet(
  VMInstruction::VMInstructionType instruction_type) {
    std::cout << "GeneratingGenerateArithmeticInstructionSet" << std::endl;
  AssemblyInstructionSet assembly;

  // Decrement SP and pop to D register.
  PushBackAll(GetDecrementStackInstructionSet(), &assembly);
  PushBackAll(GetLoadStackPointerToARegisterInstructionSet(), &assembly);
  assembly.push_back(std::make_shared<CInstruction>(/*comp=*/kMRegister,
              /*dest=*/kDRegister));

  if (kUnaryOperationTypes.find(instruction_type) == kUnaryOperationTypes.end()) {
    // Decrement SP + pop to M register.
    PushBackAll(GetDecrementStackInstructionSet(), &assembly);
    PushBackAll(GetLoadStackPointerToARegisterInstructionSet(), &assembly);
  }

  // If this isn't a logical operation, all we need to do is to store the
  // result and increment the stack.
  if (kLogicalOperationTypesToJmps.find(instruction_type)
       == kLogicalOperationTypesToJmps.end()) {
    assembly.push_back(
     std::make_shared<CInstruction>(
       /*comp=*/kOperationTypesToComputations.at(instruction_type),
       /*dest=*/kMRegister)
     );
    PushBackAll(GetIncrementStackInstructionSet(), &assembly);
    return assembly;
  }

  // Logical operations are more complicated. The approach is as follows:
  //   1) Subtract M from D and store in D
  //   2) Create new LabelInstructions for TRUE and END
  //   3) Conditionally jump to TRUE depending on a) the value in D register and
  //      b) the operation being performed.
  //   4) Add instructions to push `0` onto the stack
  //   5) Add instructions to unconditionally jump to END
  //   6) Insert the TRUE label
  //   7) Add instructions to push `1` onto the stack
  //   8) Insert the END label
  //   9) Finally, increment the stack pointer

  assembly.push_back(std::make_shared<CInstruction>(
              /*comp=*/kOperationTypesToComputations.at(instruction_type),
              /*dest=*/kDRegister));

  auto true_label = MakeNextLabelInstructionAndIncrementSeed();
  auto end_label = MakeNextLabelInstructionAndIncrementSeed();

  // Jump to TRUE
  assembly.push_back(std::make_shared<AInstruction>(true_label->GetSymbol()));
  assembly.push_back(
    std::make_shared<CInstruction>(
      /*comp=*/kDRegister,
      /*dest=*/boost::none,
      /*jump=*/kLogicalOperationTypesToJmps.at(instruction_type)
    ));

  // FALSE case
  PushBackAll(GetLoadStackPointerToARegisterInstructionSet(), &assembly);
  assembly.push_back(
    std::make_shared<CInstruction>(
      /*comp=*/kFalseComp,
      /*dest=*/kMRegister)
    );
  PushBackAll(GenerateGotoInstructionSet(end_label->GetSymbol()), &assembly);

  // TRUE case
  assembly.push_back(true_label);
  PushBackAll(GetLoadStackPointerToARegisterInstructionSet(), &assembly);
  assembly.push_back(
    std::make_shared<CInstruction>(
      /*comp=*/kTrueComp,
      /*dest=*/kMRegister)
    );

  assembly.push_back(end_label);

  PushBackAll(GetIncrementStackInstructionSet(), &assembly);
  std::cout << "Returning GenerateArithmeticInstructionSet" << std::endl;
  return assembly;
}
