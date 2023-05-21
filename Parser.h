#ifndef PARSER_H
#define PARSER_H

#include "AST.h" // declares the interface for the AST 
#include "Lexer.h"
#include "llvm/Support/raw_ostream.h"

class Parser {
  // declares some private members
  Lexer &Lex; 	// instance of "Lexer" class ("Lex" -> retrieve the next token from the input)
  Token Tok;	// instance of "Token" class ("Tok" -> stores the next token (the look-ahead))
  bool HasError; // "HasError" -> a flag that indicates if an error was detected

  void error() {
    llvm::errs() << "Unexpected: " << Tok.getText() << "\n";
    HasError = true;
  }
  // "advance()" -> retrieves the next token from lexer (use the "next" method of Lexer class)
  void advance() { Lex.next(Tok); }
  
  // "expect()" -> tests if the look-ahead is of the expected kind (if not -> error message)
  bool expect(Token::TokenKind Kind) {	
    if (!Tok.is(Kind)) {	// the look-ahead is  *not* of the expected kind
      error();				// error message
      return true;			// true -> have error in token
    }
    return false;	// the look-ahead is of the expected kind
  }
  
  // "consume()" -> retrieves the next token if the look-ahead is of the expected kind (if not -> error message)
  bool consume(Token::TokenKind Kind) {
    if (expect(Kind))	// an error message is founded (expect returns 'true' for this condition)
      return true; 		// the 'HasError' = true
    advance();
    return false;
  }

  AST  *parseCalc();
  Expr *parseExpr();
  Expr *parseTerm();
  Expr *parseFactor();

// code below : initializes all the members and retrieves the first token from the lexer
public:
  Parser(Lexer &Lex) : Lex(Lex), HasError(false) {
    advance();
  }
  AST *parse(); // the main entry point into parsing
  // "hasError" -> to get value of the error flag
  bool hasError() { return HasError; }
};

#endif