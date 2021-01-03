#ifndef SYNTAX_ANALYZER_TOKEN_HPP
#define SYNTAX_ANALYZER_TOKEN_HPP

#include <boost/optional.hpp>
#include <string>

class TokenTypeException : public std::exception {
  virtual const char* what() const throw() {
    return "Token is of the wrong type!";
  }
};

class Token {
public:
  enum class TokenType {
    TOKEN_TYPE_UNSPECIFIED,
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    CONST_INT,
    CONST_STRING
  };

  enum class Keyword {
    CLASS,
    METHOD,
    FUNCTION,
    CONSTRUCTOR,
    INT,
    BOOLEAN,
    CHAR,
    VOID,
    VAR,
    STATIC,
    FIELD,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN,
    TRUE,
    FALSE,
    NULL_KEYWORD,
    THIS
  };

  Token(TokenType type) : type_(type) {}

  Token(TokenType type, Keyword keyword) : Token(type) {
    keyword_ = keyword;
  }

  Token(TokenType type, char symbol) : Token(type) {
    symbol_ = symbol;
  }

  Token(TokenType type, const std::string& string_value) : Token(type) {
    string_value_ = string_value;
  }

  Token(TokenType type, int int_value) : Token(type) {
    const_int_ = int_value;
  }

  TokenType GetType() const;
  char GetSymbol() const;
  Keyword GetKeyword() const;
  std::string GetIdentifier() const;
  int GetIntConstant() const;
  std::string GetStringConstant() const;
  std::string ToXML() const;

private:
  void CheckTokenTypeIs(TokenType expected_type) const;
  TokenType type_;
  boost::optional<Keyword> keyword_;
  boost::optional<char> symbol_;
  boost::optional<std::string> string_value_;
  boost::optional<int> const_int_;
};

#endif
