
#ifndef CGI_HPP
# define CGI_HPP

std::string         getQuery(std::string path);
std::vector<char *> homeMadeSetEnv(HttpRequestHandler request, std::string scriptPath);
void                handleCGI(HttpRequestHandler request, std::string scriptPath);

#endif
