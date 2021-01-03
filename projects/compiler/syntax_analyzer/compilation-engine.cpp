#include "./compilation-engine.hpp"
#include "./util.hpp"

namespace {
  constexpr char kOpeningBraceChar = '{';
  constexpr char kClosingBraceChar = '}';
  constexpr char kOpeningParenChar = '(';
  constexpr char kClosingParenChar = ')';
  constexpr char kOpeningBracketChar = '[';
  constexpr char kClosingBracketChar = ']';
  constexpr char kEqualsChar = '=';
  constexpr char kCommaChar = ',';
  constexpr char kSemicolonChar = ';';
  constexpr char kDotOperatorChar = '.';
  constexpr char kClassXMLTag[] = "class";
  constexpr char kClassVarDeclXMLTag[] = "classVarDec";
  constexpr char kSubroutineXMLTag[] = "subroutineDec";
  constexpr char kParamListXMLTag[] = "parameterList";
  constexpr char kSubroutineBodyXMLTag[] = "subroutineBody";
  constexpr char kVarDeclXMLTag[] = "varDec";
  constexpr char kStatementsXMLTag[] = "statements";
  constexpr char kIfStatementXMLTag[] = "ifStatement";
  constexpr char kWhileStatementXMLTag[] = "whileStatement";
  constexpr char kDoStatementXMLTag[] = "doStatement";
  constexpr char kReturnStatementXMLTag[] = "returnStatement";
  constexpr char kLetStatementXMLTag[] = "LetStatement";
  constexpr char kExpressionXMLTag[] = "expression";
  constexpr char kExpressionListXMLTag[] = "expressionList";
  constexpr char kTermXMLTag[] = "term";

  const std::set<char> kOperatorChars = {
    '+', '-', '*', '/', '&', '|', '<', '>', '='
  };

  const std::set<Token::Keyword> kPossibleClassVarDeclKeywords = {
    Token::Keyword::STATIC,
    Token::Keyword::FIELD
  };

  const std::set<Token::Keyword> kPossibleSubroutineKeywords = {
    Token::Keyword::CONSTRUCTOR,
    Token::Keyword::FUNCTION,
    Token::Keyword::METHOD,
  };

  const std::set<Token::Keyword> kTypeKeywords = {
    Token::Keyword::INT,
    Token::Keyword::CHAR,
    Token::Keyword::BOOLEAN
  };

  const std::set<Token::Keyword> kPossibleBeginStatementKeywords = {
    Token::Keyword::IF,
    Token::Keyword::LET,
    Token::Keyword::WHILE,
    Token::Keyword::DO,
    Token::Keyword::RETURN
  };

  const std::set<Token::Keyword> kKeywordConstants = {
    Token::Keyword::TRUE,
    Token::Keyword::FALSE,
    Token::Keyword::NULL_KEYWORD,
    Token::Keyword::THIS
  };

  const std::set<char> kUnaryOperatorChars = {
    '-', '~'
  };

  bool IsTokenStartOfClassVarDecl(const Token& token) {
    return token.GetType() == Token::TokenType::KEYWORD &&
           kPossibleClassVarDeclKeywords.find(token.GetKeyword())
             != kPossibleClassVarDeclKeywords.end();
  }

  bool IsTokenStartOfSubroutineDecl(const Token& token) {
    return token.GetType() == Token::TokenType::KEYWORD &&
           kPossibleSubroutineKeywords.find(token.GetKeyword())
             != kPossibleSubroutineKeywords.end();
  }

  bool IsTokenStartOfVarDecl(const Token& token) {
    return token.GetType() == Token::TokenType::KEYWORD &&
           token.GetKeyword() == Token::Keyword::VAR;
  }

  bool IsTokenComma(const Token& token) {
    return token.GetType() == Token::TokenType::SYMBOL &&
           token.GetSymbol() == kCommaChar;
  }

