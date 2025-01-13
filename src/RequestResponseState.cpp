#include "RequestResponseState.hpp"
#include "HttpRequestHandler.hpp"

RRState::RRState() {}

RRState::~RRState() {}

// GETTER // 
HttpRequestHandler&     		RRState::getRequest() { return this->_request; }
HttpResponseHandler&			RRState::getResponse() { return this->_response; }
ServerHandler&					RRState::getServer() { return this->_server; }
int&							RRState::getClientSock() { return this->_clientSock; }

// SETTER //
void                RRState::setRequest(HttpRequestHandler& req) { this->_request = req; }
void                RRState::setResponse(HttpResponseHandler& res) { this->_response = res; }
void				RRState::setServer(const ServerHandler& server) { this->_server = server; }
void				RRState::setClientSock(const int& clientSock) { this->_clientSock = clientSock; }