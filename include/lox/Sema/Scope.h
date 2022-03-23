#ifndef LOX_SEMA_SCOPE_H
#define LOX_SEMA_SCOPE_H

#include "lox/Basic/LLVM.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

namespace lox {

class Decl;

class Scope {
  Scope *Parent;
  StringMap<Decl *> Symbols;

public:
  Scope(Scope *Parent = nullptr) : Parent(Parent) {}

  bool insert(Decl *Declaration);
  Decl *lookup(StringRef Name);

  Scope *getParent() { return Parent; }
};
} // namespace lox
#endif