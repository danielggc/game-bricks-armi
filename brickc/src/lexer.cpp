#include "lexer.hpp"
#include "util.hpp"
#include <cctype>
#include <cstdlib>
namespace brick {
Token::Token() : type(T_EOF), line(0), column(0) {
}
Token::Token(TokenType t, const std::string& lex, int l, int c)
    : type(t), lexeme(lex), line(l), column(c) {
}
std::string Token::toString() const {
    return Lexer::tokenTypeName(type) + "(" + lexeme + ")";
}
Lexer::Lexer() : pos(0), line(1), column(1) {
}
Lexer::Lexer(const std::string& src) : source(src), pos(0), line(1), column(1) {
}
void Lexer::init(const std::string& src) {
    source = src;
    pos = 0;
    line = 1;
    column = 1;
    current = Token();
}
char Lexer::currentChar() const {
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}
char Lexer::nextChar() {
    if (pos >= source.length()) {
        return '\0';
    }
    char c = source[pos++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}
void Lexer::skipWhitespace() {
    while (pos < source.length() && std::isspace(currentChar())) {
        nextChar();
    }
}
void Lexer::skipComment() {
    if (pos + 1 < source.length() && source[pos] == '/' && source[pos + 1] == '/') {
        while (pos < source.length() && currentChar() != '\n') {
            nextChar();
        }
    }
}
Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token(type, lexeme, line, column - lexeme.length());
}
Token Lexer::makeIdentifierOrKeyword() {
    int startLine = line;
    int startColumn = column;
    std::string lexeme;
    while (pos < source.length() && (std::isalnum(currentChar()) || currentChar() == '_')) {
        lexeme += nextChar();
    }
    TokenType type = T_IDENT;
    if (isKeyword(lexeme)) {
        if (lexeme == "game") type = T_GAME;
        else if (lexeme == "speed") type = T_SPEED;
        else if (lexeme == "grid") type = T_GRID;
        else if (lexeme == "color") type = T_COLOR;
        else if (lexeme == "entity") type = T_ENTITY;
        else if (lexeme == "spawn") type = T_SPAWN;
        else if (lexeme == "solid") type = T_SOLID;
        else if (lexeme == "controls") type = T_CONTROLS;
        else if (lexeme == "rule") type = T_RULE;
        else if (lexeme == "tick") type = T_TICK;
        else if (lexeme == "collide") type = T_COLLIDE;
        else if (lexeme == "score") type = T_SCORE;
        else if (lexeme == "lose") type = T_LOSE;
        else if (lexeme == "true") type = T_TRUE;
        else if (lexeme == "false") type = T_FALSE;
    }
    return Token(type, lexeme, startLine, startColumn);
}
Token Lexer::makeNumber() {
    int startLine = line;
    int startColumn = column;
    std::string lexeme;
    while (pos < source.length() && std::isdigit(currentChar())) {
        lexeme += nextChar();
    }
    if (pos < source.length() && std::isalpha(currentChar())) {
        while (pos < source.length() && (std::isalnum(currentChar()) || currentChar() == '_')) {
            lexeme += nextChar();
        }
        return Token(T_ERROR, lexeme, startLine, startColumn);
    }
    return Token(T_INT, lexeme, startLine, startColumn);
}
Token Lexer::makeHexColor() {
    int startLine = line;
    int startColumn = column;
    std::string lexeme;
    lexeme += nextChar(); 
    int hexCount = 0;
    while (pos < source.length() && std::isxdigit(currentChar()) && hexCount < 6) {
        lexeme += nextChar();
        hexCount++;
    }
    if (hexCount != 6) {
        while (pos < source.length() && (std::isalnum(currentChar()) || currentChar() == '_')) {
            lexeme += nextChar();
        }
        return Token(T_ERROR, lexeme, startLine, startColumn);
    }
    return Token(T_HEXCOLOR, lexeme, startLine, startColumn);
}
Token Lexer::makeTwoCharOperator(char first, char second, TokenType type) {
    int startLine = line;
    int startColumn = column;
    std::string lexeme;
    lexeme += nextChar(); 
    lexeme += nextChar(); 
    return Token(type, lexeme, startLine, startColumn);
}
Token Lexer::nextToken() {
    while (true) {
        skipWhitespace();
        if (pos + 1 < source.length() && source[pos] == '/' && source[pos + 1] == '/') {
            skipComment();
            continue; 
        }
        break; 
    }
    if (pos >= source.length()) {
        current = Token(T_EOF, "", line, column);
        return current;
    }
    char c = currentChar();
    if (std::isalpha(c) || c == '_') {
        current = makeIdentifierOrKeyword();
        return current;
    }
    if (std::isdigit(c)) {
        current = makeNumber();
        return current;
    }
    if (c == '#') {
        current = makeHexColor();
        return current;
    }
    if (pos + 1 < source.length()) {
        char next = source[pos + 1];
        if (c == '<' && next == '=') {
            current = makeTwoCharOperator(c, next, T_LESSEQ);
            return current;
        }
        if (c == '>' && next == '=') {
            current = makeTwoCharOperator(c, next, T_GREATEREQ);
            return current;
        }
        if (c == '=' && next == '=') {
            current = makeTwoCharOperator(c, next, T_EQUAL);
            return current;
        }
        if (c == '!' && next == '=') {
            current = makeTwoCharOperator(c, next, T_NOTEQUAL);
            return current;
        }
    }
    std::string lexeme;
    lexeme += nextChar();
    TokenType type = T_ERROR;
    switch (c) {
        case '{': type = T_LBRACE; break;
        case '}': type = T_RBRACE; break;
        case '(': type = T_LPAREN; break;
        case ')': type = T_RPAREN; break;
        case ':': type = T_COLON; break;
        case ';': type = T_SEMICOLON; break;
        case ',': type = T_COMMA; break;
        case '=': type = T_ASSIGN; break;
        case '+': type = T_PLUS; break;
        case '-': type = T_MINUS; break;
        case '*': type = T_MULTIPLY; break;
        case '/': type = T_DIVIDE; break;
        case '<': type = T_LESS; break;
        case '>': type = T_GREATER; break;
        default:
            type = T_ERROR;
            break;
    }
    current = Token(type, lexeme, line, column - 1);
    return current;
}
Token Lexer::peekToken() {
    size_t savedPos = pos;
    int savedLine = line;
    int savedColumn = column;
    Token peek = nextToken();
    pos = savedPos;
    line = savedLine;
    column = savedColumn;
    return peek;
}
const Token& Lexer::getCurrentToken() const {
    return current;
}
std::string Lexer::tokenTypeName(TokenType type) {
    switch (type) {
        case T_EOF: return "EOF";
        case T_ERROR: return "ERROR";
        case T_IDENT: return "IDENT";
        case T_INT: return "INT";
        case T_HEXCOLOR: return "HEXCOLOR";
        case T_TRUE: return "TRUE";
        case T_FALSE: return "FALSE";
        case T_GAME: return "GAME";
        case T_SPEED: return "SPEED";
        case T_GRID: return "GRID";
        case T_COLOR: return "COLOR";
        case T_ENTITY: return "ENTITY";
        case T_SPAWN: return "SPAWN";
        case T_SOLID: return "SOLID";
        case T_CONTROLS: return "CONTROLS";
        case T_RULE: return "RULE";
        case T_TICK: return "TICK";
        case T_COLLIDE: return "COLLIDE";
        case T_SCORE: return "SCORE";
        case T_LOSE: return "LOSE";
        case T_LBRACE: return "LBRACE";
        case T_RBRACE: return "RBRACE";
        case T_LPAREN: return "LPAREN";
        case T_RPAREN: return "RPAREN";
        case T_COLON: return "COLON";
        case T_SEMICOLON: return "SEMICOLON";
        case T_COMMA: return "COMMA";
        case T_ASSIGN: return "ASSIGN";
        case T_PLUS: return "PLUS";
        case T_MINUS: return "MINUS";
        case T_MULTIPLY: return "MULTIPLY";
        case T_DIVIDE: return "DIVIDE";
        case T_LESS: return "LESS";
        case T_GREATER: return "GREATER";
        case T_LESSEQ: return "LESSEQ";
        case T_GREATEREQ: return "GREATEREQ";
        case T_EQUAL: return "EQUAL";
        case T_NOTEQUAL: return "NOTEQUAL";
        default: return "UNKNOWN";
    }
}
bool Lexer::isKeyword(const std::string& word) {
    return word == "game" || word == "speed" || word == "grid" || word == "color" ||
           word == "entity" || word == "spawn" || word == "solid" || word == "controls" ||
           word == "rule" || word == "tick" || word == "collide" || word == "score" ||
           word == "lose" || word == "true" || word == "false";
}
bool Lexer::hasError() const {
    return current.type == T_ERROR;
}
std::string Lexer::getErrorMessage() const {
    if (current.type == T_ERROR) {
        return "Token inválido: '" + current.lexeme + "'";
    }
    return "";
}
} 
