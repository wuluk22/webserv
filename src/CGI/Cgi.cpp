#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <map>
#include <fstream>

# include "../Logger.hpp"
# include "../HttpRequestHandler.hpp"
# include "../HttpResponseHandler.hpp"
# include "../ErrorHandler.hpp"

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

    std::clog << "REQUEST GET METHOD : " << request.getMethod() << std::endl;

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
        envp.push_back(strdup(stringEnv[i].c_str()));
    }
    envp.push_back(NULL);
    return envp;
}

std::string readDatasFromScript(int pipefd)
{
    std::ostringstream output;
    std::string outputStr;
    char buffer[4096];
    ssize_t bytesRead;

    while((bytesRead = read(pipefd, buffer, sizeof(buffer))) > 0)
    {
        output.write(buffer, bytesRead);
    }
    close(pipefd);
    outputStr = output.str();
    return outputStr;
}

void    HttpResponseHandler::handleCgiResponse(std::string output, HttpResponseHandler& response)
{
        std::string::size_type headerEnd = output.find("\r\n\r\n");
        if (headerEnd != std::string::npos) 
        {
            std::string headers = output.substr(0, headerEnd);
            std::cout << "HEADERS : " << headers << std::endl;
            std::string body = output.substr(headerEnd + 4);

            // Analyser et ajouter les en-têtes
            std::istringstream headerStream(headers);
            std::string headerLine;
            while (std::getline(headerStream, headerLine))
            {
                std::string::size_type colonPos = headerLine.find(": ");
                if (colonPos != std::string::npos) {
                    std::string headerName = headerLine.substr(0, colonPos);
                    std::string headerValue = headerLine.substr(colonPos + 2);

                    // Supprimer un "\r" final si présent
                    if (!headerValue.empty() && headerValue[headerValue.length() - 1] == '\r') {
                        headerValue = headerValue.substr(0, headerValue.length() - 1);
                    }
                    response.setHeader(headerName, headerValue);
                }
            }
            // Ajouter le corps de la réponse
            response.setStatusCode(200);
            response.setStatusMsg("OK");
            response.setBody(body);
        } 
        else
        {
            // Si aucun en-tête n'est trouvé, tout est traité comme le corps
            response.setStatusCode(200);
            response.setStatusMsg("OK");
            response.setBody(output);
        }
}

void    handleCGI(HttpRequestHandler& request, HttpResponseHandler& response)
{
    int pid;
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        setErrorResponse(request, response, 500, "Internal Server Error - Pipe creation failed");
        return ;
    }

    std::string cgiPath = "/usr/bin/python3";
    std::vector<std::string> scriptPaths = request.getCgiPath();
    std::string selectedScriptPath;
    for (std::vector<std::string>::iterator it = scriptPaths.begin(); it != scriptPaths.end(); it++)
    {
        std::cout << "path : " << it->c_str() << "\n";
        if (access(it->c_str(), X_OK) == 0) {
            selectedScriptPath = *it;
            std::cout << "selectedScriptPath : " << selectedScriptPath << std::endl;
            break ;
        }
    }
    pid = fork();
    if (pid < 0)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        setErrorResponse(request, response, 500, "Internal Server Error - Fork creation failed");
        return;
    }
    else if (pid == 0)
    { // Processus enfant
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        std::vector<char*> argv;
        std::vector<char *> envp = homeMadeSetEnv(request, selectedScriptPath);

        argv.push_back(strdup(cgiPath.c_str()));
        argv.push_back(strdup(selectedScriptPath.c_str()));
        argv.push_back(NULL);
        // for (std::vector<char *>::iterator it = argv.begin(); it != argv.end(); it++) {
        //     std::clog << "ARGV : " << *it << std::endl;
        // }
        // for (std::vector<char *>::iterator it = envp.begin(); it != envp.end(); it++) {
        //     std::clog << "ENVP : " << *it << std::endl;
        // }
        if (access(cgiPath.c_str(), X_OK) == 0)
        {
            execve(cgiPath.c_str(), argv.data(), envp.data());
            perror("execve failed");
            exit(1);
        }
    } 
    else 
    { // Processus parent
        std::string output;
        int status;
        close(pipefd[1]);
        output = readDatasFromScript(pipefd[0]);

        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            response.handleCgiResponse(output, response);
        }
        else
            Logger::log("CGI script exited with code: " + toStrInt(WEXITSTATUS(status)));
    }
}

