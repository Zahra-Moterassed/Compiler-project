#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
/** 	
	the visitor pattern needs to know each class it must visit
	=> declare all classes here
**/
// ** start of declaration
class AST;
class Expr;
class Factor;
class BinaryOp;
class WithDecl;
// ** end of declaration

// "visit()" method -> same for AST and Expr + does nothing :)
class ASTVisitor {
public:
  virtual void visit(AST &){};
  virtual void visit(Expr &){};
  virtual void visit(Factor &) = 0;
  virtual void visit(BinaryOp &) = 0;
  virtual void visit(WithDecl &) = 0;
};
// AST -> the root of hierarchy
class AST {
public:
  virtual ~AST() {}
  virtual void accept(ASTVisitor &V) = 0;
};
// Expr -> the root of the AST classes related to expressions
class Expr : public AST {
public:
  Expr() {}
};
// Factor -> stores the number or the name of a variable
class Factor : public Expr {
public:
  enum ValueKind { Ident, Number };

private:
  ValueKind Kind; 
  // tells us which of both cases the instances represent (ident OR number)
  llvm::StringRef Val;

public:
  Factor(ValueKind Kind, llvm::StringRef Val)
      : Kind(Kind), Val(Val) {}
  ValueKind getKind() { return Kind; }
  llvm::StringRef getVal() { return Val; }
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};
/**
"BinaryOp" -> 
	1. holds the data that's needed to evaluate an expression
	2. makes no distinction between multiplicative (Mul, Div)...
	and additive(Plus, Minus) operators
**/
class BinaryOp : public Expr {
public:
  enum Operator { Plus, Minus, Mul, Div };

private:
  Expr *Left;
  Expr *Right;
  Operator Op;

public:
  BinaryOp(Operator Op, Expr *L, Expr *R)
      : Op(Op), Left(L), Right(R) {}
  Expr *getLeft() { return Left; } 		
  // return the first operand		(left side)
  Expr *getRight() { return Right; }	
  // return the second operand	(right side)
  Operator getOperator() { return Op; }	
  // return the operator
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};

// "WithDecl" -> stores the declared variables and the expression
class WithDecl : public AST {
  using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
  VarVector Vars;
  Expr *E;

public:
  WithDecl(llvm::SmallVector<llvm::StringRef, 8> Vars,
           Expr *E)
      : Vars(Vars), E(E) {}
  VarVector::const_iterator begin() { return Vars.begin(); }
  VarVector::const_iterator end() { return Vars.end(); }
  Expr *getExpr() { return E; }
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};
#endif