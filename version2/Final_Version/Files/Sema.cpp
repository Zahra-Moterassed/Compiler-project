#include "Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
class DeclCheck : public ASTVisitor {
  // implemented using a visitor
  /*base idea : 'LIVEOUT' and 'UEVAR'
	1. store the name of each declared variable in a set
	2. check each name is unique
	3. check the name is in the set later
  */
  llvm::StringSet<> Scope; 	
  // -> a set to store the names
  bool HasError; 			
  // -> used to indicate that an error occurred

  enum ErrorType { Twice, Not };

  void error(ErrorType ET, llvm::StringRef V) {
    llvm::errs() << "Variable " << V << " "
                 << (ET == Twice ? "already" : "not")
                 << " declared\n";
    HasError = true;
  }

public:
  DeclCheck() : HasError(false) {}

  bool hasError() { return HasError; }
  // RULE-1 : In a 'Factor' node that holds a variable name,...
  // we check that the variable name is in the set
  virtual void visit(Factor &Node) override {
    if (Node.getKind() == Factor::Ident) {
      if (Scope.find(Node.getVal()) == Scope.end())
        error(Not, Node.getVal());
    }
  };
  
  // RULE-2 : check that both sides of the operation exist and have been visited
  // "AST.h" -> 'BinaryOp' class-> 'accept' + 'getLeft' + 'getRight' method
  virtual void visit(BinaryOp &Node) override {	
    if (Node.getLeft())							
	// check the existence of second operand
      Node.getLeft()->accept(*this);
    else
      HasError = true;
    if (Node.getRight())						
	// check the existence of first operand
      Node.getRight()->accept(*this);
    else
      HasError = true;
  };

  // "AST.h" -> 'WithDecl' class-> 'accept' + 'begin' + 'end' method
  virtual void visit(WithDecl &Node) override { 
    for (auto I = Node.begin(), E = Node.end(); I != E; 
	/*will be continued until we reach the end of the expression*/
         ++I) { 
      if (!Scope.insert(*I).second)	
		// check if the declared variables have been stored
        error(Twice, *I);
    }
    if (Node.getExpr())				
		// check if the declared expression has been stored
      Node.getExpr()->accept(*this);
    else
      HasError = true;
  };
};
}
/**
'Sema.h' -> 'semantic' method  
	logic : only starts the tree walk and returns an error flag
**/
bool Sema::semantic(AST *Tree) { 
  if (!Tree) // check if any tree exists (if not => error)
    return false;
  DeclCheck Check;
  Tree->accept(Check);
  return Check.hasError();
}