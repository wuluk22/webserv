/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseHandler.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salowie <salowie@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 13:07:27 by clegros           #+#    #+#             */
/*   Updated: 2024/11/19 11:25:39 by salowie          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSEHANDLER_HPP
# define HTTPRESPONSEHANDLER_HPP

# include <sys/socket.h>
# include <iostream>
# include <sstream>
# include <map>
# include "HttpRequestHandler.hpp"

class HttpRequestHandler;

class HttpResponseHandler
{
	public:
		void		setHttpVersion(std::string version);
		void		setStatusCode(int code);
		void		setStatusMsg(std::string message);
		void		setHeader(const std::string &headerName, const std::string &headerValue);
		void		setBody(std::string body);
		
		std::string	getHttpVersion() const;
		int			getStatusCode() const;
		std::string getStatusMsg() const;
		std::string	getHeader(const std::string &headerName) const;
		std::map<std::string, std::string> getHeaders() const;
		std::string	getBody() const;
		std::string	getAll() const;
		HttpResponseHandler	handlePath(HttpRequestHandler &request, HttpResponseHandler &response);
	private:
		std::string	httpVersion;
		int			code;
		std::string	status;
		std::map<std::string, std::string> headers;
		std::string	body;
};

std::ostream	&operator<<(std::ostream &out, const HttpResponseHandler &i);

#endif
