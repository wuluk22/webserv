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

HttpRequestHandler	RRState::initRequest(const HttpRequestHandler& request)
{
		HttpRequestHandler			tempRequest(request);
		std::vector<std::string>	allowedMethods;
		std::string					root;
		std::vector<std::string>	allowedPaths;

		allowedMethods.push_back("GET");
		allowedMethods.push_back("POST");
		allowedMethods.push_back("DELETE");
		root = request.getPath();
		allowedPaths.push_back("/static");
		allowedPaths.push_back("/index.html");
		tempRequest.setAllowedMethods(allowedMethods);
		tempRequest.setRootDirectory(root);
		tempRequest.setAllowedPaths(allowedPaths);
		return tempRequest;	
}



