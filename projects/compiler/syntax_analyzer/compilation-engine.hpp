#ifndef SYNTAX_ANALYZER_COMPILATION_ENGINE_HPP_
#define SYNTAX_ANALYZER_COMPILATION_ENGINE_HPP_

#include <set>
#include <string>
#include <vector>

#include "./token.hpp"
#include "./tokenizer.hpp"

class CompilationEngine {
 public:
  CompilationEngine(const std::string& filename) : tokenizer_(filename) {}
  std::vector<std::string> CompileClass();

 private:
   void CompileTerm(std::vector<std::string>*dest);
   void CompileExpressionList(std::vector<std::string>* dest);
   void CompileExpression(std::vector<std::string>* dest);
   void CompileSubroutineCall(std::vector<std::string>* dest,
                              bool beginning_identifier_consumed);
   void CompileIfStatement(std::vector<std::string>* dest);
   void CompileWhileStatement(std::vector<std::string>* dest);
   void CompileLetStatement(std::vector<std::string>* dest);
   void CompileDoStatement(std::vector<std::string>* dest);
   void CompileReturnStatement(std::vector<std::string>* dest);
   void CompileStatements(std::vector<std::string>*dest);
   void CompileVarDecls(std::vector<std::string>* dest);
   void CompileClassVarDecls(std::vector<std::string>* dest);
   void CompileSubroutineDecls(std::vector<std::string>* dest);
   void CompileSubroutineBody(std::vector<std::string>* dest);
   void CompileParamList(std::vector<std::string>* dest);
   void CompileType(std::vector<std::string>* dest);

   std::string ConsumeKeyword(
     const std::set<Token::Keyword>& possible_keywords);
   std::string ConsumeIdentifier();
   std::string ConsumeSymbol(const std::set<char>& possible_symbols);
   std::string ConsumeConstString();
   std::string ConsumeConstInt();
   void CheckTokenizerHasNextToken() const;
   Tokenizer tokenizer_;
};

#endif
