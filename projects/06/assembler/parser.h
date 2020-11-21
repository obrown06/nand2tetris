#ifndef NAND2TETRIS_PROJECTS_06_ASSEMBLER_PARSER_H
#define NAND2TETRIS_PROJECTS_06_ASSEMBLER_PARSER_H

#include <boost/optional.hpp>

#include "./ainstruction.h"
#include "./cinstruction.h"

#include <string>
#include <variant>
#include <vector>

typedef std::string Label;
typedef std::variant<Label, CInstruction, AInstruction> Line;

std::vector<Line> Parse(const std::string& assembly);

boost::optional<Line> ParseLine(const std::string& str);

Line ParseLabel(const std::string& str);

Line ParseAInstruction(const std::string& str);

Line ParseCInstruction(const std::string& str);

#endif
