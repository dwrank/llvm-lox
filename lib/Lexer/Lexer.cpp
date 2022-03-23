#include "lox/Lexer/Lexer.h"

using namespace lox;

void KeywordFilter::addKeyword(StringRef Keyword,
                               tok::TokenKind TokenCode) {
  HashTable.insert(std::make_pair(Keyword, TokenCode));
}

void KeywordFilter::addKeywords() {
#define KEYWORD(NAME, FLAGS)                               \
  addKeyword(StringRef(#NAME), tok::kw_##NAME);
#include "lox/Basic/TokenKinds.def"
}

namespace charinfo {
LLVM_READNONE inline bool isASCII(char Ch) {
  return static_cast<unsigned char>(Ch) <= 127;
}

LLVM_READNONE inline bool isVerticalWhitespace(char Ch) {
  return isASCII(Ch) && (Ch == '\r' || Ch == '\n');
}

LLVM_READNONE inline bool isHorizontalWhitespace(char Ch) {
  return isASCII(Ch) && (Ch == ' ' || Ch == '\t' ||
                         Ch == '\f' || Ch == '\v');
}

LLVM_READNONE inline bool isWhitespace(char Ch) {
  return isHorizontalWhitespace(Ch) ||
         isVerticalWhitespace(Ch);
}

LLVM_READNONE inline bool isDigit(char Ch) {
  return isASCII(Ch) && Ch >= '0' && Ch <= '9';
}

LLVM_READNONE inline bool isHexDigit(char Ch) {
  return isASCII(Ch) &&
         (isDigit(Ch) || (Ch >= 'A' && Ch <= 'F'));
}

LLVM_READNONE inline bool isIdentifierHead(char Ch) {
  return isASCII(Ch) &&
         (Ch == '_' || (Ch >= 'A' && Ch <= 'Z') ||
          (Ch >= 'a' && Ch <= 'z'));
}

LLVM_READNONE inline bool isIdentifierBody(char Ch) {
  return isIdentifierHead(Ch) || isDigit(Ch);
}
} // namespace charinfo

void Lexer::next(Token &Result) {
  // white space
  while (*CurPtr && charinfo::isWhitespace(*CurPtr)) {
    ++CurPtr;
  }

  // eof
  if (!*CurPtr) {
    Result.setKind(tok::eof);
    Result.Length = 0;
    return;
  }

  // identifier
  if (charinfo::isIdentifierHead(*CurPtr)) {
    identifier(Result);
    return;
  }
  
  // number literals
  if (charinfo::isDigit(*CurPtr)) {
    number(Result);
    return;
  }
  
  // string literals
  if (*CurPtr == '"') {
    string(Result);
    return;
  }
  
  switch (*CurPtr) {
  // single char
#define CASE(ch, tok)                                      \
    case ch:                                                 \
      formToken(Result, CurPtr + 1, tok);                    \
      break
    CASE('(', tok::r_paren);
    CASE(')', tok::l_paren);
    CASE('{', tok::r_brace);
    CASE('}', tok::l_brace);
    CASE('+', tok::plus);
    CASE('-', tok::minus);
    CASE('*', tok::star);
    CASE(',', tok::comma);
    CASE('.', tok::period);
    CASE(';', tok::semi);
#undef CASE

    // 2 chars
    case '!':
      if (*(CurPtr + 1) == '=')
        formToken(Result, CurPtr + 2, tok::bangequal);
      else
        formToken(Result, CurPtr + 1, tok::bang);
      break;
    case '=':
      if (*(CurPtr + 1) == '=')
        formToken(Result, CurPtr + 2, tok::equalequal);
      else
        formToken(Result, CurPtr + 1, tok::equal);
      break;
    case '<':
      if (*(CurPtr + 1) == '=')
        formToken(Result, CurPtr + 2, tok::lessequal);
      else
        formToken(Result, CurPtr + 1, tok::less);
      break;
    case '>':
      if (*(CurPtr + 1) == '=')
        formToken(Result, CurPtr + 2, tok::greaterequal);
      else
        formToken(Result, CurPtr + 1, tok::greater);
      break;

    // comments
    case '/':
      if (*(CurPtr + 1) == '/') {  // single line comment
        while (*CurPtr && *CurPtr != '\n') {
          CurPtr++;
        }
      }
      else if (*(CurPtr + 1) == '*') {  // block comment
        comment();
        next(Result);
      } else
        formToken(Result, CurPtr + 1, tok::slash);
      break;

    default:
      Result.setKind(tok::unknown);
  }
  return;
}

void Lexer::identifier(Token &Result) {
  const char *Start = CurPtr;
  const char *End = CurPtr + 1;
  while (charinfo::isIdentifierBody(*End))
    ++End;
  StringRef Name(Start, End - Start);
  formToken(Result, End,
            Keywords.getKeyword(Name, tok::identifier));
}

void Lexer::number(Token &Result) {
  const char *Start = CurPtr;
  const char *End = CurPtr + 1;
  tok::TokenKind Kind = tok::unknown;

  while (charinfo::isDigit(*End)) {
    ++End;
  }

  if (*End == '.' && charinfo::isDigit(*(End + 1))) {
    ++End;
  }

  while (charinfo::isDigit(*End)) {
    ++End;
  }

  Kind = tok::number_literal;
  formToken(Result, End, Kind);
}

void Lexer::string(Token &Result) {
  const char *Start = CurPtr;
  const char *End = CurPtr + 1;
  while (*End && *End != *Start &&
         !charinfo::isVerticalWhitespace(*End))
    ++End;
  if (charinfo::isVerticalWhitespace(*End)) {
    Diags.report(getLoc(),
                 diag::err_unterminated_char_or_string);
  }
  formToken(Result, End + 1, tok::string_literal);
}

void Lexer::comment() {
  const char *End = CurPtr + 2;
  unsigned Level = 1;
  while (*End && Level) {
    // Check for nested comment.
    if (*End == '/' && *(End + 1) == '*') {
      End += 2;
      Level++;
    }
    // Check for end of comment
    else if (*End == '*' && *(End + 1) == '/') {
      End += 2;
      Level--;
    } else
      ++End;
  }
  if (!*End) {
    Diags.report(getLoc(),
                 diag::err_unterminated_block_comment);
  }
  CurPtr = End;
}

void Lexer::formToken(Token &Result, const char *TokEnd,
                      tok::TokenKind Kind) {
  size_t TokLen = TokEnd - CurPtr;
  Result.Ptr = CurPtr;;
  Result.Length = TokLen;
  Result.Kind = Kind;
  CurPtr = TokEnd;
}
