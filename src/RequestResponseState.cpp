#include "RequestResponseState.hpp"

RRState::RRState() {}

RRState::~RRState() {}

HttpRequestHandler&     RRState::getRequest() { return _request; }
HttpResponseHandler&	RRState::getResponse() { return _response; }

void                RRState::setRequest(HttpRequestHandler& req) { this->_request = req; }
void                RRState::setResponse(HttpResponseHandler& res) { this->_response = res; }