  bool IsTokenAType(const Token& token) {
    return (token.GetType() == Token::TokenType::KEYWORD &&
           kTypeKeywords.find(token.GetKeyword()) != kTypeKeywords.end()) ||
           token.GetType() == Token::TokenType::IDENTIFIER;
  }

  bool IsTokenStartOfStatement(const Token& token) {
    return token.GetType() == Token::TokenType::KEYWORD &&
           kPossibleBeginStatementKeywords.find(token.GetKeyword())
             != kPossibleBeginStatementKeywords.end();
  }

  bool IsTokenElse(const Token& token) {
    return token.GetType() == Token::TokenType::KEYWORD &&
           token.GetKeyword() == Token::Keyword::ELSE;
  }

  bool IsTokenOpeningBracket(const Token& token) {
    return token.GetType() == Token::TokenType::SYMBOL &&
           token.GetSymbol() == kOpeningBracketChar;
  }

  bool IsTokenSemicolon(const Token& token) {
    return token.GetType() == Token::TokenType::SYMBOL &&
           token.GetSymbol() == kSemicolonChar;
  }

  bool IsTokenOperator(const Token& token) {
    return token.GetType() == Token::TokenType::SYMBOL &&
           kOperatorChars.find(token.GetSymbol()) != kOperatorChars.end();
  }

  bool IsIntegerConstant(const Token& token) {
    return token.GetType() == Token::TokenType::CONST_INT;
  }

  bool IsStringConstant(const Token& token) {
    return token.GetType() == Token::TokenType::CONST_STRING;
  }

  bool IsKeywordConstant(const Token& token) {
    return token.GetType() == Token::TokenType::KEYWORD &&
           kKeywordConstants.find(token.GetKeyword()) != kKeywordConstants.end();
  }

  bool IsUnaryOp(const Token& token) {
    return token.GetType() == Token::TokenType::SYMBOL &&
           kUnaryOperatorChars.find(token.GetSymbol()) != kUnaryOperatorChars.end();
  }

  bool IsOpeningParen(const Token& token) {
    return token.GetType() == Token::TokenType::SYMBOL &&
           token.GetSymbol() == kOpeningParenChar;
  }

  bool IsClosingParen(const Token& token) {
    return token.GetType() == Token::TokenType::SYMBOL &&
           token.GetSymbol() == kClosingParenChar;
  }

  bool IsDotOperator(const Token& token) {
    return token.GetType() == Token::TokenType::SYMBOL &&
           token.GetSymbol() == kDotOperatorChar;
  }
}

std::vector<std::string> CompilationEngine::CompileClass() {
  std::vector<std::string> xml;
  xml.push_back(StartTag(kClassXMLTag));
  xml.push_back(ConsumeKeyword({ Token::Keyword::CLASS }));
  xml.push_back(ConsumeIdentifier());
  xml.push_back(ConsumeSymbol({ kOpeningBraceChar }));
  CompileClassVarDecls(&xml);
  CompileSubroutineDecls(&xml);
  xml.push_back(ConsumeSymbol({ kClosingBraceChar }));
  xml.push_back(EndTag(kClassXMLTag));
  return xml;
}

void CompilationEngine::CompileClassVarDecls(std::vector<std::string>* dest) {
  while (IsTokenStartOfClassVarDecl(tokenizer_.GetNextToken())) {
    dest->push_back(StartTag(kClassVarDeclXMLTag));
    dest->push_back(ConsumeKeyword(kPossibleClassVarDeclKeywords));
    CompileType(dest);
    dest->push_back(ConsumeIdentifier());
    while (IsTokenComma(tokenizer_.GetNextToken())) {
      dest->push_back(ConsumeSymbol({ kCommaChar }));
      dest->push_back(ConsumeIdentifier());
    }
    dest->push_back(ConsumeSymbol({ kSemicolonChar }));
    dest->push_back(EndTag(kClassVarDeclXMLTag));
  }
}

