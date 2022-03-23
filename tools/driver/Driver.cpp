#include "lox/Basic/Diagnostic.h"
#include "lox/Basic/Version.h"
#include "lox/Lexer/Lexer.h"
//#include "lox/Parser/Parser.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

using namespace lox;

int main(int argc_, const char **argv_) {
  llvm::InitLLVM X(argc_, argv_);
  llvm::SmallVector<const char *, 256> argv(argv_ + 1,
                                            argv_ + argc_);

  llvm::outs() << "Lox "
               << lox::getLoxVersion() << "\n";

  for (const char *F : argv) {
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
        FileOrErr = llvm::MemoryBuffer::getFile(F);
    if (std::error_code BufferError =
            FileOrErr.getError()) {
      llvm::errs() << "Error reading " << F << ": "
                   << BufferError.message() << "\n";
      continue;
    }

    llvm::SourceMgr SrcMgr;
    DiagnosticsEngine Diags(SrcMgr);

    // Tell SrcMgr about this buffer, which is what the
    // parser will pick up.
    SrcMgr.AddNewSourceBuffer(std::move(*FileOrErr),
                              llvm::SMLoc());

    auto lexer = Lexer(SrcMgr, Diags);
    Token tok;
    while (tok.isNot(tok::eof)) {
      lexer.next(tok);
      llvm::outs() << tok.toString() << "\n";
    }
    //auto sema = Sema(Diags);
    //auto parser = Parser(lexer, sema);
    //parser.parse();
  }
}
