#ifndef CODEGEN_H
#define CODEGEN_H

#include "AST.h"

// *LOGIC : AST -> IR of the module -> optimized machine code
class CodeGen
{
public:
 void compile(AST *Tree);

};
#endif