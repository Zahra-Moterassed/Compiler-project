#include "CodeGen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm; // -> used for name lookups
/**
	LOGIC : AST contains the information from semantic analysis phase
	basic idea : use a visitor to walk the AST
**/
namespace {
class ToIRVisitor : public ASTVisitor {
	/**
	each compilation unit is represented in LLVM by the 'Module' class
	=> a pointer to the module call in visitor
	**/
  Module *M;	
  IRBuilder<> Builder; // -> to represent types in IR
  // 'Type's below are used as caches to avoid repeated lookup
  // ** start of initialization of caches
  Type *VoidTy;
  Type *Int32Ty;
  Type *Int8PtrTy;
  Type *Int8PtrPtrTy;
  Constant *Int32Zero;
  // ** end of initialization of caches
  
  Value *V; // = the current calculated value (updated through tree traversal)
  
  // "nameMap" -> maps a variable name to the value that's returned by the 'calc_read()' function
  StringMap<Value *> nameMap;

public:
  ToIRVisitor(Module *M) : M(M), Builder(M->getContext()) {
    VoidTy = Type::getVoidTy(M->getContext());
    Int32Ty = Type::getInt32Ty(M->getContext());
    Int8PtrTy = Type::getInt8PtrTy(M->getContext());
    Int8PtrPtrTy = Int8PtrTy->getPointerTo();
    Int32Zero = ConstantInt::get(Int32Ty, 0, true);
  }

  void run(AST *Tree) {
    FunctionType *MainFty = FunctionType::get(
        Int32Ty, {Int32Ty, Int8PtrPtrTy}, false);
    Function *MainFn = Function::Create(
        MainFty, GlobalValue::ExternalLinkage, "main", M);
	// create the 'BB' basic block with the *entry* label
    BasicBlock *BB = BasicBlock::Create(M->getContext(),
                                        "entry", MainFn);
	// attached the BB to the IR builder
    Builder.SetInsertPoint(BB);
	// begin the tree traversal
    Tree->accept(*this);

    FunctionType *CalcWriteFnTy =
        FunctionType::get(VoidTy, {Int32Ty}, false);
    Function *CalcWriteFn = Function::Create(
        CalcWriteFnTy, GlobalValue::ExternalLinkage,
        "calc_write", M); // 'calc_write' -> prints the computed value
    Builder.CreateCall(CalcWriteFnTy, CalcWriteFn, {V});
	/**
	Builder.CreateRet:
		1. returning a 0 from the 'main()' function
		2. finish generation
	**/
    Builder.CreateRet(Int32Zero);
  }
  // 'Factor' : is for variable name or a number
  /**
	1. variable name -> look up the 'mapNames' map
	2. number -> convert to an integer + turn inyo constant value
  **/
  virtual void visit(Factor &Node) override {
    if (Node.getKind() == Factor::Ident) { 	// variable name
      V = nameMap[Node.getVal()];
    } else { 								// number
      int intval;
      Node.getVal().getAsInteger(10, intval);
      V = ConstantInt::get(Int32Ty, intval, true);
    }
  };

  virtual void visit(BinaryOp &Node) override {
    Node.getLeft()->accept(*this);
    Value *Left = V;
    Node.getRight()->accept(*this);
    Value *Right = V;
    switch (Node.getOperator()) {
    case BinaryOp::Plus:
      V = Builder.CreateNSWAdd(Left, Right);
      break;
    case BinaryOp::Minus:
      V = Builder.CreateNSWSub(Left, Right);
      break;
    case BinaryOp::Mul:
      V = Builder.CreateNSWMul(Left, Right);
      break;
    case BinaryOp::Div:
      V = Builder.CreateSDiv(Left, Right);
      break;
    }
  };
  // 'WithDecl' : holds the names of the declared variables...
  // (GENERAL EXPRESSION FOR THIS PART OF CODE)
  virtual void visit(WithDecl &Node) override {
    FunctionType *ReadFty =
        FunctionType::get(Int32Ty, {Int8PtrTy}, false);
		
	// create a function prototype for the 'calc_read()' function
	// ** start of creation
    Function *ReadFn = Function::Create(
        ReadFty, GlobalValue::ExternalLinkage, "calc_read",
        M);
	// ** end of creation
	
    for (auto I = Node.begin(), E = Node.end(); I != E;
         ++I) {
      StringRef Var = *I; 
	  // create a string with a variable name for each variable

      // Create IR code to call 'calc_read()' function
      Constant *StrText = ConstantDataArray::getString(
          M->getContext(), Var);
      GlobalVariable *Str = new GlobalVariable(
          *M, StrText->getType(),
          /*isConstant=*/true, GlobalValue::PrivateLinkage,
          StrText, Twine(Var).concat(".str"));		  
      Value *Ptr = Builder.CreateInBoundsGEP(
          Str, {Int32Zero, Int32Zero}, "ptr");
      CallInst *Call = // call 'calc_read()' (ReadFn)
          Builder.CreateCall(ReadFty, ReadFn, {Ptr});
	  // the returned value is stored in the 'mapNames' map for later use
      nameMap[Var] = Call;
    }
	// continue tree traverse with the expression
    Node.getExpr()->accept(*this);
  };
};
} // namespace
/**
compile() method :
	1. creates the global context and the module
	2. runs the tree traversal
	3. dumps the general IR to the console
**/
void CodeGen::compile(AST *Tree) {
  LLVMContext Ctx;
  Module *M = new Module("calc.expr", Ctx);
  ToIRVisitor ToIR(M);
  ToIR.run(Tree);
  M->print(outs(), nullptr);
}