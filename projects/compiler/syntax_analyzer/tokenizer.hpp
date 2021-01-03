#ifndef SYNTAX_ANALYZER_TOKENIZER_HPP_
#define SYNTAX_ANALYZER_TOKENIZER_HPP_

#include <boost/optional.hpp>
#include <fstream>
#include <string>
#include <vector>

#include "./token.hpp"

class Tokenizer {
  public:
    Tokenizer(const std::string& filename);
    Token GetNextToken();
    bool HasNextToken() const;
    void Advance();
  private:
    class CharacterStream {
     public:
      CharacterStream(const std::string& filename);
      ~CharacterStream();
      char GetNextChar();
      char PeekNextChar() const;
      bool HasMoreChars() const;
      std::string ConsumeStreamThrough(const std::string& sequence);
     private:
      void GetNextLine();
      void UnsetNextLine();
      std::ifstream stream_;
      boost::optional<std::string> next_line_;
      size_t idx_;
    };
    void UnsetNextToken();
    boost::optional<Token> next_token_;
    CharacterStream stream_;
};

#endif
