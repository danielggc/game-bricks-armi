#ifndef ERROR_HPP
#define ERROR_HPP
#include <string>
#include <vector>
#include <iostream>
namespace brick {
enum ErrorType {
    ERROR_LEXICAL,
    ERROR_SYNTAX,
    ERROR_SEMANTIC,
    ERROR_SYMBOL,
    ERROR_IO
};
class Error {
public:
    ErrorType type;
    int line;
    int column;
    std::string message;
    std::string filename;
    Error(ErrorType t, int l, int c, const std::string& msg, const std::string& file = "");
    std::string toString() const;
    std::string getTypeName() const;
    static std::string errorTypeName(ErrorType type);
};
class ErrorHandler {
private:
    std::vector<Error> errors;
    bool hasErrors_;
    std::string currentFile;
public:
    ErrorHandler();
    void reportError(ErrorType type, int line, int column, const std::string& message);
    void reportLexicalError(int line, int column, const std::string& message);
    void reportSyntaxError(int line, int column, const std::string& message);
    void reportSemanticError(int line, int column, const std::string& message);
    void reportSymbolError(int line, int column, const std::string& message);
    void reportIOError(const std::string& message);
    bool hasErrors() const;
    size_t getErrorCount() const;
    const std::vector<Error>& getErrors() const;
    const Error& getError(size_t index) const;
    void printAll(std::ostream& out = std::cerr) const;
    void printSummary(std::ostream& out = std::cerr) const;
    void clear();
    void setCurrentFile(const std::string& filename);
    const std::string& getCurrentFile() const;
    int getLexicalErrorCount() const;
    int getSyntaxErrorCount() const;
    int getSemanticErrorCount() const;
    int getSymbolErrorCount() const;
    int getIOErrorCount() const;
};
extern ErrorHandler* g_errorHandler;
void setGlobalErrorHandler(ErrorHandler* handler);
ErrorHandler* getGlobalErrorHandler();
#define LEXICAL_ERROR(handler, line, col, msg) \
    (handler)->reportLexicalError(line, col, msg)
#define SYNTAX_ERROR(handler, line, col, msg) \
    (handler)->reportSyntaxError(line, col, msg)
#define SEMANTIC_ERROR(handler, line, col, msg) \
    (handler)->reportSemanticError(line, col, msg)
#define SYMBOL_ERROR(handler, line, col, msg) \
    (handler)->reportSymbolError(line, col, msg)
#define IO_ERROR(handler, msg) \
    (handler)->reportIOError(msg)
} 
#endif 
