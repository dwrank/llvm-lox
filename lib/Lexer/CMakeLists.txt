set(LLVM_LINK_COMPONENTS support)

add_lox_library(loxLexer
  Lexer.cpp

  LINK_LIBS
  loxBasic
  )
