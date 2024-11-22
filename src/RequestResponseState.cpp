#include "RequestResponseState.hpp"

RRState::RRState() : _requestBuffer(""), _requestComplete(false), _responseBuffer(""), _bytesSent(0) {}

RRState::~RRState() {}

HttpRequestHandler&     RRState::getRequest() { return _request; }
HttpResponseHandler&	RRState::getResponse() { return _response; }
int&                    RRState::getResultRecv() { return _resultRecv; }

void                RRState::setRequest(HttpRequestHandler& req) { this->_request = req; }
void                RRState::setResponse(HttpResponseHandler& res) { this->_response = res; }
void                RRState::setResultRecv(int& resultRecv) {this->_resultRecv = resultRecv; }