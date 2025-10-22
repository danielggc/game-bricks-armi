#include "symbols.hpp"
#include "util.hpp"
#include <iostream>
#include <sstream>
namespace brick {
Entity::Entity() : spawn(0, 0), color(0), solid(false), isDefined(false) {
}
Entity::Entity(const std::string& n) : name(n), spawn(0, 0), color(0), solid(false), isDefined(true) {
}
std::string Entity::toString() const {
    std::ostringstream oss;
    oss << name << " {spawn:" << spawn.toString() << ", color:#" 
        << std::hex << color << ", solid:" << (solid ? "true" : "false") << "}";
    return oss.str();
}
void Entity::setSpawn(int x, int y) {
    spawn.x = x;
    spawn.y = y;
}
void Entity::setColor(unsigned int col) {
    color = col;
}
void Entity::setSolid(bool s) {
    solid = s;
}
Control::Control() {
}
Control::Control(const std::string& inp, const std::string& act) : input(inp), action(act) {
}
std::string Control::toString() const {
    return input + " -> " + action;
}
Rule::Rule() {
}
Rule::Rule(const std::string& n) : name(n) {
}
Rule::~Rule() {
}
void Rule::addStatement(ASTNode* stmt) {
    if (stmt) {
        statements.push_back(stmt);
    }
}
std::string Rule::toString() const {
    std::ostringstream oss;
    oss << name << " {" << statements.size() << " statements}";
    return oss.str();
}
SymbolTable::SymbolTable(ErrorHandler* errHandler)
    : speed_(0), gridWidth_(0), gridHeight_(0), gameColor_(0),
      lives_(0), score_(0),
      hasSpeed_(false), hasGrid_(false), hasColor_(false),
      hasLives_(false), hasScore_(false),
      errorHandler_(errHandler) {
}
SymbolTable::~SymbolTable() {
    clear();
}
void SymbolTable::setGameName(const std::string& name) {
    gameName_ = name;
}
void SymbolTable::setSpeed(int speed) {
    speed_ = speed;
    hasSpeed_ = true;
}
void SymbolTable::setGrid(int width, int height) {
    gridWidth_ = width;
    gridHeight_ = height;
    hasGrid_ = true;
}
void SymbolTable::setGameColor(unsigned int color) {
    gameColor_ = color;
    hasColor_ = true;
}
void SymbolTable::setLives(int lives) {
    lives_ = lives;
    hasLives_ = true;
}
void SymbolTable::setScore(int score) {
    score_ = score;
    hasScore_ = true;
}
const std::string& SymbolTable::getGameName() const {
    return gameName_;
}
int SymbolTable::getSpeed() const {
    return speed_;
}
int SymbolTable::getGridWidth() const {
    return gridWidth_;
}
int SymbolTable::getGridHeight() const {
    return gridHeight_;
}
unsigned int SymbolTable::getGameColor() const {
    return gameColor_;
}
int SymbolTable::getLives() const {
    return lives_;
}
int SymbolTable::getScore() const {
    return score_;
}
bool SymbolTable::hasSpeedSetting() const {
    return hasSpeed_;
}
bool SymbolTable::hasGridSetting() const {
    return hasGrid_;
}
bool SymbolTable::hasColorSetting() const {
    return hasColor_;
}
bool SymbolTable::hasLivesSetting() const {
    return hasLives_;
}
bool SymbolTable::hasScoreSetting() const {
    return hasScore_;
}
bool SymbolTable::addEntity(const std::string& name) {
    if (entityExists(name)) {
        reportError("Entidad '" + name + "' ya fue definida");
        return false;
    }
    entities_[name] = Entity(name);
    return true;
}
Entity* SymbolTable::findEntity(const std::string& name) {
    std::map<std::string, Entity>::iterator it = entities_.find(name);
    return (it != entities_.end()) ? &it->second : NULL;
}
const Entity* SymbolTable::findEntity(const std::string& name) const {
    std::map<std::string, Entity>::const_iterator it = entities_.find(name);
    return (it != entities_.end()) ? &it->second : NULL;
}
bool SymbolTable::entityExists(const std::string& name) const {
    return entities_.find(name) != entities_.end();
}
void SymbolTable::setEntitySpawn(const std::string& name, int x, int y) {
    Entity* entity = findEntity(name);
    if (entity) {
        entity->setSpawn(x, y);
    }
}
void SymbolTable::setEntityColor(const std::string& name, unsigned int color) {
    Entity* entity = findEntity(name);
    if (entity) {
        entity->setColor(color);
    }
}
void SymbolTable::setEntitySolid(const std::string& name, bool solid) {
    Entity* entity = findEntity(name);
    if (entity) {
        entity->setSolid(solid);
    }
}
const std::map<std::string, Entity>& SymbolTable::getEntities() const {
    return entities_;
}
size_t SymbolTable::getEntityCount() const {
    return entities_.size();
}
bool SymbolTable::addControl(const std::string& input, const std::string& action) {
    if (controlExists(input)) {
        reportError("Control '" + input + "' ya fue mapeado");
        return false;
    }
    controls_[input] = Control(input, action);
    return true;
}
Control* SymbolTable::findControl(const std::string& input) {
    std::map<std::string, Control>::iterator it = controls_.find(input);
    return (it != controls_.end()) ? &it->second : NULL;
}
const Control* SymbolTable::findControl(const std::string& input) const {
    std::map<std::string, Control>::const_iterator it = controls_.find(input);
    return (it != controls_.end()) ? &it->second : NULL;
}
bool SymbolTable::controlExists(const std::string& input) const {
    return controls_.find(input) != controls_.end();
}
const std::map<std::string, Control>& SymbolTable::getControls() const {
    return controls_;
}
size_t SymbolTable::getControlCount() const {
    return controls_.size();
}
bool SymbolTable::addRule(const std::string& name) {
    if (ruleExists(name)) {
        reportError("Regla '" + name + "' ya fue definida");
        return false;
    }
    rules_[name] = Rule(name);
    return true;
}
Rule* SymbolTable::findRule(const std::string& name) {
    std::map<std::string, Rule>::iterator it = rules_.find(name);
    return (it != rules_.end()) ? &it->second : NULL;
}
const Rule* SymbolTable::findRule(const std::string& name) const {
    std::map<std::string, Rule>::const_iterator it = rules_.find(name);
    return (it != rules_.end()) ? &it->second : NULL;
}
bool SymbolTable::ruleExists(const std::string& name) const {
    return rules_.find(name) != rules_.end();
}
void SymbolTable::addRuleStatement(const std::string& ruleName, ASTNode* statement) {
    Rule* rule = findRule(ruleName);
    if (rule) {
        rule->addStatement(statement);
    }
}
const std::map<std::string, Rule>& SymbolTable::getRules() const {
    return rules_;
}
size_t SymbolTable::getRuleCount() const {
    return rules_.size();
}
bool SymbolTable::populateFromAST(ASTNode* ast) {
    if (!ast) return false;
    if (ast->type == AST_PROGRAM) {
        ProgramNode* program = static_cast<ProgramNode*>(ast);
        setGameName(program->gameName);
        if (program->gameBody && program->gameBody->type == AST_GAME) {
            GameNode* game = static_cast<GameNode*>(program->gameBody);
            processGameNode(game);
        }
    }
    return true;
}
void SymbolTable::processGameNode(GameNode* gameNode) {
    for (size_t i = 0; i < gameNode->settings.size(); ++i) {
        if (gameNode->settings[i]->type == AST_SETTING) {
            SettingNode* setting = static_cast<SettingNode*>(gameNode->settings[i]);
            processSettingNode(setting);
        }
    }
    for (size_t i = 0; i < gameNode->entities.size(); ++i) {
        if (gameNode->entities[i]->type == AST_ENTITY) {
            EntityNode* entity = static_cast<EntityNode*>(gameNode->entities[i]);
            processEntityNode(entity);
        }
    }
    for (size_t i = 0; i < gameNode->rules.size(); ++i) {
        if (gameNode->rules[i]->type == AST_RULE) {
            RuleNode* rule = static_cast<RuleNode*>(gameNode->rules[i]);
            processRuleNode(rule);
        }
    }
}
void SymbolTable::processSettingNode(SettingNode* settingNode) {
    if (settingNode->key == "speed" && settingNode->value && settingNode->value->type == AST_INTEGER) {
        IntegerNode* intNode = static_cast<IntegerNode*>(settingNode->value);
        setSpeed(intNode->value);
    } else if (settingNode->key == "grid" && settingNode->value && settingNode->value->type == AST_POINT) {
        PointNode* pointNode = static_cast<PointNode*>(settingNode->value);
        setGrid(pointNode->point.x, pointNode->point.y);
    } else if (settingNode->key == "color" && settingNode->value && settingNode->value->type == AST_HEXCOLOR) {
        HexColorNode* colorNode = static_cast<HexColorNode*>(settingNode->value);
        setGameColor(colorNode->color);
    } else if (settingNode->key == "lives" && settingNode->value && settingNode->value->type == AST_INTEGER) {
        IntegerNode* intNode = static_cast<IntegerNode*>(settingNode->value);
        setLives(intNode->value);
    } else if (settingNode->key == "score" && settingNode->value && settingNode->value->type == AST_INTEGER) {
        IntegerNode* intNode = static_cast<IntegerNode*>(settingNode->value);
        setScore(intNode->value);
    }
}
void SymbolTable::processEntityNode(EntityNode* entityNode) {
    addEntity(entityNode->name);
    for (size_t i = 0; i < entityNode->properties.size(); ++i) {
        if (entityNode->properties[i]->type == AST_SETTING) {
            SettingNode* prop = static_cast<SettingNode*>(entityNode->properties[i]);
            if (prop->key == "spawn" && prop->value && prop->value->type == AST_POINT) {
                PointNode* pointNode = static_cast<PointNode*>(prop->value);
                setEntitySpawn(entityNode->name, pointNode->point.x, pointNode->point.y);
            } else if (prop->key == "color" && prop->value && prop->value->type == AST_HEXCOLOR) {
                HexColorNode* colorNode = static_cast<HexColorNode*>(prop->value);
                setEntityColor(entityNode->name, colorNode->color);
            } else if (prop->key == "solid" && prop->value && prop->value->type == AST_BOOLEAN) {
                BooleanNode* boolNode = static_cast<BooleanNode*>(prop->value);
                setEntitySolid(entityNode->name, boolNode->value);
            }
        }
    }
}
void SymbolTable::processControlNode(ControlNode* controlNode) {
    addControl(controlNode->input, controlNode->action);
}
void SymbolTable::processRuleNode(RuleNode* ruleNode) {
    addRule(ruleNode->name);
    for (size_t i = 0; i < ruleNode->statements.size(); ++i) {
        addRuleStatement(ruleNode->name, ruleNode->statements[i]);
    }
}
bool SymbolTable::validate() {
    bool valid = true;
    valid &= validateGameSettings();
    valid &= validateEntities();
    valid &= validateControls();
    valid &= validateRules();
    return valid;
}
bool SymbolTable::validateGameSettings() {
    if (gameName_.empty()) {
        reportError("El juego debe tener un nombre");
        return false;
    }
    return true;
}
bool SymbolTable::validateEntities() {
    return !entities_.empty();
}
bool SymbolTable::validateControls() {
    return true;
}
bool SymbolTable::validateRules() {
    return true;
}
void SymbolTable::print(std::ostream& out) const {
    out << "TABLA DE SÍMBOLOS\n";
    out << "=================\n\n";
    out << "Juego: " << gameName_ << "\n";
    if (hasSpeed_) out << "Velocidad: " << speed_ << "\n";
    if (hasGrid_) out << "Grilla: " << gridWidth_ << "x" << gridHeight_ << "\n";
    if (hasColor_) out << "Color: #" << std::hex << gameColor_ << std::dec << "\n";
    if (hasLives_) out << "Vidas: " << lives_ << "\n";
    if (hasScore_) out << "Puntaje: " << score_ << "\n";
    out << "\nEntidades (" << entities_.size() << "):\n";
    for (std::map<std::string, Entity>::const_iterator it = entities_.begin(); 
         it != entities_.end(); ++it) {
        out << "  " << it->second.toString() << "\n";
    }
    out << "\nControles (" << controls_.size() << "):\n";
    for (std::map<std::string, Control>::const_iterator it = controls_.begin(); 
         it != controls_.end(); ++it) {
        out << "  " << it->second.toString() << "\n";
    }
    out << "\nReglas (" << rules_.size() << "):\n";
    for (std::map<std::string, Rule>::const_iterator it = rules_.begin(); 
         it != rules_.end(); ++it) {
        out << "  " << it->second.toString() << "\n";
    }
}
void SymbolTable::printJSON(std::ostream& out) const {
    out << "{\n";
    out << "  \"game\": \"" << gameName_ << "\",\n";
    out << "  \"settings\": {\n";
    if (hasSpeed_) out << "    \"speed\": " << speed_ << ",\n";
    if (hasGrid_) out << "    \"grid\": [" << gridWidth_ << ", " << gridHeight_ << "],\n";
    if (hasColor_) out << "    \"color\": \"#" << std::hex << gameColor_ << std::dec << "\"\n";
    out << "  },\n";
    out << "  \"entities\": " << entities_.size() << ",\n";
    out << "  \"controls\": " << controls_.size() << ",\n";
    out << "  \"rules\": " << rules_.size() << "\n";
    out << "}\n";
}
void SymbolTable::printSummary(std::ostream& out) const {
    out << "Juego '" << gameName_ << "' tiene:\n";
    out << "- " << entities_.size() << " entidades\n";
    out << "- " << controls_.size() << " controles\n";
    out << "- " << rules_.size() << " reglas\n";
}
void SymbolTable::clear() {
    entities_.clear();
    controls_.clear();
    rules_.clear();
    gameName_.clear();
    speed_ = 0;
    gridWidth_ = gridHeight_ = 0;
    gameColor_ = 0;
    hasSpeed_ = hasGrid_ = hasColor_ = false;
}
void SymbolTable::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_->reportSemanticError(0, 0, message);
    }
}
SymbolTableVisitor::SymbolTableVisitor(SymbolTable* table) : symbolTable_(table) {
}
void SymbolTableVisitor::visitProgram(ProgramNode& node) {
}
void SymbolTableVisitor::visitGame(GameNode& node) {
}
void SymbolTableVisitor::visitSetting(SettingNode& node) {
}
void SymbolTableVisitor::visitEntity(EntityNode& node) {
}
void SymbolTableVisitor::visitControl(ControlNode& node) {
}
void SymbolTableVisitor::visitRule(RuleNode& node) {
}
void SymbolTableVisitor::visitTickStmt(TickStmtNode& node) {
}
void SymbolTableVisitor::visitCollideStmt(CollideStmtNode& node) {
}
void SymbolTableVisitor::visitScoreStmt(ScoreStmtNode& node) {
}
void SymbolTableVisitor::visitLoseStmt(LoseStmtNode& node) {
}
void SymbolTableVisitor::visitIdentifier(IdentifierNode& node) {
}
void SymbolTableVisitor::visitInteger(IntegerNode& node) {
}
void SymbolTableVisitor::visitHexColor(HexColorNode& node) {
}
void SymbolTableVisitor::visitBoolean(BooleanNode& node) {
}
void SymbolTableVisitor::visitPoint(PointNode& node) {
}
void SymbolTableVisitor::visitBinaryOp(BinaryOpNode& node) {
}
} 
