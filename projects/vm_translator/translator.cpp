#include "./translator.hpp"
#include "./parser.hpp"
#include "./assembly-generator.hpp"

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

namespace {
  std::string RemoveVMSuffix(const std::string& filename) {
    return filename.substr(0, filename.size() - 3);
  }

  bool IsVMFile(const std::string& filename) {
    return filename.size() >= 3 && filename.substr(filename.size() - 3) == ".vm";
  }
}

void GenerateAssemblyFromFile(const boost::filesystem::path& path,
                          AssemblyGenerator* generator) {
  generator->ResetModuleName(RemoveVMSuffix(path.filename().string()));
  std::ifstream ifs;
  ifs.open(path.generic_string(), std::ifstream::in);
  std::cout << "PATH: " << path.generic_string() << std::endl;

  std::string line;
  while (std::getline(ifs, line)) {
    boost::optional<VMInstruction> instruction = ParseLine(line);
    if (instruction) {
      generator->GenerateAssemblyFor(*instruction);
    }
  }
}

void TranslateVMToAssembly(const std::string& path_in,
                           const std::string& file_out) {
  AssemblyGenerator generator;
  generator.GenerateInitAssembly();

  if (boost::filesystem::is_regular_file(path_in)) {
    boost::filesystem::path path(path_in);
    GenerateAssemblyFromFile(path, &generator);
  } else {
    for (const auto & entry : boost::filesystem::directory_iterator(path_in)) {
      if (IsVMFile(entry.path().filename().string())) {
        GenerateAssemblyFromFile(entry.path(), &generator);
      }
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
