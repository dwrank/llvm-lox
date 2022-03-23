set(LLVM_LINK_COMPONENTS support)

add_lox_library(loxParser
  Parser.cpp

  LINK_LIBS
  loxBasic
  loxLexer
  )
