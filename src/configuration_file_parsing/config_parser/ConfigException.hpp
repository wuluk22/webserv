#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

#include <iostream>
#include "../../ErrorHandler.hpp"

// COMMON MACROS
#define ERROR_HEADER "\e[91m[CONFIG_PARSER | ERROR] : "
#define AB " aborting."

#ifndef RESET
    #define RESET "\e[0m"
#endif

// PATH RELATED ERROR MESSAGES
#define BAD_ACCESS ERROR_HEADER "No such file present," AB RESET
#define BAD_CONFIG_FILE ERROR_HEADER "Config file isn't readable," AB RESET
#define NO_SERVER_CONFIG ERROR_HEADER "No such server configuration," AB RESET
#define PATH_NOT_RECOGNIZED ERROR_HEADER "Path isn't recognized," AB RESET
#define PATH_ALREADY_DEFINED ERROR_HEADER "Path is already defined," AB RESET
#define ABSOULTE_ROOT ERROR_HEADER "Root path should be absolute," AB RESET
#define RELATIVE_ERROR_NO_ROOT ERROR_HEADER "Error pages lacks root in the server level to base its URI," AB RESET
#define URI_STYLE_FORMAT_ERROR ERROR_HEADER "Error pages uses either an absolute path, or an URI based on the existing root," AB RESET

// CONFIG PARSER RULES ERROR MESSAGES
#define NOT_VALID_SERVER_NAME ERROR_HEADER "Invalid server name," AB RESET
#define INVALID_TOKEN ERROR_HEADER  "Invalid token in the configuration file," AB RESET
#define TOKEN_REPEATED ERROR_HEADER "Non-repeatable token repeated within block," AB RESET
#define BAD_INSTRUCTION ERROR_HEADER "Bad instruction within a directive," AB RESET
#define TOKEN_POSITION_MISMATCH ERROR_HEADER "Token is not within any block," AB RESET
#define NO_INSTRUCTION ERROR_HEADER "No instruction given in the configuration file," AB RESET
#define DUPE_ELEMS ERROR_HEADER "Duplicate elements discovered," AB RESET
#define NO_ELEMENTS ERROR_HEADER "Lack of proper arguments," AB RESET
#define DOUBLE_DIRECTIVE ERROR_HEADER "Double server directive," AB RESET
#define ROOT_PRIORITY ERROR_HEADER "Root redefinition should be declared before inbricked location," AB RESET
#define NO_ROOT_DEFINITION ERROR_HEADER "No root definition, vacant URI prefix," AB RESET
#define UNDEFINED_PARAMS ERROR_HEADER "Undefined parameter," AB RESET
#define NUMERICAL_VALUE_EXPECTED ERROR_HEADER "Numerical value expected," AB RESET
#define AMBIGUOUS_URI_DEFINITION ERROR_HEADER "Root and alias cannot be declared inside the same location block," AB RESET
#define DOUBLE_LOCATION_URI ERROR_HEADER "Repeated URI," AB RESET

// SERVER HEADER ERROR MESSAGES
#define EXCEEDING_LIMIT ERROR_HEADER "Size limit exceeded," AB RESET
#define PORT_SCOPE_LINUX ERROR_HEADER "Port scope is limited to 1024 - 65.535 for linux," AB RESET
#define PORT_SCOPE_GENERAL ERROR_HEADER "Port scope is limited to 0 - 65.535 for your system," AB RESET 
#define WRONG_ERROR_PAGES_SCOPE ERROR_HEADER "Wrong error pages scope," AB RESET

// LOCATION RELATED ERROR MESSAGES
#define NO_URI_LOCATION ERROR_HEADER "Location token contain no URI," AB RESET
#define BAD_URI ERROR_HEADER "Bad URI," AB RESET

// WARNING HEADER
#define WARNING_HEADER "\e[33m[CONFIG_PARSER | WARNING] : "
// WARNING MESSAGES
#define NO_ROOT_DEFINED_ERROR_PAGES WARNING_HEADER "No root defined for error pages, switching to absolute path" RESET

#endif