void CompilationEngine::CompileSubroutineDecls(std::vector<std::string>* dest) {
  while (IsTokenStartOfSubroutineDecl(tokenizer_.GetNextToken())) {
    dest->push_back(StartTag(kSubroutineXMLTag));
    dest->push_back(ConsumeKeyword(kPossibleSubroutineKeywords));

    // (void | type)
    if (IsTokenAType(tokenizer_.GetNextToken())) {
      CompileType(dest);
    } else {
      dest->push_back(ConsumeKeyword({ Token::Keyword::VOID }));
    }

    // subroutineName
    dest->push_back(ConsumeIdentifier());

    dest->push_back(ConsumeSymbol({ kOpeningParenChar }));
    CompileParamList(dest);
    dest->push_back(ConsumeSymbol({ kClosingParenChar }));
    CompileSubroutineBody(dest);
    dest->push_back(EndTag(kSubroutineXMLTag));
  }
}

void CompilationEngine::CompileParamList(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kParamListXMLTag));
  while (IsTokenAType(tokenizer_.GetNextToken())) {
    CompileType(dest);
    dest->push_back(ConsumeIdentifier());
    if (IsTokenComma(tokenizer_.GetNextToken())) {
      dest->push_back(ConsumeSymbol({ kCommaChar }));
    }
  }
  dest->push_back(EndTag(kParamListXMLTag));
}

void CompilationEngine::CompileSubroutineBody(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kSubroutineBodyXMLTag));
  dest->push_back(ConsumeSymbol({ kOpeningBraceChar }));
  CompileVarDecls(dest);
  CompileStatements(dest);
  dest->push_back(ConsumeSymbol({ kClosingBraceChar }));
  dest->push_back(EndTag(kSubroutineBodyXMLTag));
}

void CompilationEngine::CompileVarDecls(std::vector<std::string>* dest) {
  while (IsTokenStartOfVarDecl(tokenizer_.GetNextToken())) {
    dest->push_back(StartTag(kVarDeclXMLTag));
    dest->push_back(ConsumeKeyword({ Token::Keyword::VAR }));
    CompileType(dest);
    dest->push_back(ConsumeIdentifier());
    while (IsTokenComma(tokenizer_.GetNextToken())) {
      dest->push_back(ConsumeSymbol({ kCommaChar }));
      dest->push_back(ConsumeIdentifier());
    }
    dest->push_back(ConsumeSymbol({ kSemicolonChar }));
    dest->push_back(EndTag(kVarDeclXMLTag));
  }
}

void CompilationEngine::CompileStatements(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kStatementsXMLTag));
  while (IsTokenStartOfStatement(tokenizer_.GetNextToken())) {
    switch (tokenizer_.GetNextToken().GetKeyword()) {
      case Token::Keyword::IF:
        CompileIfStatement(dest);
        break;
      case Token::Keyword::WHILE:
        CompileWhileStatement(dest);
        break;
      case Token::Keyword::LET:
        CompileLetStatement(dest);
        break;
      case Token::Keyword::DO:
        CompileDoStatement(dest);
        break;
      case Token::Keyword::RETURN:
        CompileReturnStatement(dest);
    }
  }
  dest->push_back(EndTag(kStatementsXMLTag));
}

void CompilationEngine::CompileIfStatement(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kIfStatementXMLTag));
  dest->push_back(ConsumeKeyword({ Token::Keyword::IF }));
  dest->push_back(ConsumeSymbol({ kOpeningParenChar }));
  CompileExpression(dest);
  dest->push_back(ConsumeSymbol({ kClosingParenChar }));
  dest->push_back(ConsumeSymbol({ kOpeningBraceChar }));
  CompileStatements(dest);
  dest->push_back(ConsumeSymbol({ kClosingBraceChar }));
  if (IsTokenElse(tokenizer_.GetNextToken())) {
    dest->push_back(ConsumeKeyword({ Token::Keyword::ELSE }));
    dest->push_back(ConsumeSymbol({ kOpeningBraceChar }));
    CompileStatements(dest);
    dest->push_back(ConsumeSymbol({ kClosingBraceChar }));
  }
  dest->push_back(EndTag(kIfStatementXMLTag));
}

