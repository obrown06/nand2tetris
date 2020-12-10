#include "./translator.hpp"
#include "./parser.hpp"
#include "./assembly-generator.hpp"

#include <fstream>
#include <iostream>

namespace {
  std::string RemoveVMSuffix(const std::string& filename) {
    return filename.substr(0, filename.size() - 3);
  }
}

void TranslateVMToAssembly(const std::string& file_in,
                           const std::string& file_out) {
  AssemblyGenerator generator(RemoveVMSuffix(file_in));

  std::ifstream ifs;
  ifs.open(file_in, std::ifstream::in);

  std::string line;
  while (std::getline(ifs, line)) {
    boost::optional<VMInstruction> instruction = ParseLine(line);
    if (instruction) {
      generator.TranslateAndStoreAssemblyFor(*instruction);
    }
  }

  AssemblyInstructionSet program = generator.GetCurrentInstructionSet();

  std::ofstream ofs;
  ofs.open(file_out, std::ofstream::out);
  for (const auto& instruction : program) {
    ofs << instruction->ToString() << "\n";
  }
  ofs.close();
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "You must supply input and output file names!" << "\n";
    return 1;
  }
  TranslateVMToAssembly(argv[1], argv[2]);
  return 0;
}
