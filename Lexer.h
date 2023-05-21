// The implementation of a lexical analyzer
// Lexical analyzer is to define tokens
#ifndef LEXER_H
#define LEXER_H

// "StringRef" capsulates a pointer to a C string and its length 
// => no need to terminate with a zero character
#include "llvm/ADT/StringRef.h"
// "MemoryBuffer" is used to provide read-only access to a block of memery
#include "llvm/Support/MemoryBuffer.h" 

class Lexer;
// Token -> contains the definition of the enumeration for the unique token numbers 
class Token {
  friend class Lexer;

public:
  enum TokenKind : unsigned short {	// the definition of the enumeration for the unique token numbers
  
    eoi, 							// eoi -> stands for end of input 
									// (is returned when all the characters of the input are processed)
    unknown,						// unkniwn -> is used in case of an error at the lexical level
    ident,
    number,
    comma,
    equation,
    plus,
    minus,
    star,
    slash,
	semicolon,
    l_paren,
    r_paren,
    KW_TypeInt
  };
// two parts below are useful for semantic processing
// ** start of mentioned part
private:
  TokenKind Kind;
  // 1.points to the start of the text of the token
  // 2.StringRef is used
  llvm::StringRef Text; 

public:
  TokenKind getKind() const { return Kind; }
  llvm::StringRef getText() const {
    return Text;
  }
// ** end of mentioned part

// "is" , "isOneOf" -> useful to test if the token is of a certain kind
  bool is(TokenKind K) const { return Kind == K; }
  
 // "isOneOf" -> can be used for variable number of arguments
  bool isOneOf(TokenKind K1, TokenKind K2) const {
    return is(K1) || is(K2);
  }
  template <typename... Ts>
  bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks) const {
    return is(K1) || isOneOf(K2, Ks...);
  }
};

class Lexer {
  const char *BufferStart;
  const char *BufferPtr;

public:
  Lexer(const llvm::StringRef &Buffer) {
	// "BufferStart" , "BufferPtr" -> pointers to the beginning of the input + next unprocessed character
	// *hint : it is assumed that the buffer ends with a terminating 0 (like a C string)
    BufferStart = Buffer.begin(); 
    BufferPtr = BufferStart;
  }
  // "next" method -> returns the next token (acts like an iterator , advancing to the next available token)
  void next(Token &token);

private:
  void formToken(Token &Result, const char *TokEnd,
                 Token::TokenKind Kind);
};
#endif