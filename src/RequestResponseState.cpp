#include "RequestResponseState.hpp"

RRState::RRState() : requestBuffer(""), requestComplete(false), responseBuffer(""), bytesSent(0) {}

RRState::~RRState() {}