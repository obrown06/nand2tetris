#include "./tokenizer.hpp"

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <sstream>

namespace {
  constexpr char kTabChar = '\t';
  constexpr char kCarriageReturnChar = '\r';
  constexpr char kNewLineChar = '\n';
  constexpr char kSpaceChar = ' ';
  constexpr char kStringDelimiterChar = '"';
  constexpr char kBlockCommentSequence[] = "/**";
  constexpr char kEndBlockCommentSequence[] = "*/";
  constexpr char kBackslashChar = '/';
  constexpr char kStarChar = '*';

  const std::set<char> kSymbolChars = {
    '{', '}', '(', ')', '[', ']', '.', ',', ';', '+',
    '-', '*', '/', '&', '|', '<', '>', '=', '~'
  };

  const std::map<std::string, Token::Keyword> kKeywordsMap= {
    { "class", Token::Keyword::CLASS },
    { "method", Token::Keyword::METHOD },
    { "function", Token::Keyword::FUNCTION },
    { "constructor", Token::Keyword::CONSTRUCTOR },
    { "int", Token::Keyword::INT },
    { "boolean", Token::Keyword::BOOLEAN },
    { "char", Token::Keyword::CHAR },
    { "void", Token::Keyword::VOID },
    { "var", Token::Keyword::VAR },
    { "static", Token::Keyword::STATIC },
    { "field", Token::Keyword::FIELD },
    { "let", Token::Keyword::LET },
    { "do", Token::Keyword::DO },
    { "if", Token::Keyword::IF },
    { "else", Token::Keyword::ELSE },
    { "while", Token::Keyword::WHILE },
    { "return", Token::Keyword::RETURN },
    { "true", Token::Keyword::TRUE },
    { "false", Token::Keyword::FALSE },
    { "null", Token::Keyword::NULL_KEYWORD },
    { "this", Token::Keyword::THIS },
  };
}

Tokenizer::Tokenizer(const std::string& filename) :
  stream_(filename) {
  Advance();
}

Token Tokenizer::GetNextToken() {
  if (!HasNextToken()) {
    throw std::runtime_error("No more tokens!");
  }
  return *next_token_;
}

void Tokenizer::UnsetNextToken() {
  next_token_ = boost::none;
}

bool Tokenizer::HasNextToken() const {
  return next_token_ != boost::none;
}

void Tokenizer::Advance() {
  if (HasNextToken()) {
  }
  UnsetNextToken();
  std::stringstream sequence;

  while (!HasNextToken() && stream_.HasMoreChars()) {
    char c = stream_.GetNextChar();

    // Disregard space, newline, and carriage return chars.
    if (c == kSpaceChar || c == kNewLineChar ||
        c == kCarriageReturnChar || c == kTabChar) {
      continue;
    }

    // Disregard line comments.
    if (c == kBackslashChar && stream_.PeekNextChar() == kBackslashChar) {
      stream_.ConsumeStreamThrough(std::string(1, kNewLineChar));
      continue;
    }

    // Disregard block comments.
    if (c == kBackslashChar && stream_.PeekNextChar() == kStarChar) {
      stream_.ConsumeStreamThrough(kEndBlockCommentSequence);
      continue;
    }

    // If this is the start of a string, parse a new CONST_STRING token by
    // consuming the stream until the terminating delimiter.
    if (c == kStringDelimiterChar) {
      std::string string_const = stream_.ConsumeStreamThrough(
        std::string(1, kStringDelimiterChar));
      next_token_ = Token(Token::TokenType::CONST_STRING, string_const);
      return;
    }

    // If this is a symbol char, parse a new SYMBOL token and return.
    if (kSymbolChars.find(c) != kSymbolChars.end()) {
      next_token_ = Token(Token::TokenType::SYMBOL, c);
      return;
    }

    // If we've reached here, we know this char is part of a KEYWORD, CONST_INT
    // or IDENTIFIER.
    sequence << c;
    std::string curr_seq = sequence.str();

    // KEYWORD, CONST_INT, and IDENTIFIER tokens are always terminated by a
    // space or a symbol. If the next (unconsumed) char in the stream is either
    // of those, we can immediately parse the token. If not, we need to continue
    // to build the token.
    if (stream_.PeekNextChar() == kSpaceChar ||
        kSymbolChars.find(stream_.PeekNextChar()) != kSymbolChars.end()) {
      std::string token_str = sequence.str();
      if (kKeywordsMap.find(token_str) != kKeywordsMap.end()) {
        next_token_ =
          Token(Token::TokenType::KEYWORD, kKeywordsMap.at(token_str));
      } else if (isdigit(token_str[0])) {
        next_token_ = Token(Token::TokenType::CONST_INT, atoi(token_str.c_str()));
      } else {
        next_token_ = Token(Token::TokenType::IDENTIFIER, token_str);
      }
    }
  }
}

// Tokenizer::CharacterStream

Tokenizer::CharacterStream::CharacterStream(const std::string& filename)
  : stream_(filename) {
  GetNextLine();
}

Tokenizer::CharacterStream::~CharacterStream() {
  stream_.close();
}

std::string Tokenizer::CharacterStream::ConsumeStreamThrough(
  const std::string& sequence) {
  std::stringstream found;
  while (HasMoreChars()) {
    found << GetNextChar();
    std::string curr_seq = found.str();
    if (curr_seq.size() >= sequence.size() &&
        curr_seq.substr(curr_seq.size() - sequence.size()) == sequence) {
      return curr_seq.substr(0, curr_seq.size() - sequence.size());
    }
  }
  return found.str();
}

bool Tokenizer::CharacterStream::HasMoreChars() const {
  return next_line_ != boost::none;
}

char Tokenizer::CharacterStream::PeekNextChar() const {
  if (idx_ == next_line_->size()) {
    return kNewLineChar;
  }
  return (*next_line_)[idx_];
}

char Tokenizer::CharacterStream::GetNextChar() {
  if (idx_ == next_line_->size()) {
    UnsetNextLine();
    GetNextLine();
    return kNewLineChar;
  }

  return (*next_line_)[idx_++];
}

void Tokenizer::CharacterStream::UnsetNextLine() {
  next_line_ = boost::none;
}

void Tokenizer::CharacterStream::GetNextLine() {
  std::string line;
  if (std::getline(stream_, line)) {
    next_line_ = line;
    idx_ = 0;
  }
}
