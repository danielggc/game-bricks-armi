#ifndef SYMBOLS_HPP
#define SYMBOLS_HPP
#include "ast.hpp"
#include "error.hpp"
#include <string>
#include <vector>
#include <map>
namespace brick {
class SymbolTable;
class Entity {
public:
    std::string name;
    Point spawn;
    unsigned int color;
    bool solid;
    bool isDefined;
    Entity();
    Entity(const std::string& n);
    std::string toString() const;
    void setSpawn(int x, int y);
    void setColor(unsigned int col);
    void setSolid(bool s);
};
class Control {
public:
    std::string input;
    std::string action;
    Control();
    Control(const std::string& inp, const std::string& act);
    std::string toString() const;
};
class Rule {
public:
    std::string name;
    std::vector<ASTNode*> statements;
    Rule();
    explicit Rule(const std::string& n);
    ~Rule();
    void addStatement(ASTNode* stmt);
    std::string toString() const;
};
class SymbolTable {
private:
    std::string gameName_;
    int speed_;
    int gridWidth_;
    int gridHeight_;
    unsigned int gameColor_;
    int lives_;
    int score_;
    std::map<std::string, Entity> entities_;
    std::map<std::string, Control> controls_;
    std::map<std::string, Rule> rules_;
    bool hasSpeed_;
    bool hasGrid_;
    bool hasColor_;
    bool hasLives_;
    bool hasScore_;
    ErrorHandler* errorHandler_;
public:
    explicit SymbolTable(ErrorHandler* errHandler = NULL);
    ~SymbolTable();
    void setGameName(const std::string& name);
    void setSpeed(int speed);
    void setGrid(int width, int height);
    void setGameColor(unsigned int color);
    void setLives(int lives);
    void setScore(int score);
    const std::string& getGameName() const;
    int getSpeed() const;
    int getGridWidth() const;
    int getGridHeight() const;
    unsigned int getGameColor() const;
    int getLives() const;
    int getScore() const;
    bool hasSpeedSetting() const;
    bool hasGridSetting() const;
    bool hasColorSetting() const;
    bool hasLivesSetting() const;
    bool hasScoreSetting() const;
    bool addEntity(const std::string& name);
    Entity* findEntity(const std::string& name);
    const Entity* findEntity(const std::string& name) const;
    bool entityExists(const std::string& name) const;
    void setEntitySpawn(const std::string& name, int x, int y);
    void setEntityColor(const std::string& name, unsigned int color);
    void setEntitySolid(const std::string& name, bool solid);
    const std::map<std::string, Entity>& getEntities() const;
    size_t getEntityCount() const;
    bool addControl(const std::string& input, const std::string& action);
    Control* findControl(const std::string& input);
    const Control* findControl(const std::string& input) const;
    bool controlExists(const std::string& input) const;
    const std::map<std::string, Control>& getControls() const;
    size_t getControlCount() const;
    bool addRule(const std::string& name);
    Rule* findRule(const std::string& name);
    const Rule* findRule(const std::string& name) const;
    bool ruleExists(const std::string& name) const;
    void addRuleStatement(const std::string& ruleName, ASTNode* statement);
    const std::map<std::string, Rule>& getRules() const;
    size_t getRuleCount() const;
    bool populateFromAST(ASTNode* ast);
    bool validate();
    bool validateGameSettings();
    bool validateEntities();
    bool validateControls();
    bool validateRules();
    void print(std::ostream& out = std::cout) const;
    void printJSON(std::ostream& out = std::cout) const;
    void printSummary(std::ostream& out = std::cout) const;
    void clear();
private:
    void reportError(const std::string& message);
    void processGameNode(GameNode* gameNode);
    void processSettingNode(SettingNode* settingNode);
    void processEntityNode(EntityNode* entityNode);
    void processControlNode(ControlNode* controlNode);
    void processRuleNode(RuleNode* ruleNode);
};
class SymbolTableVisitor : public Visitor {
private:
    SymbolTable* symbolTable_;
public:
    explicit SymbolTableVisitor(SymbolTable* table);
    void visitProgram(ProgramNode& node);
    void visitGame(GameNode& node);
    void visitSetting(SettingNode& node);
    void visitEntity(EntityNode& node);
    void visitControl(ControlNode& node);
    void visitRule(RuleNode& node);
    void visitTickStmt(TickStmtNode& node);
    void visitCollideStmt(CollideStmtNode& node);
    void visitScoreStmt(ScoreStmtNode& node);
    void visitLoseStmt(LoseStmtNode& node);
    void visitIdentifier(IdentifierNode& node);
    void visitInteger(IntegerNode& node);
    void visitHexColor(HexColorNode& node);
    void visitBoolean(BooleanNode& node);
    void visitPoint(PointNode& node);
    void visitBinaryOp(BinaryOpNode& node);
};
} 
#endif 
