/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clegros <clegros@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:10:05 by clegros           #+#    #+#             */
/*   Updated: 2024/11/07 14:10:08 by clegros          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTHANDLER_HPP
# define HTTPREQUESTHANDLER_HPP
# include "HttpResponseHandler.hpp"
# include <sys/socket.h> // recv | send
# include <iostream>
# include <unistd.h> // close
# include <map>
# include <sstream>

class HttpResponseHandler;

class HttpRequestHandler
{
	public:
		std::string	getMethod(void) const;
		std::string	getPath(void) const;
		std::string	getHttpVersion(void) const;
		std::string	getHeader(const std::string &headerName) const;
		std::map<std::string, std::string> getHeaders() const;
		std::string	getBody() const;

		void		setMethod(const std::string &m);
		void		setPath(const std::string &p);
		void		setHttpVersion(const std::string &h);
		void		setHeader(const std::string &headerName, const std::string &headerValue);
		void		setBody(const std::string &body);

		static void handle_request(int client_sock);
		HttpRequestHandler	httpParsing(const std::string &buffer);
		//HttpRequestHandler&	operator=(const HttpRequestHandler &assign);
	private:
		std::string	method;
		std::string	path;
		std::string	httpVersion;
		std::map<std::string, std::string> headers;
		std::string body;
};
std::ostream	&operator<<(std::ostream &out, const HttpRequestHandler &i);

#endif
