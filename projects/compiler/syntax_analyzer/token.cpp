#include "./token.hpp"
#include "./util.hpp"

#include <map>
#include <sstream>

namespace {
  const std::map<Token::TokenType, std::string> kTokenTypesToXMLTags = {
    { Token::TokenType::KEYWORD, "keyword" },
    { Token::TokenType::IDENTIFIER, "identifier" },
    { Token::TokenType::SYMBOL, "symbol" },
    { Token::TokenType::CONST_INT, "integerConstant" },
    { Token::TokenType::CONST_STRING, "stringConstant" }
  };

  const std::map<Token::Keyword, std::string> kKeywordsMap= {
    { Token::Keyword::CLASS, "class" },
    { Token::Keyword::METHOD, "method" },
    { Token::Keyword::FUNCTION, "function" },
    { Token::Keyword::CONSTRUCTOR, "constructor" },
    { Token::Keyword::INT, "int" },
    { Token::Keyword::BOOLEAN, "boolean" },
    { Token::Keyword::CHAR, "char" },
    { Token::Keyword::VOID, "void" },
    { Token::Keyword::VAR, "var" },
    { Token::Keyword::STATIC, "static" },
    { Token::Keyword::FIELD, "field" },
    { Token::Keyword::LET, "let" },
    { Token::Keyword::DO, "do" },
    { Token::Keyword::IF, "if" },
    { Token::Keyword::ELSE, "else" },
    { Token::Keyword::WHILE, "while" },
    { Token::Keyword::RETURN, "return" },
    { Token::Keyword::TRUE, "true" },
    { Token::Keyword::FALSE, "false" },
    { Token::Keyword::NULL_KEYWORD, "null" },
    { Token::Keyword::THIS, "this" },
  };

  const std::map<char, std::string> kSpecialcasedSymbols = {
    { '<', "&lt;" },
    { '>', "&gt;" },
    { '"', "&quot;" },
    { '&', "&amp;" }
  };
}

std::string Token::ToXML() const {
  std::stringstream xml;
  xml << StartTag(kTokenTypesToXMLTags.at(GetType())) << " ";
  switch (GetType()) {
    case Token::TokenType::KEYWORD:
      xml << kKeywordsMap.at(GetKeyword());
      break;
    case Token::TokenType::IDENTIFIER:
      xml << GetIdentifier();
      break;
    case Token::TokenType::CONST_INT:
      xml << GetIntConstant();
      break;
    case Token::TokenType::CONST_STRING:
      xml << GetStringConstant();
      break;
    case Token::TokenType::SYMBOL:
      if (kSpecialcasedSymbols.find(GetSymbol()) != kSpecialcasedSymbols.end()) {
        xml << kSpecialcasedSymbols.at(GetSymbol());
      } else {
        xml << GetSymbol();
      }
      break;
  }

  xml << " " << EndTag(kTokenTypesToXMLTags.at(GetType()));
  return xml.str();
}


Token::TokenType Token::GetType() const { return type_; }

char Token::GetSymbol() const {
  CheckTokenTypeIs(TokenType::SYMBOL);
  return *symbol_;
}

Token::Keyword Token::GetKeyword() const {
  CheckTokenTypeIs(TokenType::KEYWORD);
  return *keyword_;
}

std::string Token::GetIdentifier() const {
  CheckTokenTypeIs(TokenType::IDENTIFIER);
  return *string_value_;
}

int Token::GetIntConstant() const {
  CheckTokenTypeIs(TokenType::CONST_INT);
  return *const_int_;
}

std::string Token::GetStringConstant() const {
  CheckTokenTypeIs(TokenType::CONST_STRING);
  return *string_value_;
}

void Token::CheckTokenTypeIs(TokenType expected_type) const {
  if (GetType() != expected_type) {
    throw TokenTypeException();
  }
}
