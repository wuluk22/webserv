# include "Cgi.hpp"
# include "../Logger.hpp"
# include "../HttpRequestHandler.hpp"
# include "../HttpResponseHandler.hpp"
# include "../RequestResponseState.hpp"
# include "../ErrorHandler.hpp"
# include "../ServerHandler.hpp"

Cgi::Cgi() {}
Cgi::~Cgi() {}

void        ftFree(std::vector<char*> vec)
{
    for (size_t i = 0; i < vec.size(); i++) {
        free(vec[i]);
    }
    vec.clear();
}

std::string Cgi::getClientIP(RRState& rrstate) {
    if (getsockname(rrstate.getServer().getSock(), rrstate.getServer().getAddress(), &rrstate.getServer().getAddrlen()) == -1) {
        perror("getsockname");
        return "0.0.0.0";
    }
    // Convertir l'adresse IP en format lisible
    return inet_ntoa(rrstate.getServer().getAdd().sin_addr);
}

int Cgi::getClientPort(RRState& rrstate) {
    struct sockaddr_in clientAddr;

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

std::vector<char *> Cgi::homeMadeSetEnv(RRState& rrstate, std::string scriptPath, std::string path)
{
    std::vector<std::string> stringEnv;
    std::vector<char *> envp;

    stringEnv.push_back("REQUEST_METHOD=" + rrstate.getRequest().getMethod());
    stringEnv.push_back("SCRIPT_NAME=" + scriptPath);
    stringEnv.push_back("PATH=" + path);
    if (rrstate.getRequest().getMethod() == "GET") {
        stringEnv.push_back("QUERY_STRING=" + getQuery(rrstate.getRequest().getPath()));
        if (!getQuery(rrstate.getRequest().getPath()).empty())
            rrstate.getResponse().setQuery(getQuery(rrstate.getRequest().getPath()));
    }
    if (rrstate.getRequest().getMethod() == "POST") {
        stringEnv.push_back("CONTENT_TYPE=" + findAccept(rrstate.getRequest().getHeaders()));
        stringEnv.push_back("CONTENT_LENGTH=" + rrstate.getRequest().getHeader("CONTENT_LENGTH"));
    }
    stringEnv.push_back("SERVER_PROTOCOL=" + rrstate.getRequest().getHttpVersion());
    stringEnv.push_back("SERVER_SOFTWARE=WebServ/1.0");
    stringEnv.push_back("REMOTE_ADDR=" + getClientIP(rrstate));
    stringEnv.push_back("REMOTE_PORT=" + toStrInt(getClientPort(rrstate)));
    // stringEnv.push_back("SERVER_NAME=localhost"); // not the final one
    stringEnv.push_back("SERVER_NAME=" + rrstate.getServer().getServerName());
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
    char buffer[8192];
    ssize_t bytesRead;

    while((bytesRead = read(pipefd, buffer, sizeof(buffer))) > 0)
    {
        output.write(buffer, bytesRead);
    }
    close(pipefd);
    outputStr = output.str();
    return outputStr;
}

void    Cgi::handleCgiResponse(std::string output, RRState& rrstate)
{
        std::string::size_type headerEnd = output.find("\r\n\r\n");
        if (headerEnd != std::string::npos) 
        {
            std::string headers = output.substr(0, headerEnd);
            std::string body = output.substr(headerEnd + 4);
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
                    rrstate.getResponse().setHeader(headerName, headerValue);
                }
            }
            rrstate.getResponse().setBody(body);
            // CAREFULL
            rrstate.getResponse().setHeader("Content-Type", rrstate.getRequest().getMimeType(".html"));
            rrstate.getResponse().setHeader("Content-Length", rrstate.getRequest().toString(body.length()));
        } 
        else
        {
            rrstate.getResponse().setBody(output);
            // CARREFULL
            rrstate.getResponse().setHeader("Content-Type", rrstate.getRequest().getMimeType(".html"));
            rrstate.getResponse().setHeader("Content-Length", rrstate.getRequest().toString(output.length()));
        }
        rrstate.getResponse().setStatusCode(200);
        rrstate.getResponse().setStatusMsg("OK");
        rrstate.getResponse().setHttpVersion("HTTP/1.1");
        // add connexion keep alive !
        rrstate.getResponse().setHeader("X-Content-Type-Options", "nosniff");
        rrstate.getResponse().setHeader("X-Frame-Options", "SAMEORIGIN");
        rrstate.getResponse().setHeader("X-XSS-Protection", "1; mode=block");
}

void    Cgi::handleCGI(RRState& rrstate, std::string path)
{
    int pid;
    int pipefd[2];
    std::string output;
    if (pipe(pipefd) == -1)
        setErrorResponse(rrstate, 500, "Internal Server Error - Pipe creation failed");

    std::string cgiPath = "/usr/bin/python3";
    std::vector<std::string> scriptPaths = rrstate.getRequest().getCgiPath();
    std::string selectedScriptPath;
    for (std::vector<std::string>::iterator it = scriptPaths.begin(); it != scriptPaths.end(); it++)
    {
        if (access(it->c_str(), X_OK) == 0) {
            selectedScriptPath = *it;
            break ;
        }
    }
    pid = fork();
    if (pid < 0)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        setErrorResponse(rrstate, 500, "Internal Server Error - Fork creation failed");
    }
    else if (pid == 0)
    { // Processus enfant
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        std::vector<char*> argv;
        std::vector<char *> envp = homeMadeSetEnv(rrstate, selectedScriptPath, path);

        argv.push_back(strdup(cgiPath.c_str()));
        argv.push_back(strdup(selectedScriptPath.c_str()));
        argv.push_back(NULL);
        if (access(cgiPath.c_str(), X_OK) == 0)
        {
            
            execve(cgiPath.c_str(), argv.data(), envp.data());
            perror("execve failed");
            ftFree(argv);
            ftFree(envp);
            exit(1);
        }
        ftFree(argv);
        ftFree(envp);
    }
    else 
    {
        int status;
        int timeout = 15; // Timeout en secondes

        close(pipefd[1]);
        while (timeout > 0) {
            // sleep(1);
            timeout--;
            if (waitpid(pid, &status, 0) != 0) {
                break;
            }
        }
        if (timeout == 0) {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            setErrorResponse(rrstate, 500, "Internal Server Error - CGI Timeout");
        } else {
            Cgi cgi;

            output = readDatasFromScript(pipefd[0]);
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                cgi.handleCgiResponse(output, rrstate);
            } else {
                setErrorResponse(rrstate, 500, "Internal Server Error - CGI Execution Failed");
            }
        }
    }
}
