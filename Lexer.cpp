#include "Lexer.h"
// first part contains some helper functions to help classify characters
// these functions are used to make conditions more readable
// ** start of mentioned part
namespace charinfo {
LLVM_READNONE inline bool isWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\f' || c == '\v' ||
         c == '\r' || c == '\n';
}

LLVM_READNONE inline bool isDigit(char c) {
  return c >= '0' && c <= '9';
}

LLVM_READNONE inline bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
}
// ** end of mentioned part

// "next" method -> ignoring the characters not defined in the grammar 
// ** start of "if" conditions of method "next"
void Lexer::next(Token &token) {
  // PART 1 : ignoring whitespace + new line (defined in helper function "isWhitespace")
  while (*BufferPtr && charinfo::isWhitespace(*BufferPtr)) {
    ++BufferPtr;
  }
  // PART 2 : make sure there are still characters left to process
  // (check if we have reached the end of string)
  if (!*BufferPtr) {
    token.Kind = Token::eoi;
    return;
  }
  // PART 3 :
  // prob 1 -> check if the character is lowercase or uppercase
  // prob 2 -> check if the token is identifier or "type int" keyword
  // =>
  // step 1. collect all characters matched by the regular expression
  if (charinfo::isLetter(*BufferPtr)) {
    const char *end = BufferPtr + 1;
    while (charinfo::isLetter(*end))
      ++end;
    llvm::StringRef Name(BufferPtr, end - BufferPtr);
	// step 2. check if the string happens to be the keyword
    Token::TokenKind kind =
        Name == "type int" ? Token::KW_TypeInt : Token::ident;
    formToken(token, end, kind);
    return;
  // PART 4 : check for numbers (similar to "PART2" code)
  } else if (charinfo::isDigit(*BufferPtr)) {
    const char *end = BufferPtr + 1;
    while (charinfo::isDigit(*end))
      ++end;
    formToken(token, end, Token::number);
    return;
// ** end of "if" conditions of method "next" => only the tokens defined by fixed strings are left
  } else {
    switch (*BufferPtr) { // reason of usage (reduce typing): all tokens have only one character 
#define CASE(ch, tok) \
case ch: formToken(token, BufferPtr + 1, tok); break
CASE('+', Token::plus);
CASE('-', Token::minus);
CASE('*', Token::star);
CASE('/', Token::slash);
CASE('(', Token::Token::l_paren);
CASE(')', Token::Token::r_paren);
CASE('=', Token::Token::equation);
CASE(',', Token::Token::comma);
CASE(';', Token::Token::semicolon);
#undef CASE
	// check for unexpected characters
    default:
      formToken(token, BufferPtr + 1, Token::unknown);
    }
    return;
  }
}
/** "fromToken" method ->
	1. populates the member of the "Token" instance
	2. update the pointer to the next unprocessed character
**/
void Lexer::formToken(Token &Tok, const char *TokEnd,
                      Token::TokenKind Kind) {
  Tok.Kind = Kind;
  Tok.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
  BufferPtr = TokEnd;
}