#include "./token.hpp"
#include "./tokenizer.hpp"
#include "./compilation-engine.hpp"

#include <fstream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
  CompilationEngine analyzer(argv[1]);
  std::vector<std::string> xml = analyzer.CompileClass();
  std::ofstream out(argv[2]);

  for (auto& line : xml) {
    out << line << std::endl;
  }
  out.close();
  return 0;
}
