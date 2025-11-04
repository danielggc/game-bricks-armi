#ifndef BRICK_LOADER_HPP
#define BRICK_LOADER_HPP

#include <string>
#include <memory>
#include <map>

namespace brick {
    class SymbolTable;
    class ErrorHandler;
    class Entity;
}

class BrickLoader {
private:
    std::unique_ptr<brick::SymbolTable> symbolTable_;
    std::unique_ptr<brick::ErrorHandler> errorHandler_;
    
public:
    BrickLoader();
    ~BrickLoader();
    
    bool loadBrickFile(const std::string& filename);
    const brick::SymbolTable* getSymbolTable() const;
    bool isLoaded() const;
    std::string getGameName() const;
    int getSpeed() const;
    int getGridWidth() const;
    int getGridHeight() const;
    unsigned int getGameColor() const;
    int getLives() const;
    int getScore() const;
    std::string getErrorMessages() const;
    
    const std::map<std::string, brick::Entity>& getEntities() const;
    const brick::Entity* findEntity(const std::string& name) const;
    bool hasEntity(const std::string& name) const;
};

#endif // BRICK_LOADER_HPP
