#include "error.hpp"
#include <iostream>
#include <sstream>
#include <cstdarg>
#include <cstdio>
namespace brick {
ErrorHandler* g_errorHandler = NULL;
void setGlobalErrorHandler(ErrorHandler* handler) {
    g_errorHandler = handler;
}
ErrorHandler* getGlobalErrorHandler() {
    return g_errorHandler;
}
Error::Error(ErrorType t, int l, int c, const std::string& msg, const std::string& file)
    : type(t), line(l), column(c), message(msg), filename(file) {
}
std::string Error::toString() const {
    std::ostringstream oss;
    if (!filename.empty()) {
        oss << filename << ":";
    }
    oss << "L" << line << ":C" << column << ": " << message;
    return oss.str();
}
std::string Error::getTypeName() const {
    return errorTypeName(type);
}
std::string Error::errorTypeName(ErrorType type) {
    switch (type) {
        case ERROR_LEXICAL: return "Error Léxico";
        case ERROR_SYNTAX:  return "Error Sintáctico";
        case ERROR_SEMANTIC: return "Error Semántico";
        case ERROR_SYMBOL:  return "Error de Símbolo";
        case ERROR_IO:      return "Error de E/S";
        default:            return "Error Desconocido";
    }
}
ErrorHandler::ErrorHandler() : hasErrors_(false) {
}
void ErrorHandler::reportError(ErrorType type, int line, int column, const std::string& message) {
    errors.push_back(Error(type, line, column, message, currentFile));
    hasErrors_ = true;
}
void ErrorHandler::reportLexicalError(int line, int column, const std::string& message) {
    reportError(ERROR_LEXICAL, line, column, message);
}
void ErrorHandler::reportSyntaxError(int line, int column, const std::string& message) {
    reportError(ERROR_SYNTAX, line, column, message);
}
void ErrorHandler::reportSemanticError(int line, int column, const std::string& message) {
    reportError(ERROR_SEMANTIC, line, column, message);
}
void ErrorHandler::reportSymbolError(int line, int column, const std::string& message) {
    reportError(ERROR_SYMBOL, line, column, message);
}
void ErrorHandler::reportIOError(const std::string& message) {
    reportError(ERROR_IO, 0, 0, message);
}
bool ErrorHandler::hasErrors() const {
    return hasErrors_;
}
size_t ErrorHandler::getErrorCount() const {
    return errors.size();
}
const std::vector<Error>& ErrorHandler::getErrors() const {
    return errors;
}
const Error& ErrorHandler::getError(size_t index) const {
    return errors[index];
}
void ErrorHandler::printAll(std::ostream& out) const {
    for (size_t i = 0; i < errors.size(); ++i) {
        out << errors[i].toString() << std::endl;
    }
}
void ErrorHandler::printSummary(std::ostream& out) const {
    if (errors.empty()) {
        out << "No hay errores." << std::endl;
        return;
    }
    out << "Total de errores: " << errors.size() << std::endl;
    int lexical = getLexicalErrorCount();
    int syntax = getSyntaxErrorCount();
    int semantic = getSemanticErrorCount();
    int symbol = getSymbolErrorCount();
    int io = getIOErrorCount();
    if (lexical > 0) out << "  Errores léxicos: " << lexical << std::endl;
    if (syntax > 0)  out << "  Errores sintácticos: " << syntax << std::endl;
    if (semantic > 0) out << "  Errores semánticos: " << semantic << std::endl;
    if (symbol > 0)  out << "  Errores de símbolos: " << symbol << std::endl;
    if (io > 0)      out << "  Errores de E/S: " << io << std::endl;
}
void ErrorHandler::clear() {
    errors.clear();
    hasErrors_ = false;
}
void ErrorHandler::setCurrentFile(const std::string& filename) {
    currentFile = filename;
}
const std::string& ErrorHandler::getCurrentFile() const {
    return currentFile;
}
int ErrorHandler::getLexicalErrorCount() const {
    int count = 0;
    for (size_t i = 0; i < errors.size(); ++i) {
        if (errors[i].type == ERROR_LEXICAL) count++;
    }
    return count;
}
int ErrorHandler::getSyntaxErrorCount() const {
    int count = 0;
    for (size_t i = 0; i < errors.size(); ++i) {
        if (errors[i].type == ERROR_SYNTAX) count++;
    }
    return count;
}
int ErrorHandler::getSemanticErrorCount() const {
    int count = 0;
    for (size_t i = 0; i < errors.size(); ++i) {
        if (errors[i].type == ERROR_SEMANTIC) count++;
    }
    return count;
}
int ErrorHandler::getSymbolErrorCount() const {
    int count = 0;
    for (size_t i = 0; i < errors.size(); ++i) {
        if (errors[i].type == ERROR_SYMBOL) count++;
    }
    return count;
}
int ErrorHandler::getIOErrorCount() const {
    int count = 0;
    for (size_t i = 0; i < errors.size(); ++i) {
        if (errors[i].type == ERROR_IO) count++;
    }
    return count;
}
} 
