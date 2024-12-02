#include "RequestResponseState.hpp"

RRState::RRState() {}

RRState::~RRState() {}

HttpRequestHandler&     		RRState::getRequest() { return _request; }
HttpResponseHandler&			RRState::getResponse() { return _response; }
std::vector<LocationBlock *>&	RRState::getLocations() { return _locations; }

void                RRState::setRequest(HttpRequestHandler& req) { this->_request = req; }
void                RRState::setResponse(HttpResponseHandler& res) { this->_response = res; }
void				RRState::setLocations(std::vector<LocationBlock *>& locations) { this->_locations = locations; }

HttpRequestHandler		RRState::initRequest(const HttpRequestHandler& request)
{
		HttpRequestHandler			tempRequest(request);
		std::vector<std::string>	allowedMethods;
		std::string					root;
		std::vector<std::string>	allowedPaths;

		allowedMethods.push_back("GET");
		allowedMethods.push_back("POST");
		allowedMethods.push_back("DELETE");
		root = "public";
		allowedPaths.push_back("/static");
		allowedPaths.push_back("/index.html");
		tempRequest.setAllowedMethods(allowedMethods);
		tempRequest.setRootDirectory(root);
		tempRequest.setAllowedPaths(allowedPaths);
		return tempRequest;	
}



