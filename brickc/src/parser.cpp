#include "parser.hpp"
#include "util.hpp"
#include <iostream>
namespace brick {
Parser::Parser(Lexer* lex, ErrorHandler* errHandler)
    : lexer(lex), errorHandler(errHandler), hasError(false) {
    if (lexer) {
        current = lexer->nextToken();
    }
}
void Parser::advance() {
    if (lexer) {
        current = lexer->nextToken();
    }
}
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}
bool Parser::check(TokenType type) const {
    return current.type == type;
}
void Parser::consume(TokenType type, const std::string& message) {
    if (current.type == type) {
        advance();
    } else {
        error(message);
    }
}
void Parser::error(const std::string& message) {
    hasError = true;
    if (errorHandler) {
        errorHandler->reportSyntaxError(current.line, current.column, message);
    }
}
void Parser::synchronize() {
    hasError = false;
    while (current.type != T_EOF) {
        if (current.type == T_SEMICOLON) {
            advance();
            return;
        }
        switch (current.type) {
            case T_GAME:
            case T_ENTITY:
            case T_CONTROLS:
            case T_RULE:
                return;
            default:
                break;
        }
        advance();
    }
}
ASTNode* Parser::parseProgram() {
    if (!match(T_GAME)) {
        error("Se esperaba 'game'");
        return NULL;
    }
    if (current.type != T_IDENT) {
        error("Se esperaba nombre del juego");
        return NULL;
    }
    std::string gameName = current.lexeme;
    advance();
    consume(T_LBRACE, "Se esperaba '{' después del nombre del juego");
    GameNode* gameNode = new GameNode(gameName);
    while (current.type != T_RBRACE && current.type != T_EOF) {
        if (current.type == T_SPEED || current.type == T_GRID || current.type == T_COLOR || 
            current.type == T_SCORE) {
            ASTNode* setting = parseSetting();
            if (setting) {
                gameNode->addSetting(setting);
            }
        } else if (current.type == T_IDENT && current.lexeme == "lives") {
            // Tratar "lives" como configuración especial
            ASTNode* setting = parseSetting();
            if (setting) {
                gameNode->addSetting(setting);
            }
        } else if (current.type == T_ENTITY) {
            ASTNode* entity = parseEntity();
            if (entity) {
                gameNode->addEntity(entity);
            }
        } else if (current.type == T_CONTROLS) {
            ASTNode* controls = parseControls();
            if (controls) {
                gameNode->addControl(controls);
            }
        } else if (current.type == T_RULE) {
            ASTNode* rule = parseRule();
            if (rule) {
                gameNode->addRule(rule);
            }
        } else {
            error("Elemento inesperado en el cuerpo del juego");
            synchronize();
        }
        if (hasError) {
            synchronize();
        }
    }
    consume(T_RBRACE, "Se esperaba '}' al final del juego");
    return new ProgramNode(gameName, gameNode);
}
ASTNode* Parser::parseGame() {
    return NULL;
}
ASTNode* Parser::parseSetting() {
    TokenType settingType = current.type;
    std::string key = current.lexeme;
    advance();
    consume(T_COLON, "Se esperaba ':' después del nombre de configuración");
    ASTNode* value = NULL;
    if (settingType == T_GRID) {
        consume(T_LPAREN, "Se esperaba '(' para configuración de grilla");
        if (current.type != T_INT) {
            error("Se esperaba número entero para ancho de grilla");
            return NULL;
        }
        int width = StringUtils::toInt(current.lexeme);
        advance();
        consume(T_COMMA, "Se esperaba ',' entre dimensiones de grilla");
        if (current.type != T_INT) {
            error("Se esperaba número entero para alto de grilla");
            return NULL;
        }
        int height = StringUtils::toInt(current.lexeme);
        advance();
        consume(T_RPAREN, "Se esperaba ')' al final de configuración de grilla");
        value = new PointNode(width, height);
    } else if (settingType == T_SPEED || settingType == T_SCORE || 
               (settingType == T_IDENT && key == "lives")) {
        if (current.type == T_INT) {
            value = new IntegerNode(StringUtils::toInt(current.lexeme));
            advance();
        } else {
            error("Se esperaba número entero para " + key);
            return NULL;
        }
    } else if (settingType == T_COLOR) {
        if (current.type == T_HEXCOLOR) {
            unsigned int color;
            if (ColorUtils::parseHexColor(current.lexeme, &color)) {
                value = new HexColorNode(color);
            } else {
                error("Color hexadecimal inválido");
                return NULL;
            }
            advance();
        } else {
            error("Se esperaba color hexadecimal");
            return NULL;
        }
    }
    consume(T_SEMICOLON, "Se esperaba ';' al final de configuración");
    return new SettingNode(key, value);
}
ASTNode* Parser::parseEntity() {
    consume(T_ENTITY, "Se esperaba 'entity'");
    if (current.type != T_IDENT) {
        error("Se esperaba nombre de entidad");
        return NULL;
    }
    std::string entityName = current.lexeme;
    advance();
    consume(T_LBRACE, "Se esperaba '{' después del nombre de entidad");
    EntityNode* entityNode = new EntityNode(entityName);
    while (current.type != T_RBRACE && current.type != T_EOF) {
        ASTNode* property = parseEntityProperty();
        if (property) {
            entityNode->addProperty(property);
        }
        if (hasError) {
            synchronize();
        }
    }
    consume(T_RBRACE, "Se esperaba '}' al final de entidad");
    return entityNode;
}
ASTNode* Parser::parseEntityProperty() {
    if (current.type == T_SPAWN || current.type == T_COLOR || current.type == T_SOLID) {
        std::string key = current.lexeme;
        advance();
        consume(T_COLON, "Se esperaba ':' después de propiedad de entidad");
        ASTNode* value = NULL;
        if (key == "spawn") {
            consume(T_LPAREN, "Se esperaba '(' para posición de spawn");
            if (current.type != T_INT) {
                error("Se esperaba coordenada X para spawn");
                return NULL;
            }
            int x = StringUtils::toInt(current.lexeme);
            advance();
            consume(T_COMMA, "Se esperaba ',' entre coordenadas");
            if (current.type != T_INT) {
                error("Se esperaba coordenada Y para spawn");
                return NULL;
            }
            int y = StringUtils::toInt(current.lexeme);
            advance();
            consume(T_RPAREN, "Se esperaba ')' al final de spawn");
            value = new PointNode(x, y);
        } else if (key == "color") {
            if (current.type == T_HEXCOLOR) {
                unsigned int color;
                if (ColorUtils::parseHexColor(current.lexeme, &color)) {
                    value = new HexColorNode(color);
                } else {
                    error("Color hexadecimal inválido");
                    return NULL;
                }
                advance();
            } else {
                error("Se esperaba color hexadecimal");
                return NULL;
            }
        } else if (key == "solid") {
            if (current.type == T_TRUE || current.type == T_FALSE) {
                value = new BooleanNode(current.type == T_TRUE);
                advance();
            } else {
                error("Se esperaba 'true' o 'false' para solid");
                return NULL;
            }
        }
        consume(T_SEMICOLON, "Se esperaba ';' al final de propiedad");
        return new SettingNode(key, value);
    } else {
        error("Propiedad de entidad no reconocida");
        return NULL;
    }
}
ASTNode* Parser::parseControls() {
    consume(T_CONTROLS, "Se esperaba 'controls'");
    consume(T_LBRACE, "Se esperaba '{' después de 'controls'");
    while (current.type != T_RBRACE && current.type != T_EOF) {
        advance(); 
    }
    consume(T_RBRACE, "Se esperaba '}' al final de controls");
    return new IdentifierNode("controls"); 
}
ASTNode* Parser::parseRule() {
    consume(T_RULE, "Se esperaba 'rule'");
    if (current.type != T_IDENT) {
        error("Se esperaba nombre de regla");
        return NULL;
    }
    std::string ruleName = current.lexeme;
    advance();
    consume(T_LBRACE, "Se esperaba '{' después del nombre de regla");
    while (current.type != T_RBRACE && current.type != T_EOF) {
        advance(); 
    }
    consume(T_RBRACE, "Se esperaba '}' al final de regla");
    return new RuleNode(ruleName);
}
ASTNode* Parser::parseExpression() {
    return new IntegerNode(0);
}
ASTNode* Parser::parseBinaryExpression(int minPrec) {
    return new IntegerNode(0);
}
ASTNode* Parser::parsePrimary() {
    return new IntegerNode(0);
}
ASTNode* Parser::parsePoint() {
    return new PointNode(0, 0);
}
ASTNode* Parser::parseSettings() {
    return NULL;
}
ASTNode* Parser::parseEntities() {
    return NULL;
}
ASTNode* Parser::parseControl() {
    return NULL;
}
ASTNode* Parser::parseRules() {
    return NULL;
}
ASTNode* Parser::parseRuleStatement() {
    return NULL;
}
ASTNode* Parser::parseTickStatement() {
    return NULL;
}
ASTNode* Parser::parseCollideStatement() {
    return NULL;
}
ASTNode* Parser::parseScoreStatement() {
    return NULL;
}
ASTNode* Parser::parseLoseStatement() {
    return NULL;
}
bool Parser::hasErrors() const {
    return hasError;
}
void Parser::resetError() {
    hasError = false;
}
int Parser::getOperatorPrecedence(TokenType type) const {
    return 0;
}
bool Parser::isOperator(TokenType type) const {
    return false;
}
std::string Parser::tokenToOperator(TokenType type) const {
    return "";
}
ASTNode* Parser::parseExpressionOnly() {
    return parseExpression();
}
ASTNode* Parser::parseSettingOnly() {
    return parseSetting();
}
ASTNode* Parser::parseEntityOnly() {
    return parseEntity();
}
ASTNode* Parser::parseRuleOnly() {
    return parseRule();
}
} 
