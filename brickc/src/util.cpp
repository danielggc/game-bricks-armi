#include "util.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstdio>
namespace brick {
bool Debug::enabled_ = false;
std::string StringUtils::trim(const std::string& str) {
    return trimLeft(trimRight(str));
}
std::string StringUtils::trimLeft(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start);
}
std::string StringUtils::trimRight(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}
std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}
std::string StringUtils::join(const std::vector<std::string>& parts, const std::string& separator) {
    if (parts.empty()) return "";
    std::ostringstream result;
    result << parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result << separator << parts[i];
    }
    return result.str();
}
std::string StringUtils::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
std::string StringUtils::toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}
bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && 
           str.substr(0, prefix.length()) == prefix;
}
bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() &&
           str.substr(str.length() - suffix.length()) == suffix;
}
bool StringUtils::equals(const std::string& a, const std::string& b) {
    return a == b;
}
bool StringUtils::equalsIgnoreCase(const std::string& a, const std::string& b) {
    return toLowerCase(a) == toLowerCase(b);
}
std::string StringUtils::toString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
std::string StringUtils::toString(unsigned int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
std::string StringUtils::toString(bool value) {
    return value ? "true" : "false";
}
int StringUtils::toInt(const std::string& str, bool* success) {
    std::istringstream iss(str);
    int result;
    iss >> result;
    if (success) {
        *success = !iss.fail() && iss.eof();
    }
    return result;
}
unsigned int StringUtils::toUInt(const std::string& str, bool* success) {
    std::istringstream iss(str);
    unsigned int result;
    iss >> result;
    if (success) {
        *success = !iss.fail() && iss.eof();
    }
    return result;
}
bool StringUtils::toBool(const std::string& str, bool* success) {
    std::string lower = toLowerCase(str);
    if (lower == "true" || lower == "1") {
        if (success) *success = true;
        return true;
    } else if (lower == "false" || lower == "0") {
        if (success) *success = true;
        return false;
    } else {
        if (success) *success = false;
        return false;
    }
}
bool ColorUtils::parseHexColor(const std::string& hex, unsigned int* color) {
    if (hex.length() != 7 || hex[0] != '#') {
        return false;
    }
    for (size_t i = 1; i < 7; ++i) {
        char c = hex[i];
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
            return false;
        }
    }
    std::istringstream iss(hex.substr(1));
    unsigned int result;
    iss >> std::hex >> result;
    if (color) {
        *color = result;
    }
    return true;
}
std::string ColorUtils::colorToHex(unsigned int color) {
    std::ostringstream oss;
    oss << "#" << std::hex << std::uppercase;
    oss.width(6);
    oss.fill('0');
    oss << (color & 0xFFFFFF);
    return oss.str();
}
void ColorUtils::colorToRGB(unsigned int color, int* r, int* g, int* b) {
    if (r) *r = (color >> 16) & 0xFF;
    if (g) *g = (color >> 8) & 0xFF;
    if (b) *b = color & 0xFF;
}
unsigned int ColorUtils::rgbToColor(int r, int g, int b) {
    return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}
bool ColorUtils::isValidHexColor(const std::string& hex) {
    return parseHexColor(hex, NULL);
}
std::string FileUtils::readFile(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo: " + filename);
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}
bool FileUtils::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    return file.good();
}
bool FileUtils::fileExists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return file.good();
}
std::string FileUtils::getExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == filename.length() - 1) {
        return "";
    }
    return filename.substr(dotPos + 1);
}
std::string FileUtils::getBasename(const std::string& filename) {
    size_t sepPos = filename.find_last_of("/\\");
    if (sepPos == std::string::npos) {
        return filename;
    }
    return filename.substr(sepPos + 1);
}
void Debug::setEnabled(bool enable) {
    enabled_ = enable;
}
bool Debug::isEnabled() {
    return enabled_;
}
void Debug::print(const std::string& message) {
    if (enabled_) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}
void Debug::printf(const char* format, ...) {
    if (!enabled_) return;
    va_list args;
    va_start(args, format);
    std::printf("[DEBUG] ");
    std::vprintf(format, args);
    std::printf("\n");
    va_end(args);
}
} 
