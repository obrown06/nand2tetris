#include "./parser.h"

#include <sstream>

namespace {
  constexpr char kSpaceChar = ' ';
  constexpr char kNewlineChar = '\n';
  constexpr char kCarriageReturnChar = '\r';
  constexpr char kAInstructionStart = '@';
  constexpr char kCommentStart = '/';
  constexpr char kLabelStart = '(';
  constexpr char kLabelEnd = ')';
  constexpr char kEqualSign = '=';
  constexpr char kJumpSign = ';';
}

std::vector<Line> Parse(const std::string& assembly) {
  std::vector<Line> lines;
  std::stringstream in;
  in << assembly;
  std::string line;
  while (std::getline(in, line)) {
    auto parsed = ParseLine(line);
    if (parsed) {
      lines.push_back(parsed.get());
    }
  }
  return lines;
}

boost::optional<Line> ParseLine(const std::string& str) {
  boost::optional<Line> line;

  auto it = str.begin();
  while (it != str.end() && *it == kSpaceChar) {
    it++;
  }

  if (it == str.end() || *it == kNewlineChar || *it == kCarriageReturnChar) {
    return line;
  }

  std::string trimmed(it, str.end() - 1);
  switch (trimmed.front()) {
    case kCommentStart:
      break;
    case kLabelStart:
      line = ParseLabel(trimmed);
      break;
    case kAInstructionStart:
      line = ParseAInstruction(trimmed);
      break;
    default:
      line = ParseCInstruction(trimmed);
  }
  return line;
}

Line ParseAInstruction(const std::string& str) {
  std::stringstream ss;
  for (auto it = str.begin() + 1; it != str.end() && *it != kSpaceChar; it++) {
    ss << *it;
  }
  return AInstruction(ss.str());
}

Line ParseCInstruction(const std::string& str) {
  std::string comp;
  boost::optional<std::string> dest;
  boost::optional<std::string> jmp;

  auto it = str.begin();

  std::stringstream ss;
  while (*it != kEqualSign && *it != kJumpSign) {
    ss << *it;
    it++;
  }

  if (*it == kEqualSign) {
    dest = ss.str();

    ss.clear();
    ss.str(std::string());
    it++;

    while (it != str.end() && *it != kJumpSign && *it != kSpaceChar && *it != kCarriageReturnChar) {
      ss << *it;
      it++;
    }
  }
  comp = ss.str();

  ss.clear();
  ss.str(std::string());
  if (it != str.end()) {
    it++;
  }

  while (it != str.end() && *it != kSpaceChar && *it != kCarriageReturnChar) {
    ss << *it;
    it++;
  }

  if (!ss.str().empty()) {
    jmp = ss.str();
  }

  return CInstruction(comp, dest, jmp);
}

Line ParseLabel(const std::string& str) {
  std::stringstream ss;
  for (auto it = str.begin() + 1; *it != kLabelEnd; it++) {
    ss << *it;
  }
  return ss.str();
}
