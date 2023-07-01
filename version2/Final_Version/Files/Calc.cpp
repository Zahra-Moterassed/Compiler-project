#include "CodeGen.h"
#include "Parser.h"
#include "Sema.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

static llvm::cl::opt<std::string>
    Input(llvm::cl::Positional,
          llvm::cl::desc("<input expression>"),
          llvm::cl::init(""));

int main(int argc, const char **argv) {
  llvm::InitLLVM X(argc, argv);
  llvm::cl::ParseCommandLineOptions( 	// ->	1. to handle the options on the command line
										//		2. to handle printing help information
      argc, argv, "calc - the expression compiler\n");

  Lexer Lex(Input);
  Parser Parser(Lex);
  AST *Tree = Parser.parse();
  if (!Tree || Parser.hasError()) {	// check if errors occured
    llvm::errs() << "Syntax errors occured\n";
    return 1; // exit the compiler with a return code
  }
  Sema Semantic;
  if (Semantic.semantic(Tree)) {	// check for semantic error
    llvm::errs() << "Semantic errors occured\n";
    return 1;
  }
  CodeGen CodeGenerator;
  CodeGenerator.compile(Tree);
  return 0;
}