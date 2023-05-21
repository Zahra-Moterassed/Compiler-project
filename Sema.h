#ifndef SEMA_H
#define SEMA_H

#include "AST.h"
#include "Lexer.h"

class Sema {
public:
  bool semantic(AST *Tree);
};

#endif
 /**
 semantic analyzer : 
 1. walks the AST
 2. checks for various semantic rules (meaning of the language)
  -- in our language it should check that each used variable is declared--
 3. print out warnings (if it finds a situation that can be improved)
 **/