#include "RequestResponseState.hpp"
# include "configuration_file_parsing/ConfigParser.hpp"

RRState::RRState() {}

RRState::~RRState() {}

HttpRequestHandler&             RRState::getRequest() { return _request; }
HttpResponseHandler&	        RRState::getResponse() { return _response; }
std::vector<LocationBlock *>&   RRState::getLocations() { return _locations; }

void    RRState::setRequest(HttpRequestHandler& req) { this->_request = req; }
void    RRState::setResponse(HttpResponseHandler& res) { this->_response = res; }
void    RRState::setLocations(std::vector<LocationBlock *>& locations) { this->_locations = locations; }