#ifndef LOX_LEXER_LEXER_H
#define LOX_LEXER_LEXER_H

#include "lox/Basic/Diagnostic.h"
#include "lox/Basic/LLVM.h"
#include "lox/Lexer/Token.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"

namespace lox {

class KeywordFilter {
  llvm::StringMap<tok::TokenKind> HashTable;

  void addKeyword(StringRef Keyword,
                  tok::TokenKind TokenCode);

public:
  void addKeywords();

  tok::TokenKind getKeyword(
      StringRef Name,
      tok::TokenKind DefaultTokenCode = tok::unknown) {
    auto Result = HashTable.find(Name);
    if (Result != HashTable.end())
      return Result->second;
    return DefaultTokenCode;
  }
};

class Lexer {
  SourceMgr &SrcMgr;
  DiagnosticsEngine &Diags;

  const char *CurPtr;
  StringRef CurBuf;

  /// CurBuffer - This is the current buffer index we're
  /// lexing from as managed by the SourceMgr object.
  unsigned CurBuffer = 0;

  KeywordFilter Keywords;

public:
  Lexer(SourceMgr &SrcMgr, DiagnosticsEngine &Diags)
      : SrcMgr(SrcMgr), Diags(Diags) {
    CurBuffer = SrcMgr.getMainFileID();
    CurBuf = SrcMgr.getMemoryBuffer(CurBuffer)->getBuffer();
    CurPtr = CurBuf.begin();
    Keywords.addKeywords();
  }

  DiagnosticsEngine &getDiagnostics() const {
    return Diags;
  }

  /// Returns the next token from the input.
  void next(Token &Result);

  /// Gets source code buffer.
  StringRef getBuffer() const { return CurBuf; }

private:
  void identifier(Token &Result);
  void number(Token &Result);
  void string(Token &Result);
  void comment();

  SMLoc getLoc() { return SMLoc::getFromPointer(CurPtr); }

  void formToken(Token &Result, const char *TokEnd,
                 tok::TokenKind Kind);
};
} // namespace lox
#endif