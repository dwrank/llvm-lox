set(LLVM_LINK_COMPONENTS support)

add_lox_library(loxSema
  Scope.cpp
  Sema.cpp

  LINK_LIBS
  loxBasic
  )
