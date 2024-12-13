# include "Cgi.hpp"
# include "../Logger.hpp"
# include "../HttpRequestHandler.hpp"
# include "../HttpResponseHandler.hpp"
# include "../RequestResponseState.hpp"
# include "../ErrorHandler.hpp"
# include "../ServerHandler.hpp"

Cgi::Cgi() {}
Cgi::~Cgi() {}

std::string Cgi::getClientIP(RRState& rrstate) {
    if (getsockname(rrstate.getServer().getSock(), rrstate.getServer().getAddress(), &rrstate.getServer().getAddrlen()) == -1) {
        perror("getsockname");
        return "0.0.0.0";
    }
    // Convertir l'adresse IP en format lisible
    return inet_ntoa(rrstate.getServer().getAdd().sin_addr);
}

int Cgi::getClientPort(RRState& rrstate) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    if (getsockname(rrstate.getServer().getSock(), rrstate.getServer().getAddress(), &rrstate.getServer().getAddrlen()) == -1) {
        perror("getsockname");
        return 0;
    }

    return ntohs(rrstate.getServer().getAdd().sin_port);
}

std::string         Cgi::getQuery(std::string path)
{
    std::string query;
    size_t pos = 0;
    pos = path.find("?");
    if (pos != std::string::npos)
        query = path.substr(pos + 1);
    else
        query = "";
    return query;
}

std::string         Cgi::findAccept(std::map<std::string, std::string> headers)
{
    std::string result;
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
    {
        if (it->first == "Accept")
            result =  it->second;
    }
    return result;
}

std::vector<char *> Cgi::homeMadeSetEnv(RRState& rrstate, std::string scriptPath)
{
    std::vector<std::string> stringEnv;
    std::vector<char *> envp;

    stringEnv.push_back("REQUEST_METHOD=" + rrstate.getRequest().getMethod());
    stringEnv.push_back("SCRIPT_NAME=" + scriptPath);
    if (rrstate.getRequest().getMethod() == "GET")
        stringEnv.push_back("QUERY_STRING=" + getQuery(rrstate.getRequest().getPath()));
    if (rrstate.getRequest().getMethod() == "POST") {
        stringEnv.push_back("CONTENT_TYPE=" + findAccept(rrstate.getRequest().getHeaders()));
        stringEnv.push_back("CONTENT_LENGTH=" + rrstate.getRequest().getHeader("CONTENT_LENGTH"));
    }
    stringEnv.push_back("SERVER_PROTOCOL=" + rrstate.getRequest().getHttpVersion());
    stringEnv.push_back("SERVER_SOFTWARE=WebServ/1.0");
    stringEnv.push_back("SERVER_NAME=localhost"); // not the final one
    stringEnv.push_back("REMOTE_ADDR=" + getClientIP(rrstate));
    stringEnv.push_back("REMOTE_PORT=" + toStrInt(getClientPort(rrstate)));
    // stringEnv.push_back("SERVER_NAME=" + request.getServerName());
    for (size_t i = 0; i < stringEnv.size(); i++) {
        envp.push_back(strdup(stringEnv[i].c_str()));
    }
    envp.push_back(NULL);
    return envp;
}

std::string Cgi::readDatasFromScript(int pipefd)
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
            std::string body = output.substr(headerEnd + 4);
            
            std::clog << "HEADERS : " << headers << std::endl;
            std::clog << "BODY : " << body << std::endl;

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

void    Cgi::handleCGI(RRState& rrstate)
{
    int pid;
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        setErrorResponse(rrstate, 500, "Internal Server Error - Pipe creation failed");
        return ;
    }

    std::string cgiPath = "/usr/bin/python3";
    std::vector<std::string> scriptPaths = rrstate.getRequest().getCgiPath();
    std::string selectedScriptPath;
    for (std::vector<std::string>::iterator it = scriptPaths.begin(); it != scriptPaths.end(); it++)
    {
        std::cout << "path : " << it->c_str() << "\n";
        if (access(it->c_str(), X_OK) == 0) {
            selectedScriptPath = *it;
            std::clog << "selectedScriptPath : " << selectedScriptPath << std::endl;
            break ;
        }
    }
    pid = fork();
    if (pid < 0)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        setErrorResponse(rrstate, 500, "Internal Server Error - Fork creation failed");
        return;
    }
    else if (pid == 0)
    { // Processus enfant
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        std::vector<char*> argv;
        std::vector<char *> envp = homeMadeSetEnv(rrstate, selectedScriptPath);

        argv.push_back(strdup(cgiPath.c_str()));
        argv.push_back(strdup(selectedScriptPath.c_str()));
        argv.push_back(NULL);
        // for (std::vector<char *>::iterator it = argv.begin(); it != argv.end(); it++) {
        //     std::clog << "ARGV : " << *it << std::endl;
        // }
        // for (std::vector<char *>::iterator it = envp.begin(); it != envp.end(); it++) {
        //     std::clog << "ENVP : " << *it << std::endl;
        // }
        std::clog << "QUERY : " << getQuery(rrstate.getRequest().getPath()) << std::endl;
        if (access(cgiPath.c_str(), X_OK) == 0) //  && getQuery(request.getPath()) != ""
        {
            std::clog << "ICI" << std::endl;
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
            rrstate.getResponse().handleCgiResponse(output, rrstate.getResponse());
        }
        // else
        //     Logger::log("CGI script exited with code: " + toStrInt(WEXITSTATUS(status)));
    }
}

