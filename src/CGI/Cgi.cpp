#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <map>

# include "../Logger.hpp"
# include "../HttpRequestHandler.hpp"
#include "../ErrorHandler.hpp"

std::string         getQuery(std::string path)
{
    std::string query;
    size_t pos = 0;
    pos = path.find("?");
    if (pos != std::string::npos)
        query = path.substr(pos + 1);
    else
        query = "";
    std::cout << "QUERY : " << query << std::endl;
    return query;
}

std::string         findAccept(std::map<std::string, std::string> headers)
{
    std::string result;
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
    {
        if (it->first == "Accept")
            result =  it->second;
    }
    return result;
}

std::vector<char *> homeMadeSetEnv(HttpRequestHandler request, std::string scriptPath)
{
    std::vector<std::string> stringEnv;
    std::vector<char *> envp;

    stringEnv.push_back("REQUEST_METHOD=" + request.getMethod());
    stringEnv.push_back("SCRIPT_PATH=" + scriptPath);
    if (request.getMethod() == "GET")
        stringEnv.push_back("QUERY_STRING=" + getQuery(request.getPath()));
    if (request.getMethod() == "POST") {
        stringEnv.push_back("CONTENT_TYPE=" + findAccept(request.getHeaders()));
        stringEnv.push_back("CONTENT_LENGTH=" + request.getHeader("CONTENT_LENGTH"));
    }
    stringEnv.push_back("SERVER_PROTOCOL=" + request.getHttpVersion());
    stringEnv.push_back("SERVER_SOFTWARE=WebServ/1.0");
    stringEnv.push_back("SERVER_NAME=localhost"); // not the final one
    // stringEnv.push_back("SERVER_NAME=" + request.getServerName());

    for (size_t i = 0; i < stringEnv.size(); i++) {
        envp.push_back(const_cast<char *>(stringEnv[i].c_str()));
        std::cout << "ENV : " << envp[i] << std::endl;
    }
    envp.push_back(NULL);
    return envp;
}

void handleCGI(HttpRequestHandler& request, HttpResponseHandler& response)
{
    int pid = fork();
    std::string cgiPath = "/usr/bin/python3";
    // std::string staticDir = request.getRootDirectory();
    // std::string scriptPath = staticDir + request.getPath();
    std::vector<std::string> scriptPaths = request.getCgiPath();
    for (std::vector<std::string>::iterator it = scriptPaths.begin(); it != scriptPaths.end(); it++) {
        std::cout << "scriptpath : " << *it << std::endl;
    }
    if (pid < 0) {
        Logger::log("Fork failed!");
        return;
    } else if (pid == 0) { // Processus enfant
        std::vector<char *> envp = homeMadeSetEnv(request, scriptPath);
        // Redirection des entrées/sorties si nécessaire

        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(cgiPath.c_str()));
        argv.push_back(const_cast<char*>(scriptPath.c_str()));
        argv.push_back(NULL);
        if (access(cgiPath.c_str(), X_OK) == 0) {
            execve(cgiPath.c_str(), argv.data(), envp.data());
            std::cout << "ICI" << std::endl;
            perror("execve failed");
            exit(1);
        }
    } else { // Processus parent
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            Logger::log("CGI script exited with code: " + toStrInt(WEXITSTATUS(status)));
        }
    }
}

