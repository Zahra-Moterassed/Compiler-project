#include "Parser.h"

// ** RECURSIVE DESCENT PARSER ** //
AST *Parser::parse() {
  AST *Res = parseCalc(); // "parseCalc()" -> implements the corresponding rule
  expect(Token::eoi);
  return Res;
}

// calc : ("type int" ident ("," ident)*)? expr  ;
AST *Parser::parseCalc() {
  Expr *E;
  llvm::SmallVector<llvm::StringRef, 8> Vars;
  // step 1 : the group begins with "type int" token
  // => compare the token to this value
  if (Tok.is(Token::KW_TypeInt)) {
    advance();
  // step 2. expect an identifier
  // if it is an identifier => save it in the 'Vars' vector (if not => error message)
    if (expect(Token::ident))
      goto _error;
    Vars.push_back(Tok.getText());
    advance();
	//the loop below :a repeating group, which parses more identifiers, it's separated by a comma
    while (Tok.is(Token::comma)) {
      advance();
      if (expect(Token::ident))
        goto _error;
      Vars.push_back(Tok.getText());
      advance();
    }
  }
  E = parseExpr(); // in this part, the rule 'expr' must be parsed
  
  // ** THIS PART IS ADDED BY MYSELF ()the semicolon at the end of string
  //if (consume(Token::semicolon))
      //goto _error;
  // ** END OF ADDITION
  
  if (expect(Token::eoi))
    goto _error;

// by passing the code above (check the end of input)
// => create AST node from collected information
  if (Vars.empty())
    return E;
  else
    return new WithDecl(Vars, E);

_error:
  while (Tok.getKind() != Token::eoi)
    advance();
  return nullptr;
}

// expr : ident "=" term (( "+" | "-" ) term)*  ; 
Expr *Parser::parseExpr() {
  // ** THIS PART IS ADDED BY MYSELF (the first part of the expr (ident "="))
  if (Tok.is(Token::ident)) {
    advance();
    if (expect(Token::equation))
      goto _error;
    Vars.push_back(Tok.getText());
    advance();
  }
  // ** END OF ADDITION
  Expr *Left = parseTerm();
  while (Tok.isOneOf(Token::plus, Token::minus)) {
    BinaryOp::Operator Op = Tok.is(Token::plus)
                                ? BinaryOp::Plus
                                : BinaryOp::Minus;
    advance();
    Expr *Right = parseTerm();
    Left = new BinaryOp(Op, Left, Right);
  }
  return Left;
}

// term : factor (( "*" | "/" ) term)* ;
Expr *Parser::parseTerm() {
  Expr *Left = parseFactor();
  while (Tok.isOneOf(Token::star, Token::slash)) {
    BinaryOp::Operator Op =
        Tok.is(Token::star) ? BinaryOp::Mul : BinaryOp::Div;
    advance();
    Expr *Right = parseFactor();
    Left = new BinaryOp(Op, Left, Right);
  }
  return Left;
}

// factor : ident | number | "("expr")" ;
Expr *Parser::parseFactor() {
  Expr *Res = nullptr;
  // switch-case is more suitable than if-else statement here
  switch (Tok.getKind()) {
  case Token::number:
    Res = new Factor(Factor::Number, Tok.getText());
    advance(); break;
  case Token::ident:
    Res = new Factor(Factor::Ident, Tok.getText());
    advance(); break;
  case Token::l_paren:
    advance();
    Res = parseExpr();
    if (!consume(Token::r_paren)) break;
  default:
    if (!Res)
      error();
    while (!Tok.isOneOf(Token::r_paren, Token::star,
                        Token::plus, Token::minus,
                        Token::slash, Token::eoi))
      advance();
  }
  return Res;
}