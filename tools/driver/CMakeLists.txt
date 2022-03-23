set(LLVM_LINK_COMPONENTS
  Support
  )

add_lox_tool(lox
  Driver.cpp
  )

target_link_libraries(lox
  PRIVATE
  loxBasic
  loxLexer
  )
#loxParser
#loxSema
