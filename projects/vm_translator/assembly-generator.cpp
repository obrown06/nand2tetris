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
  const std::string kAPlusDComputation = "A+D";
  const std::string kFalseComp = "0";
  const std::string kTrueComp = "1";
  const std::string kUnconditionalJumpComp = "0";
  const std::string kUnconditionalJumpJump = "JMP";

  const std::map<VMInstruction::VMInstructionType, std::string> kOperationTypesToComputations {
    { VMInstruction::VMInstructionType::ADD, "M+D" },
    { VMInstruction::VMInstructionType::SUB, "D-M" },
    { VMInstruction::VMInstructionType::NEG, "-D" },
    { VMInstruction::VMInstructionType::AND, "D&M" },
    { VMInstruction::VMInstructionType::OR, "D|M" },
    { VMInstruction::VMInstructionType::NOT, "!D" },
    { VMInstruction::VMInstructionType::EQ, "D-M" },
    { VMInstruction::VMInstructionType::GT, "D-M" },
    { VMInstruction::VMInstructionType::LT, "D-M" }
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
    VMInstruction::MemorySegmentType::LOCAL,
    VMInstruction::MemorySegmentType::ARGUMENT,
    VMInstruction::MemorySegmentType::THIS,
    VMInstruction::MemorySegmentType::THAT
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
}

void
AssemblyGenerator::TranslateAndStoreAssemblyFor(const VMInstruction& instruction) {
  VMInstruction::VMInstructionType instruction_type = instruction.GetInstructionType();
  auto generated =
    (kArithmeticOperationTypes.find(instruction_type)
      != kArithmeticOperationTypes.end())
    ? GenerateArithmeticInstructionSet(instruction_type) :
      GenerateMemAccessInstructionSet(instruction_type,
                                      *instruction.GetMemorySegmentType(),
                                      *instruction.GetMemorySegmentAddress());
  return PushBackAll(generated, &instructions_);
}

std::string
AssemblyGenerator::MakeStaticSymbol(size_t seed) {
  std::stringstream symbol;
  symbol << program_name_ << "." << std::to_string(seed);
  return symbol.str();
}

AssemblyInstructionSet
AssemblyGenerator::GenerateMemAccessInstructionSet(
  VMInstruction::VMInstructionType instruction_type,
  VMInstruction::MemorySegmentType memory_segment_type,
  size_t memory_segment_address) {
  if (instruction_type == VMInstruction::VMInstructionType::PUSH) {
    return GeneratePushMemAccessInstructionSet(
      memory_segment_type,
      memory_segment_address);
  } else {
    return GeneratePopMemAccessInstructionSet(
      memory_segment_type,
      memory_segment_address);
  }
}

AssemblyInstructionSet
AssemblyGenerator::GetLoadMemorySegmentAddressToARegisterInstructionSet(
  VMInstruction::MemorySegmentType memory_segment_type,
  size_t memory_segment_address) {
  AssemblyInstructionSet assembly;
  if (memory_segment_type == VMInstruction::MemorySegmentType::STATIC) {
    assembly.push_back(
     std::make_shared<AInstruction>(MakeStaticSymbol(memory_segment_address))
    );
  } else {
    assembly.push_back(
      std::make_shared<AInstruction>(
        kMemorySegmentTypesToRAMAddrs.at(memory_segment_type)));

    if (kIndirectMemorySegmentAddrs.find(memory_segment_type)
        != kIndirectMemorySegmentAddrs.end()) {
      assembly.push_back(
       std::make_shared<CInstruction>(
         /*comp=*/kMRegister,
         /*dest=*/kARegister)
       );
    }

    assembly.push_back(
     std::make_shared<CInstruction>(
       /*comp=*/kMRegister,
       /*dest=*/kDRegister)
     );

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
) {
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
AssemblyGenerator::GeneratePopMemAccessInstructionSet(
  VMInstruction::MemorySegmentType memory_segment_type,
  size_t memory_segment_address)
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
  assembly.push_back(std::make_shared<AInstruction>(end_label->GetSymbol()));
  assembly.push_back(
    std::make_shared<CInstruction>(
      /*comp=*/kUnconditionalJumpComp,
      /*dest=*/boost::none,
      /*jump=*/kUnconditionalJumpJump
    ));

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
