/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nechaara <nechaara@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:10:05 by clegros           #+#    #+#             */
/*   Updated: 2025/01/11 01:41:20 by nechaara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTHANDLER_HPP
# define HTTPREQUESTHANDLER_HPP
# include "DirectoryHandler.hpp"
# include "configuration_file_parsing/server_config/ServerConfig.hpp"

# include <sys/socket.h>
# include <iostream>
# include <unistd.h>
# include <map>
# include <vector>
# include <sstream>
# include <string>
# include <fstream>
# include <sys/stat.h>
# include <cstring>
# include <algorithm>

class RRState;
class HttpResponseHandler;

class HttpRequestHandler
{
	private:
		int																			fd;
		int																			_clientSocket;
		std::string																	method;
		std::string																	path;
		std::string																	httpVersion;
		std::map<std::string, std::string>											headers;
		std::string																	body;
		bool																		valid;

		std::vector<std::string>													allowedMethods;
		std::vector<std::string>													allowedPaths;
		std::string																	allowedPath;
		std::string																	contentPath;
		unsigned int																maxBodySize;
		bool																		autoIndex;
		std::string																	rootDirectory;
		std::vector<std::string>													_CgiPath;
		LocationBlock																_locationBlocks;
		
		std::map<std::string, std::map<std::string, std::vector<std::string> > >	_locInfo;
		bool																		isRequestComplete;
		static std::string															extractBoundary(const std::string& contentType);
		DirectoryHandler															_handler;
		std::map<std::string, std::string>											_cookies;
	public:
		HttpRequestHandler();
		~HttpRequestHandler();

		std::string																		trim(const std::string& str);
		bool																			endsWith(const std::string& str, const std::string& suffix);
		std::string																		getMimeType(const std::string& path);
		std::string																		createErrorPage(int statusCode, const std::string& message);
		std::string																		toString(size_t value);
		static std::string																readFile(RRState& rrstate, const std::string& path);
		bool																			isMethodAllowed(const HttpRequestHandler& request, const std::string& method) const;

		void																			setMethod(const std::string &m);
		void																			setPath(const std::string &p);
		void																			setHttpVersion(const std::string &h);
		void																			setHeader(const std::string &headerName, const std::string &headerValue);
		void																			setBody(const std::string &body);
		void																			setFd(const int &nb);
		void																			setIsComplete(const bool& is);
		void																			setAllowedMethods(const std::vector<std::string>& methods);
		void																			setAllowedPaths(const std::vector<std::string>& paths);
		void																			setRootDirectory(const std::string& path);
		void																			setCgiPath(const std::vector<std::string>& cgiPath);
		void																			setClientSocket(const int& clientSock);

		std::string																		getMethod(void) const;
		std::string																		getPath(void) const;
		std::string																		getHttpVersion(void) const;
		std::string																		getHeader(const std::string &headerName) const;
		const std::map<std::string, std::string>&										getHeaders() const;
		std::string																		getBody() const;
		int																				getFd() const;
		bool																			getIsComplete() const;
		std::string																		getRootDirectory() const;
		const std::vector<std::string>&													getAllowedMethods() const;
		const std::vector<std::string>&													getAllowedPaths() const;
		const std::vector<std::string>&													getCgiPath() const;
		const int&																		getClientSocket() const;

// --------------------------------									

		void																			setLocInfo(const std::map<std::string, std::map<std::string, std::vector<std::string> > >& locInfo);
		void																			setIsValid(const bool& is);
		void																			setAllowedPath(const std::string& paths);
		void																			setAutoIndex(const bool& index);
		void																			setMaxBody(const unsigned int& max);

		bool																			getAutoIndex() const;
		const unsigned int&																getMaxBody() const;
		const std::string&																getAllowedPath() const;
		const std::map<std::string, std::map<std::string, std::vector<std::string> > >&	getLocInfo()const;
		std::map<std::string, std::vector<std::string> >								getLocInfoByUri(HttpRequestHandler request);
		bool																			getIsValid() const;

		std::vector<std::string>														getContentPathsFromLoc(RRState& rrstate, const std::string& uri) const;
		std::string																		getRootDirectoryFromLoc(RRState& rrstate, const std::string& uri) const;
		bool																			isAutoIndexEnabled(RRState& rrstate, const std::string& uri) const;
		std::string																		getFullPathFromLoc(RRState& rrstate, const std::string& relativePath) const;
		unsigned int																	getMaxBodyFromLoc(RRState& rrstate, const std::string& uri) const;
		void																			setContentPath(const std::string& path);
		std::string																		getContPath();
		std::string 																	getContentPath(const std::map<std::string, std::vector<std::string> >& config);

		bool																			isAutoIndexEnabledForUri(RRState& rrstate, const std::string& uri) const;
		void																			reset();
		LocationBlock*																	getLocationBlock(RRState& rrstate, std::vector<LocationBlock*> locationBlocks) const;

// --------------------------------									

		HttpRequestHandler																handleRequest(int clientSock, RRState& rrstate);
		static HttpRequestHandler														httpParsing(const std::string &buffer);
		void																			handleDirectoryRequest(RRState& rrstate, const std::string& path, HttpResponseHandler& response);
    	void																			handleFileUpload(RRState& rrstate, const std::string& requestData, const std::string& path, HttpResponseHandler& response);
		HttpRequestHandler																handleConfig(HttpRequestHandler& request, std::vector<LocationBlock *> locationsBlock);
		std::string																		extractDir(std::string& requestPath);
		std::string																		removeExcessiveSlashes(std::string& path);

		void																			parseCookies(const std::string& cookieHeader);
		const std::map<std::string, std::string>&										getCookies() const;
		std::string																		trimCookies(const std::string& str);

};
std::ostream	&operator<<(std::ostream &out, const HttpRequestHandler &i);

#endif