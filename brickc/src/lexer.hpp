#ifndef LEXER_HPP
#define LEXER_HPP
#include <string>
#include <vector>
namespace brick {
enum TokenType {
    T_EOF = 0,
    T_ERROR,
    T_IDENT,
    T_INT,
    T_HEXCOLOR,
    T_TRUE,
    T_FALSE,
    T_GAME,
    T_SPEED,
    T_GRID,
    T_COLOR,
    T_ENTITY,
    T_SPAWN,
    T_SOLID,
    T_CONTROLS,
    T_RULE,
    T_TICK,
    T_COLLIDE,
    T_SCORE,
    T_LOSE,
    T_LIVES,
    T_LBRACE = '{',
    T_RBRACE = '}',
    T_LPAREN = '(',
    T_RPAREN = ')',
    T_COLON = ':',
    T_SEMICOLON = ';',
    T_COMMA = ',',
    T_ASSIGN = '=',
    T_PLUS = '+',
    T_MINUS = '-',
    T_MULTIPLY = '*',
    T_DIVIDE = '/',
    T_LESS = '<',
    T_GREATER = '>',
    T_LESSEQ = 256,
    T_GREATEREQ,
    T_EQUAL,
    T_NOTEQUAL
};
class Token {
public:
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    Token();
    Token(TokenType t, const std::string& lex, int l, int c);
    std::string toString() const;
    bool isKeyword() const;
    bool isLiteral() const;
    bool isOperator() const;
};
class Lexer {
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    Token current;
    char currentChar() const;
    char nextChar();
    void skipWhitespace();
    void skipComment();
    Token makeToken(TokenType type, const std::string& lexeme);
    Token makeIdentifierOrKeyword();
    Token makeNumber();
    Token makeHexColor();
    Token makeTwoCharOperator(char first, char second, TokenType type);
public:
    Lexer();
    explicit Lexer(const std::string& src);
    void init(const std::string& src);
    Token nextToken();
    Token peekToken();
    const Token& getCurrentToken() const;
    static std::string tokenTypeName(TokenType type);
    static bool isKeyword(const std::string& word);
    bool hasError() const;
    std::string getErrorMessage() const;
};
}
#endif
