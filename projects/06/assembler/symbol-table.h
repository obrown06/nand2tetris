#ifndef NAND2TETRIS_PROJECTS_06_ASSEMBLER_SYMBOL_TABLE_H
#define NAND2TETRIS_PROJECTS_06_ASSEMBLER_SYMBOL_TABLE_H

#include <map>
#include <string>

typedef std::map<std::string, std::string> SymbolTable;

SymbolTable GetDefaultSymbolTable();

#endif
