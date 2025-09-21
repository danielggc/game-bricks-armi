#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "symbols.hpp"
#include "error.hpp"
#include "util.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
using namespace brick;
using namespace std;
const char* VERSION = "1.0.0";
const char* BUILD_DATE = __DATE__ " " __TIME__;
struct Options {
    string inputFile;
    bool showHelp;
    bool showVersion;
    bool verbose;
    bool jsonOutput;
    bool debugMode;
    Options() : showHelp(false), showVersion(false), verbose(false), 
                jsonOutput(false), debugMode(false) {}
};
void printUsage(const char* programName) {
    cout << "BrickC - Compilador para BrickLang v" << VERSION << "\n";
    cout << "Motor de Juegos de Ladrillos - Entrega 1\n\n";
    cout << "Uso: " << programName << " [opciones] <archivo.brick>\n\n";
    cout << "Opciones:\n";
    cout << "  -h, --help      Mostrar esta ayuda\n";
    cout << "  -v, --version   Mostrar información de versión\n";
    cout << "  --verbose       Mostrar información detallada del análisis\n";
    cout << "  --json          Generar salida en formato JSON\n";
    cout << "  --debug         Mostrar información de debug\n\n";
    cout << "Ejemplos:\n";
    cout << "  " << programName << " examples/snake.brick\n";
    cout << "  " << programName << " --verbose examples/tetris.brick\n";
    cout << "  " << programName << " --json examples/snake.brick > output.json\n\n";
    cout << "Archivos de prueba incluidos:\n";
    cout << "  examples/snake.brick    - Juego Snake completo\n";
    cout << "  examples/tetris.brick   - Juego Tetris completo\n";
    cout << "  tests/lexer_*.brick     - Pruebas del analizador léxico\n";
    cout << "  tests/parser_*.brick    - Pruebas del analizador sintáctico\n";
    cout << "  tests/invalid_*.brick   - Casos de error para validación\n";
}
void printVersion() {
    cout << "BrickC versión " << VERSION << "\n";
    cout << "Compilado: " << BUILD_DATE << "\n";
    cout << "Compatible con Windows XP / AMD Athlon XP\n";
    cout << "Lenguaje: C++98/C++03\n";
}
Options parseCommandLine(int argc, char* argv[]) {
    Options opts;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            opts.showHelp = true;
        } else if (arg == "-v" || arg == "--version") {
            opts.showVersion = true;
        } else if (arg == "--verbose") {
            opts.verbose = true;
        } else if (arg == "--json") {
            opts.jsonOutput = true;
        } else if (arg == "--debug") {
            opts.debugMode = true;
        } else if (arg[0] == '-') {
            cerr << "Opción desconocida: " << arg << "\n";
            cerr << "Use -h o --help para ver opciones disponibles.\n";
            exit(1);
        } else {
            if (opts.inputFile.empty()) {
                opts.inputFile = arg;
            } else {
                cerr << "Solo se puede especificar un archivo de entrada.\n";
                exit(1);
            }
        }
    }
    return opts;
}
int main(int argc, char* argv[]) {
    Options opts = parseCommandLine(argc, argv);
    if (opts.showHelp) {
        printUsage(argv[0]);
        return 0;
    }
    if (opts.showVersion) {
        printVersion();
        return 0;
    }
    if (opts.inputFile.empty()) {
        cerr << "Error: No se especificó archivo de entrada.\n";
        cerr << "Use -h o --help para ver instrucciones de uso.\n";
        return 1;
    }
    Debug::setEnabled(opts.debugMode);
    if (opts.verbose) {
        cout << "BrickC v" << VERSION << " - Analizando: " << opts.inputFile << "\n";
        cout << "==========================================\n";
    }
    try {
        string sourceCode = FileUtils::readFile(opts.inputFile);
        if (opts.verbose) {
            cout << "Archivo leído exitosamente (" << sourceCode.length() << " caracteres)\n";
        }
        ErrorHandler errorHandler;
        errorHandler.setCurrentFile(opts.inputFile);
        if (opts.verbose) {
            cout << "\n1. Análisis Léxico...\n";
        }
        Lexer lexer(sourceCode);
        if (opts.verbose) {
            cout << "2. Análisis Sintáctico...\n";
        }
        Parser parser(&lexer, &errorHandler);
        ASTNode* ast = parser.parseProgram();
        if (errorHandler.hasErrors()) {
            cerr << "\nERRORES ENCONTRADOS:\n";
            cerr << "====================\n";
            errorHandler.printAll(cerr);
            if (ast) {
                deleteAST(ast);
            }
            cerr << "\nEl análisis falló con " << errorHandler.getErrorCount() 
                 << " error(es).\n";
            return 1;
        }
        if (!ast) {
            cerr << "Error: No se pudo construir el árbol sintáctico.\n";
            return 1;
        }
        if (opts.verbose) {
            cout << "3. Construcción de Tabla de Símbolos...\n";
        }
        SymbolTable symbolTable(&errorHandler);
        bool success = symbolTable.populateFromAST(ast);
        if (!success || errorHandler.hasErrors()) {
            cerr << "\nERRORES SEMÁNTICOS:\n";
            cerr << "===================\n";
            errorHandler.printAll(cerr);
            deleteAST(ast);
            cerr << "\nEl análisis semántico falló con " 
                 << errorHandler.getErrorCount() << " error(es).\n";
            return 1;
        }
        if (opts.verbose) {
            cout << "4. Validación Semántica...\n";
        }
        bool valid = symbolTable.validate();
        if (!valid || errorHandler.hasErrors()) {
            cerr << "\nERRORES DE VALIDACIÓN:\n";
            cerr << "======================\n";
            errorHandler.printAll(cerr);
            deleteAST(ast);
            cerr << "\nLa validación falló con " 
                 << errorHandler.getErrorCount() << " error(es).\n";
            return 1;
        }
        if (opts.verbose) {
            cout << "5. Generando Resultados...\n\n";
        }
        cout << "OK: juego '" << symbolTable.getGameName() 
             << "' analizado correctamente\n";
        if (opts.jsonOutput) {
            if (opts.verbose) {
                cout << "\nTABLA DE SÍMBOLOS (JSON):\n";
                cout << "=========================\n";
            }
            symbolTable.printJSON(cout);
        } else {
            if (opts.verbose) {
                cout << "\nTABLA DE SÍMBOLOS:\n";
                cout << "==================\n";
            }
            symbolTable.print(cout);
        }
        if (opts.verbose) {
            cout << "\nRESUMEN:\n";
            cout << "========\n";
            symbolTable.printSummary(cout);
            cout << "\nESTADÍSTICAS:\n";
            cout << "=============\n";
            cout << "Entidades definidas: " << symbolTable.getEntityCount() << "\n";
            cout << "Controles mapeados: " << symbolTable.getControlCount() << "\n";
            cout << "Reglas definidas: " << symbolTable.getRuleCount() << "\n";
        }
        if (opts.debugMode) {
            cout << "\n\nÁRBOL SINTÁCTICO (DEBUG):\n";
            cout << "==========================\n";
            printAST(ast);
        }
        deleteAST(ast);
        if (opts.verbose) {
            cout << "\n✓ Análisis completado exitosamente.\n";
        }
        return 0;
    } catch (const std::exception& e) {
        cerr << "Error inesperado: " << e.what() << "\n";
        return 1;
    } catch (...) {
        cerr << "Error inesperado desconocido.\n";
        return 1;
    }
}
