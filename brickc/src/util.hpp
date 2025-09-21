#ifndef UTIL_HPP
#define UTIL_HPP
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
namespace brick {
class StringUtils {
public:
    static std::string trim(const std::string& str);
    static std::string trimLeft(const std::string& str);
    static std::string trimRight(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    static std::string join(const std::vector<std::string>& parts, const std::string& separator);
    static std::string toLowerCase(const std::string& str);
    static std::string toUpperCase(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static bool equals(const std::string& a, const std::string& b);
    static bool equalsIgnoreCase(const std::string& a, const std::string& b);
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string toString(int value);
    static std::string toString(unsigned int value);
    static std::string toString(bool value);
    static std::string toString(double value);
    static int toInt(const std::string& str, bool* success = NULL);
    static unsigned int toUInt(const std::string& str, bool* success = NULL);
    static bool toBool(const std::string& str, bool* success = NULL);
    static double toDouble(const std::string& str, bool* success = NULL);
};
class ColorUtils {
public:
    static bool parseHexColor(const std::string& hex, unsigned int* color);
    static std::string colorToHex(unsigned int color);
    static void colorToRGB(unsigned int color, int* r, int* g, int* b);
    static unsigned int rgbToColor(int r, int g, int b);
    static bool isValidHexColor(const std::string& hex);
    static const unsigned int BLACK = 0x000000;
    static const unsigned int WHITE = 0xFFFFFF;
    static const unsigned int RED = 0xFF0000;
    static const unsigned int GREEN = 0x00FF00;
    static const unsigned int BLUE = 0x0000FF;
    static const unsigned int YELLOW = 0xFFFF00;
    static const unsigned int CYAN = 0x00FFFF;
    static const unsigned int MAGENTA = 0xFF00FF;
};
class FileUtils {
public:
    static std::string readFile(const std::string& filename);
    static bool writeFile(const std::string& filename, const std::string& content);
    static bool fileExists(const std::string& filename);
    static long getFileSize(const std::string& filename);
    static std::string getExtension(const std::string& filename);
    static std::string getBasename(const std::string& filename);
    static std::string getDirname(const std::string& filename);
    static std::string joinPath(const std::string& dir, const std::string& file);
    static bool isAbsolutePath(const std::string& path);
    static std::string absolutePath(const std::string& path);
private:
    static char getPathSeparator();
};
class MemoryUtils {
public:
    template<typename T>
    static void safeDelete(T*& ptr) {
        delete ptr;
        ptr = NULL;
    }
    template<typename T>
    static void safeDeleteArray(T*& ptr) {
        delete[] ptr;
        ptr = NULL;
    }
};
class Debug {
private:
    static bool enabled_;
public:
    static void setEnabled(bool enable);
    static bool isEnabled();
    static void print(const std::string& message);
    static void printf(const char* format, ...);
    static void printAST(class ASTNode* root, int indent = 0);
    static void printSymbolTable(const class SymbolTable& table);
};
#ifdef _WIN32
    #define PATH_SEPARATOR '\\'
    #define LINE_ENDING "\r\n"
#else
    #define PATH_SEPARATOR '/'
    #define LINE_ENDING "\n"
#endif
#ifdef _MSC_VER
    #if _MSC_VER < 1300  
        #define BRICK_LEGACY_COMPILER
    #endif
#endif
#ifdef BRICK_LEGACY_COMPILER
    #define for if(false); else for
#endif
#if defined(_WIN32) && defined(_MSC_VER)
    #define BRICK_SPRINTF sprintf_s
    #define BRICK_SNPRINTF _snprintf_s
#else
    #define BRICK_SPRINTF sprintf
    #define BRICK_SNPRINTF snprintf
#endif
} 
#endif 
