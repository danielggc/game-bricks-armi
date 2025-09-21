#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>

namespace brick {

class ASTNode;
class Visitor;

enum ASTNodeType {
    AST_PROGRAM,
    AST_GAME,
    AST_SETTINGS,
    AST_SETTING,
    AST_ENTITIES,
    AST_ENTITY,
    AST_ENTITY_PROP,
    AST_CONTROLS,
    AST_CONTROL,
    AST_RULES,
    AST_RULE,
    AST_RULE_STMT,
    AST_TICK_STMT,
    AST_COLLIDE_STMT,
    AST_SCORE_STMT,
    AST_LOSE_STMT,
    AST_EXPRESSION,
    AST_BINARY_OP,
    AST_IDENTIFIER,
    AST_INTEGER,
    AST_HEXCOLOR,
    AST_BOOLEAN,
    AST_POINT
};

// Point structure
struct Point {
    int x, y;
    
    Point() : x(0), y(0) {}
    Point(int x_, int y_) : x(x_), y(y_) {}
    
    std::string toString() const;
};

// Base AST Node class
class ASTNode {
public:
    ASTNodeType type;
    int line;
    int column;
    
    explicit ASTNode(ASTNodeType t, int l = 0, int c = 0);
    virtual ~ASTNode();
    
    virtual void accept(Visitor& visitor) = 0;
    
    std::string getTypeName() const;
    virtual std::string toString() const = 0;
    
protected:
    static std::string nodeTypeName(ASTNodeType type);
};

class ProgramNode : public ASTNode {
public:
    std::string gameName;
    ASTNode* gameBody;
    
    ProgramNode(const std::string& name, ASTNode* body);
    ~ProgramNode();
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class GameNode : public ASTNode {
public:
    std::string name;
    std::vector<ASTNode*> settings;
    std::vector<ASTNode*> entities;
    std::vector<ASTNode*> controls;
    std::vector<ASTNode*> rules;
    
    explicit GameNode(const std::string& gameName);
    ~GameNode();
    
    void addSetting(ASTNode* setting);
    void addEntity(ASTNode* entity);
    void addControl(ASTNode* control);
    void addRule(ASTNode* rule);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class SettingNode : public ASTNode {
public:
    std::string key;
    ASTNode* value;
    
    SettingNode(const std::string& k, ASTNode* v);
    ~SettingNode();
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class EntityNode : public ASTNode {
public:
    std::string name;
    std::vector<ASTNode*> properties;
    
    explicit EntityNode(const std::string& entityName);
    ~EntityNode();
    
    void addProperty(ASTNode* property);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class ControlNode : public ASTNode {
public:
    std::string input;
    std::string action;
    
    ControlNode(const std::string& inp, const std::string& act);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class RuleNode : public ASTNode {
public:
    std::string name;
    std::vector<ASTNode*> statements;
    
    explicit RuleNode(const std::string& ruleName);
    ~RuleNode();
    
    void addStatement(ASTNode* stmt);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class TickStmtNode : public ASTNode {
public:
    std::string action;
    std::vector<ASTNode*> args;
    
    explicit TickStmtNode(const std::string& act);
    ~TickStmtNode();
    
    void addArg(ASTNode* arg);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class CollideStmtNode : public ASTNode {
public:
    std::string entity1;
    std::string entity2;
    std::string action;
    std::vector<ASTNode*> args;
    
    CollideStmtNode(const std::string& e1, const std::string& e2, const std::string& act);
    ~CollideStmtNode();
    
    void addArg(ASTNode* arg);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class ScoreStmtNode : public ASTNode {
public:
    bool isPositive;
    int value;
    
    ScoreStmtNode(bool positive, int val);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class LoseStmtNode : public ASTNode {
public:
    LoseStmtNode();
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class IdentifierNode : public ASTNode {
public:
    std::string name;
    
    explicit IdentifierNode(const std::string& n);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class IntegerNode : public ASTNode {
public:
    int value;
    
    explicit IntegerNode(int val);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class HexColorNode : public ASTNode {
public:
    unsigned int color; // RGB as 0xRRGGBB
    
    explicit HexColorNode(unsigned int col);
    
    void accept(Visitor& visitor);
    std::string toString() const;
    
    // Utility functions
    int getRed() const;
    int getGreen() const;
    int getBlue() const;
};

class BooleanNode : public ASTNode {
public:
    bool value;
    
    explicit BooleanNode(bool val);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class PointNode : public ASTNode {
public:
    Point point;
    
    PointNode(int x, int y);
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class BinaryOpNode : public ASTNode {
public:
    std::string operator_;
    ASTNode* left;
    ASTNode* right;
    
    BinaryOpNode(const std::string& op, ASTNode* l, ASTNode* r);
    ~BinaryOpNode();
    
    void accept(Visitor& visitor);
    std::string toString() const;
};

class Visitor {
public:
    virtual ~Visitor() {}
    
    virtual void visitProgram(ProgramNode& node) = 0;
    virtual void visitGame(GameNode& node) = 0;
    virtual void visitSetting(SettingNode& node) = 0;
    virtual void visitEntity(EntityNode& node) = 0;
    virtual void visitControl(ControlNode& node) = 0;
    virtual void visitRule(RuleNode& node) = 0;
    virtual void visitTickStmt(TickStmtNode& node) = 0;
    virtual void visitCollideStmt(CollideStmtNode& node) = 0;
    virtual void visitScoreStmt(ScoreStmtNode& node) = 0;
    virtual void visitLoseStmt(LoseStmtNode& node) = 0;
    virtual void visitIdentifier(IdentifierNode& node) = 0;
    virtual void visitInteger(IntegerNode& node) = 0;
    virtual void visitHexColor(HexColorNode& node) = 0;
    virtual void visitBoolean(BooleanNode& node) = 0;
    virtual void visitPoint(PointNode& node) = 0;
    virtual void visitBinaryOp(BinaryOpNode& node) = 0;
};

void printAST(ASTNode* root, int indent = 0);
void deleteAST(ASTNode* root);

} 
#endif 
