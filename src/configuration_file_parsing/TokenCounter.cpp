#include "ConfigParser.hpp"

TokenCounter::TokenCounter() {
	blockStack.push(std::map<std::string, int>());
}

void TokenCounter::enterBlock() {
	blockStack.push(std::map<std::string, int>());
}

void TokenCounter::exitBlock() {
	if (blockStack.size() > 1) {
		blockStack.pop();
	} else {
		std::cerr << "Error: Attempted to exit global block!" << std::endl;
	}
}

void TokenCounter::incrementToken(const std::string& token) {
	if (!blockStack.empty()) {
		blockStack.top()[token]++;
	}
}

int TokenCounter::getTokenCount(const std::string& token) const {
	if (!blockStack.empty()) {
		std::map<std::string, int>::const_iterator it = blockStack.top().find(token);
		if (it != blockStack.top().end()) {
			return it->second;
		}
	}
	return 0;
}