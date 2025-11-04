#include "brick_loader.hpp"
#include "../../brickc/src/lexer.hpp"
#include "../../brickc/src/parser.hpp"
#include "../../brickc/src/symbols.hpp"
#include "../../brickc/src/error.hpp"
#include "../../brickc/src/util.hpp"
#include <iostream>
#include <sstream>

BrickLoader::BrickLoader() 
    : symbolTable_(nullptr), errorHandler_(nullptr) {
}

BrickLoader::~BrickLoader() {
}

bool BrickLoader::loadBrickFile(const std::string& filename) {
    try {
        errorHandler_ = std::unique_ptr<brick::ErrorHandler>(new brick::ErrorHandler());
        errorHandler_->setCurrentFile(filename);
        
        std::string sourceCode = brick::FileUtils::readFile(filename);
        
        brick::Lexer lexer(sourceCode);
        brick::Parser parser(&lexer, errorHandler_.get());
        
        brick::ASTNode* ast = parser.parseProgram();
        
        if (errorHandler_->hasErrors()) {
            if (ast) {
                brick::deleteAST(ast);
            }
            return false;
        }
        
        if (!ast) {
            return false;
        }
        
        symbolTable_ = std::unique_ptr<brick::SymbolTable>(new brick::SymbolTable(errorHandler_.get()));
        bool success = symbolTable_->populateFromAST(ast);
        
        brick::deleteAST(ast);
        
        if (!success || errorHandler_->hasErrors()) {
            symbolTable_.reset();
            return false;
        }
        
        bool valid = symbolTable_->validate();
        if (!valid || errorHandler_->hasErrors()) {
            symbolTable_.reset();
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading brick file: " << e.what() << std::endl;
        symbolTable_.reset();
        return false;
    }
}

const brick::SymbolTable* BrickLoader::getSymbolTable() const {
    return symbolTable_.get();
}

bool BrickLoader::isLoaded() const {
    return symbolTable_ != nullptr;
}

std::string BrickLoader::getGameName() const {
    if (!symbolTable_) return "";
    return symbolTable_->getGameName();
}

int BrickLoader::getSpeed() const {
    if (!symbolTable_) return 8; // default
    return symbolTable_->hasSpeedSetting() ? symbolTable_->getSpeed() : 8;
}

int BrickLoader::getGridWidth() const {
    if (!symbolTable_) return 30; // default
    return symbolTable_->hasGridSetting() ? symbolTable_->getGridWidth() : 30;
}

int BrickLoader::getGridHeight() const {
    if (!symbolTable_) return 30; // default
    return symbolTable_->hasGridSetting() ? symbolTable_->getGridHeight() : 30;
}

unsigned int BrickLoader::getGameColor() const {
    if (!symbolTable_) return 0x001122; // default
    return symbolTable_->hasColorSetting() ? symbolTable_->getGameColor() : 0x001122;
}

int BrickLoader::getLives() const {
    if (!symbolTable_) {
        return 3;
    }
    return symbolTable_->getLives();
}

int BrickLoader::getScore() const {
    if (!symbolTable_) {
        return 0;
    }
    return symbolTable_->getScore();
}

std::string BrickLoader::getErrorMessages() const {
    if (!errorHandler_) return "";
    
    std::ostringstream oss;
    errorHandler_->printAll(oss);
    return oss.str();
}

const std::map<std::string, brick::Entity>& BrickLoader::getEntities() const {
    static std::map<std::string, brick::Entity> emptyMap;
    if (!symbolTable_) {
        return emptyMap;
    }
    return symbolTable_->getEntities();
}

const brick::Entity* BrickLoader::findEntity(const std::string& name) const {
    if (!symbolTable_) {
        return nullptr;
    }
    return symbolTable_->findEntity(name);
}

bool BrickLoader::hasEntity(const std::string& name) const {
    if (!symbolTable_) {
        return false;
    }
    return symbolTable_->entityExists(name);
}
