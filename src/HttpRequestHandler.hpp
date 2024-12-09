/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clegros <clegros@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:10:05 by clegros           #+#    #+#             */
/*   Updated: 2024/11/19 15:30:13 by clegros          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTHANDLER_HPP
# define HTTPREQUESTHANDLER_HPP
# include "HttpResponseHandler.hpp"
# include "DirectoryHandler.hpp"
# include "configuration_file_parsing/ServerConfig.hpp"
# include <sys/socket.h> // recv | send
# include <iostream>
# include <unistd.h> // close
# include <map>
# include <vector>
# include <sstream>
# include <string>
# include <fstream>
# include <sys/stat.h>
# include <cstring>
# include <algorithm>

class HttpResponseHandler;

class HttpRequestHandler
{
	private:
		int											fd;
		std::string									method;
		std::string									path;
		std::string									httpVersion;
		std::map<std::string, std::string>			headers;
		std::string									body;
		bool										isRequestComplete;
		static std::string							extractBoundary(const std::string& content_type);
    	static std::string							generateErrorResponse(const std::string& message);
	public:
		HttpRequestHandler();
		~HttpRequestHandler();

		std::string									trim(const std::string& str);
		bool										endsWith(const std::string& str, const std::string& suffix);
		std::string									getMimeType(const std::string& path);
		std::string									createErrorPage(int statusCode, const std::string& message);
		std::string									toString(size_t value);
		static std::string							readFile(const std::string& path);
		static bool									fileExists(const std::string& path);
		bool										isMethodAllowed(const HttpRequestHandler& request, const std::string& method) const;
		bool										isPathAllowed(const HttpRequestHandler& request, const std::string& path) const;

		void										setMethod(const std::string &m);
		void										setPath(const std::string &p);
		void										setHttpVersion(const std::string &h);
		void										setHeader(const std::string &headerName, const std::string &headerValue);
		void										setBody(const std::string &body);
		void										setFd(const int &nb);
		void										setIsComplete(const bool& is);

		std::string									getMethod(void) const;
		std::string									getPath(void) const;
		std::string									getHttpVersion(void) const;
		std::string									getHeader(const std::string &headerName) const;
		const std::map<std::string, std::string>&	getHeaders() const;
		std::string									getBody() const;
		int											getFd() const;
		bool										getIsComplete() const;

		HttpRequestHandler							handleRequest(int client_sock);
		static HttpRequestHandler					httpParsing(const std::string &buffer);
		//HttpResponseHandler							handlePath(const HttpRequestHandler &request, HttpResponseHandler &response);
		void										handleDirectoryRequest(const std::string& path, HttpResponseHandler& response);
    	void										handleFileUpload(const std::string& requestData, const std::string& path, HttpResponseHandler& response);
		HttpRequestHandler							handleConfig(HttpRequestHandler& request, std::vector<LocationBlock *> locationsBlock);
		HttpRequestHandler							initRequest(const HttpRequestHandler& request);
};
std::ostream	&operator<<(std::ostream &out, const HttpRequestHandler &i);

#endif