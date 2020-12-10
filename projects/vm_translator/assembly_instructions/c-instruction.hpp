#ifndef VMTRANSLATOR_ASSEMBLY_INSTRUCTIONS_C_INSTRUCTION_H_
#define VMTRANSLATOR_ASSEMBLY_INSTRUCTIONS_C_INSTRUCTION_H_

#include "./base-assembly-instruction.hpp"

#include <boost/optional.hpp>
#include <string>

class CInstruction : public BaseAssemblyInstruction {
  public:
    CInstruction(const std::string& comp,
                 const boost::optional<std::string>& dest = boost::none,
                 const boost::optional<std::string>& jump = boost::none)
                 : comp_(comp), dest_(dest), jump_(jump) {}
    std::string ToString() const override;

  private:
    std::string comp_;
    boost::optional<std::string> dest_;
    boost::optional<std::string> jump_;
};

#endif