void CompilationEngine::CompileWhileStatement(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kWhileStatementXMLTag));
  dest->push_back(ConsumeKeyword({ Token::Keyword::WHILE }));
  dest->push_back(ConsumeSymbol({ kOpeningParenChar }));
  CompileExpression(dest);
  dest->push_back(ConsumeSymbol({ kClosingParenChar }));
  dest->push_back(ConsumeSymbol({ kOpeningBraceChar }));
  CompileStatements(dest);
  dest->push_back(ConsumeSymbol({ kClosingBraceChar }));
  dest->push_back(EndTag(kWhileStatementXMLTag));
}

void CompilationEngine::CompileLetStatement(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kLetStatementXMLTag));
  dest->push_back(ConsumeKeyword({ Token::Keyword::LET }));
  dest->push_back(ConsumeIdentifier());
  if (IsTokenOpeningBracket(tokenizer_.GetNextToken())) {
    dest->push_back(ConsumeSymbol({ kOpeningBracketChar }));
    CompileExpression(dest);
    dest->push_back(ConsumeSymbol({ kClosingBracketChar }));
  }
  dest->push_back(ConsumeSymbol({ kEqualsChar }));
  CompileExpression(dest);
  dest->push_back(ConsumeSymbol({ kSemicolonChar }));
  dest->push_back(EndTag(kLetStatementXMLTag));
}

void CompilationEngine::CompileDoStatement(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kDoStatementXMLTag));
  dest->push_back(ConsumeKeyword({ Token::Keyword::DO }));
  CompileSubroutineCall(dest, /*beginning_identifier_consumed=*/false);
  dest->push_back(ConsumeSymbol({ kSemicolonChar }));
  dest->push_back(EndTag(kDoStatementXMLTag));
}

void CompilationEngine::CompileReturnStatement(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kReturnStatementXMLTag));
  dest->push_back(ConsumeKeyword({ Token::Keyword::RETURN }));
  if (!IsTokenSemicolon(tokenizer_.GetNextToken())) {
    CompileExpression(dest);
  }
  dest->push_back(ConsumeSymbol({ kSemicolonChar }));
  dest->push_back(EndTag(kReturnStatementXMLTag));
}

void CompilationEngine::CompileType(std::vector<std::string>* dest) {
  if (tokenizer_.GetNextToken().GetType() == Token::TokenType::KEYWORD) {
    dest->push_back(ConsumeKeyword(kTypeKeywords));
  } else {
    dest->push_back(ConsumeIdentifier());
  }
}

void CompilationEngine::CompileExpression(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kExpressionXMLTag));
  CompileTerm(dest);
  while (IsTokenOperator(tokenizer_.GetNextToken())) {
    dest->push_back(ConsumeSymbol({ kOperatorChars }));
    CompileTerm(dest);
  }
  dest->push_back(EndTag(kExpressionXMLTag));
}

void CompilationEngine::CompileTerm(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kTermXMLTag));
  if (IsIntegerConstant(tokenizer_.GetNextToken())) {
    dest->push_back(ConsumeConstInt());
  } else if (IsStringConstant(tokenizer_.GetNextToken())) {
    dest->push_back(ConsumeConstString());
  } else if (IsKeywordConstant(tokenizer_.GetNextToken())) {
    dest->push_back(ConsumeKeyword(kKeywordConstants));
  } else if (IsUnaryOp(tokenizer_.GetNextToken())) {
    dest->push_back(ConsumeSymbol(kUnaryOperatorChars));
    CompileTerm(dest);
  } else if (IsOpeningParen(tokenizer_.GetNextToken())) {
    dest->push_back(ConsumeSymbol({ kOpeningParenChar }));
    CompileExpression(dest);
    dest->push_back(ConsumeSymbol({ kClosingParenChar }));
  } else {
    dest->push_back(ConsumeIdentifier());

    if (IsOpeningParen(tokenizer_.GetNextToken()) || IsDotOperator(tokenizer_.GetNextToken())) {
      CompileSubroutineCall(dest, /*beginning_identifier_consumed=*/true);
    } else if (IsTokenOpeningBracket(tokenizer_.GetNextToken())) {
      dest->push_back(ConsumeSymbol({ kOpeningBracketChar }));
      CompileExpression(dest);
      dest->push_back(ConsumeSymbol({ kClosingBracketChar }));
    }
  }
  dest->push_back(EndTag(kTermXMLTag));
}

