#include "./ainstruction.h"
#include "./cinstruction.h"
#include "./instruction-interface.h"
#include "./parser.h"
#include "./symbol-table.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char** argv) {
  std::ifstream ifs;

  if (argc < 2) {
    std::cerr << "You must supply a file name!" << "\n";
    return 1;
  }

  ifs.open(argv[1], std::ifstream::in);

  std::stringstream contents;
  std::string line;
  while (std::getline(ifs, line)) {
    contents << line;
    contents << '\n';
  }

  std::vector<Line> lines = Parse(contents.str());

  auto table = GetDefaultSymbolTable();

  size_t line_number = 0;

  for (const auto& line : lines) {
    if (std::holds_alternative<Label>(line)) {
      table.insert({ std::get<Label>(line), std::to_string(line_number)});
    } else {
      line_number++;
    }
  }

  size_t variable_addr = 16;

  std::stringstream out;
  for (auto& line : lines) {
    if (std::holds_alternative<AInstruction>(line)) {
      auto instruction = std::get<AInstruction>(line);
      if (instruction.HoldsSymbol()) {
        if (table.find(instruction.GetValue()) == table.end()) {
          table.insert({ instruction.GetValue(), std::to_string(variable_addr)});
          variable_addr++;
        }
        instruction.ReplaceSymbols(table);
      }
      out << instruction.ToBinary();
      out << "\n";
    } else if (std::holds_alternative<CInstruction>(line)) {
      out << std::get<CInstruction>(line).ToBinary() << "\n";
    }
  }

  std::ofstream ofs;

  ofs.open("Out.hack", std::ofstream::out);
  ofs << out.str();
  ofs.close();
  return 0;
}
