#include "ast.hpp"
#include <sstream>
#include <iostream>
namespace brick {
std::string Point::toString() const {
    std::ostringstream oss;
    oss << "(" << x << ", " << y << ")";
    return oss.str();
}
ASTNode::ASTNode(ASTNodeType t, int l, int c) : type(t), line(l), column(c) {
}
ASTNode::~ASTNode() {
}
std::string ASTNode::getTypeName() const {
    return nodeTypeName(type);
}
std::string ASTNode::nodeTypeName(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "Program";
        case AST_GAME: return "Game";
        case AST_SETTINGS: return "Settings";
        case AST_SETTING: return "Setting";
        case AST_ENTITIES: return "Entities";
        case AST_ENTITY: return "Entity";
        case AST_ENTITY_PROP: return "EntityProperty";
        case AST_CONTROLS: return "Controls";
        case AST_CONTROL: return "Control";
        case AST_RULES: return "Rules";
        case AST_RULE: return "Rule";
        case AST_RULE_STMT: return "RuleStatement";
        case AST_TICK_STMT: return "TickStatement";
        case AST_COLLIDE_STMT: return "CollideStatement";
        case AST_SCORE_STMT: return "ScoreStatement";
        case AST_LOSE_STMT: return "LoseStatement";
        case AST_EXPRESSION: return "Expression";
        case AST_BINARY_OP: return "BinaryOperation";
        case AST_IDENTIFIER: return "Identifier";
        case AST_INTEGER: return "Integer";
        case AST_HEXCOLOR: return "HexColor";
        case AST_BOOLEAN: return "Boolean";
        case AST_POINT: return "Point";
        default: return "Unknown";
    }
}
ProgramNode::ProgramNode(const std::string& name, ASTNode* body)
    : ASTNode(AST_PROGRAM), gameName(name), gameBody(body) {
}
ProgramNode::~ProgramNode() {
    delete gameBody;
}
void ProgramNode::accept(Visitor& visitor) {
    visitor.visitProgram(*this);
}
std::string ProgramNode::toString() const {
    return "Program(" + gameName + ")";
}
GameNode::GameNode(const std::string& gameName) : ASTNode(AST_GAME), name(gameName) {
}
GameNode::~GameNode() {
    for (size_t i = 0; i < settings.size(); ++i) {
        delete settings[i];
    }
    for (size_t i = 0; i < entities.size(); ++i) {
        delete entities[i];
    }
    for (size_t i = 0; i < controls.size(); ++i) {
        delete controls[i];
    }
    for (size_t i = 0; i < rules.size(); ++i) {
        delete rules[i];
    }
}
void GameNode::addSetting(ASTNode* setting) {
    if (setting) {
        settings.push_back(setting);
    }
}
void GameNode::addEntity(ASTNode* entity) {
    if (entity) {
        entities.push_back(entity);
    }
}
void GameNode::addControl(ASTNode* control) {
    if (control) {
        controls.push_back(control);
    }
}
void GameNode::addRule(ASTNode* rule) {
    if (rule) {
        rules.push_back(rule);
    }
}
void GameNode::accept(Visitor& visitor) {
    visitor.visitGame(*this);
}
std::string GameNode::toString() const {
    return "Game(" + name + ")";
}
SettingNode::SettingNode(const std::string& k, ASTNode* v)
    : ASTNode(AST_SETTING), key(k), value(v) {
}
SettingNode::~SettingNode() {
    delete value;
}
void SettingNode::accept(Visitor& visitor) {
    visitor.visitSetting(*this);
}
std::string SettingNode::toString() const {
    return "Setting(" + key + ")";
}
EntityNode::EntityNode(const std::string& entityName) : ASTNode(AST_ENTITY), name(entityName) {
}
EntityNode::~EntityNode() {
    for (size_t i = 0; i < properties.size(); ++i) {
        delete properties[i];
    }
}
void EntityNode::addProperty(ASTNode* property) {
    if (property) {
        properties.push_back(property);
    }
}
void EntityNode::accept(Visitor& visitor) {
    visitor.visitEntity(*this);
}
std::string EntityNode::toString() const {
    return "Entity(" + name + ")";
}
ControlNode::ControlNode(const std::string& inp, const std::string& act)
    : ASTNode(AST_CONTROL), input(inp), action(act) {
}
void ControlNode::accept(Visitor& visitor) {
    visitor.visitControl(*this);
}
std::string ControlNode::toString() const {
    return "Control(" + input + " -> " + action + ")";
}
RuleNode::RuleNode(const std::string& ruleName) : ASTNode(AST_RULE), name(ruleName) {
}
RuleNode::~RuleNode() {
    for (size_t i = 0; i < statements.size(); ++i) {
        delete statements[i];
    }
}
void RuleNode::addStatement(ASTNode* stmt) {
    if (stmt) {
        statements.push_back(stmt);
    }
}
void RuleNode::accept(Visitor& visitor) {
    visitor.visitRule(*this);
}
std::string RuleNode::toString() const {
    return "Rule(" + name + ")";
}
TickStmtNode::TickStmtNode(const std::string& act) : ASTNode(AST_TICK_STMT), action(act) {
}
TickStmtNode::~TickStmtNode() {
    for (size_t i = 0; i < args.size(); ++i) {
        delete args[i];
    }
}
void TickStmtNode::addArg(ASTNode* arg) {
    if (arg) {
        args.push_back(arg);
    }
}
void TickStmtNode::accept(Visitor& visitor) {
    visitor.visitTickStmt(*this);
}
std::string TickStmtNode::toString() const {
    return "Tick(" + action + ")";
}
CollideStmtNode::CollideStmtNode(const std::string& e1, const std::string& e2, const std::string& act)
    : ASTNode(AST_COLLIDE_STMT), entity1(e1), entity2(e2), action(act) {
}
CollideStmtNode::~CollideStmtNode() {
    for (size_t i = 0; i < args.size(); ++i) {
        delete args[i];
    }
}
void CollideStmtNode::addArg(ASTNode* arg) {
    if (arg) {
        args.push_back(arg);
    }
}
void CollideStmtNode::accept(Visitor& visitor) {
    visitor.visitCollideStmt(*this);
}
std::string CollideStmtNode::toString() const {
    return "Collide(" + entity1 + ", " + entity2 + " -> " + action + ")";
}
ScoreStmtNode::ScoreStmtNode(bool positive, int val) : ASTNode(AST_SCORE_STMT), isPositive(positive), value(val) {
}
void ScoreStmtNode::accept(Visitor& visitor) {
    visitor.visitScoreStmt(*this);
}
std::string ScoreStmtNode::toString() const {
    std::ostringstream oss;
    oss << "Score(" << (isPositive ? "+" : "-") << value << ")";
    return oss.str();
}
LoseStmtNode::LoseStmtNode() : ASTNode(AST_LOSE_STMT) {
}
void LoseStmtNode::accept(Visitor& visitor) {
    visitor.visitLoseStmt(*this);
}
std::string LoseStmtNode::toString() const {
    return "Lose";
}
IdentifierNode::IdentifierNode(const std::string& n) : ASTNode(AST_IDENTIFIER), name(n) {
}
void IdentifierNode::accept(Visitor& visitor) {
    visitor.visitIdentifier(*this);
}
std::string IdentifierNode::toString() const {
    return "Identifier(" + name + ")";
}
IntegerNode::IntegerNode(int val) : ASTNode(AST_INTEGER), value(val) {
}
void IntegerNode::accept(Visitor& visitor) {
    visitor.visitInteger(*this);
}
std::string IntegerNode::toString() const {
    std::ostringstream oss;
    oss << "Integer(" << value << ")";
    return oss.str();
}
HexColorNode::HexColorNode(unsigned int col) : ASTNode(AST_HEXCOLOR), color(col) {
}
void HexColorNode::accept(Visitor& visitor) {
    visitor.visitHexColor(*this);
}
std::string HexColorNode::toString() const {
    std::ostringstream oss;
    oss << "HexColor(#" << std::hex << color << ")";
    return oss.str();
}
int HexColorNode::getRed() const {
    return (color >> 16) & 0xFF;
}
int HexColorNode::getGreen() const {
    return (color >> 8) & 0xFF;
}
int HexColorNode::getBlue() const {
    return color & 0xFF;
}
BooleanNode::BooleanNode(bool val) : ASTNode(AST_BOOLEAN), value(val) {
}
void BooleanNode::accept(Visitor& visitor) {
    visitor.visitBoolean(*this);
}
std::string BooleanNode::toString() const {
    return value ? "Boolean(true)" : "Boolean(false)";
}
PointNode::PointNode(int x, int y) : ASTNode(AST_POINT), point(x, y) {
}
void PointNode::accept(Visitor& visitor) {
    visitor.visitPoint(*this);
}
std::string PointNode::toString() const {
    return "Point" + point.toString();
}
BinaryOpNode::BinaryOpNode(const std::string& op, ASTNode* l, ASTNode* r)
    : ASTNode(AST_BINARY_OP), operator_(op), left(l), right(r) {
}
BinaryOpNode::~BinaryOpNode() {
    delete left;
    delete right;
}
void BinaryOpNode::accept(Visitor& visitor) {
    visitor.visitBinaryOp(*this);
}
std::string BinaryOpNode::toString() const {
    return "BinaryOp(" + operator_ + ")";
}
void printAST(ASTNode* root, int indent) {
    if (!root) return;
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << root->toString() << std::endl;
}
void deleteAST(ASTNode* root) {
    delete root;
}
} 