void CompilationEngine::CompileSubroutineCall(std::vector<std::string>* dest,
                                              bool beginning_identifier_consumed) {
  if (!beginning_identifier_consumed) {
    dest->push_back(ConsumeIdentifier());
  }

  if (IsDotOperator(tokenizer_.GetNextToken())) {
    dest->push_back(ConsumeSymbol({ kDotOperatorChar }));
    dest->push_back(ConsumeIdentifier());
  }
  dest->push_back(ConsumeSymbol({ kOpeningParenChar }));
  CompileExpressionList(dest);
  dest->push_back(ConsumeSymbol({ kClosingParenChar }));
}

void CompilationEngine::CompileExpressionList(std::vector<std::string>* dest) {
  dest->push_back(StartTag(kExpressionListXMLTag));
  if (!IsClosingParen(tokenizer_.GetNextToken())) {
    CompileExpression(dest);
    while (IsTokenComma(tokenizer_.GetNextToken())) {
      dest->push_back(ConsumeSymbol({ kCommaChar }));
      CompileExpression(dest);
    }
  }
  dest->push_back(EndTag(kExpressionListXMLTag));
}

std::string
CompilationEngine::ConsumeKeyword(
  const std::set<Token::Keyword>& possible_keywords) {
  CheckTokenizerHasNextToken();
  Token token = tokenizer_.GetNextToken();
  if (token.GetType() != Token::TokenType::KEYWORD) {
    throw std::runtime_error("Expected keyword token!");
  }
  if (possible_keywords.find(token.GetKeyword()) == possible_keywords.end()) {
    throw std::runtime_error("Wrong keyword token found!");
  }

  tokenizer_.Advance();
  return token.ToXML();
}

std::string CompilationEngine::ConsumeIdentifier() {
  CheckTokenizerHasNextToken();
  Token token = tokenizer_.GetNextToken();
  if (token.GetType() != Token::TokenType::IDENTIFIER) {
    throw std::runtime_error("Expected identifier token!");
  }
  tokenizer_.Advance();
  return token.ToXML();
}

std::string CompilationEngine::ConsumeSymbol(const std::set<char>& possible_symbols) {
  CheckTokenizerHasNextToken();
  Token token = tokenizer_.GetNextToken();
  if (token.GetType() != Token::TokenType::SYMBOL) {
    throw std::runtime_error("Expected symbol token!");
  }
  if (possible_symbols.find(token.GetSymbol()) == possible_symbols.end()) {
    throw std::runtime_error("Wrong symbol token found!");
  }
  tokenizer_.Advance();
  return token.ToXML();
}

std::string CompilationEngine::ConsumeConstInt() {
  CheckTokenizerHasNextToken();
  Token token = tokenizer_.GetNextToken();
  if (token.GetType() != Token::TokenType::CONST_INT) {
    throw std::runtime_error("Expected const int token!");
  }
  tokenizer_.Advance();
  return token.ToXML();
}

std::string CompilationEngine::ConsumeConstString() {
  CheckTokenizerHasNextToken();
  Token token = tokenizer_.GetNextToken();
  if (token.GetType() != Token::TokenType::CONST_STRING) {
    throw std::runtime_error("Expected const string token!");
  }
  tokenizer_.Advance();
  return token.ToXML();
}

void CompilationEngine::CheckTokenizerHasNextToken() const {
  if (!tokenizer_.HasNextToken()) {
    throw std::runtime_error("No more tokens!");
  }
}
