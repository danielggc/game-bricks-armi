#ifndef PARSER_HPP
#define PARSER_HPP
#include "lexer.hpp"
#include "ast.hpp"
#include "error.hpp"
#include <string>
namespace brick {
class Parser {
private:
    Lexer* lexer;
    Token current;
    ErrorHandler* errorHandler;
    bool hasError;
    void advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    void consume(TokenType type, const std::string& message);
    void error(const std::string& message);
    void synchronize();
    ASTNode* parseExpression();
    ASTNode* parseBinaryExpression(int minPrec = 0);
    ASTNode* parsePrimary();
    ASTNode* parsePoint();
    ASTNode* parseSettings();
    ASTNode* parseSetting();
    ASTNode* parseEntities();
    ASTNode* parseEntity();
    ASTNode* parseEntityProperty();
    ASTNode* parseControls();
    ASTNode* parseControl();
    ASTNode* parseRules();
    ASTNode* parseRule();
    ASTNode* parseRuleStatement();
    ASTNode* parseTickStatement();
    ASTNode* parseCollideStatement();
    ASTNode* parseScoreStatement();
    ASTNode* parseLoseStatement();
    int getOperatorPrecedence(TokenType type) const;
    bool isOperator(TokenType type) const;
    std::string tokenToOperator(TokenType type) const;
public:
    explicit Parser(Lexer* lex, ErrorHandler* errHandler = NULL);
    ASTNode* parseProgram();
    ASTNode* parseGame();
    bool hasErrors() const;
    void resetError();
    ASTNode* parseExpressionOnly();
    ASTNode* parseSettingOnly();
    ASTNode* parseEntityOnly();
    ASTNode* parseRuleOnly();
};
} 
#endif 
