#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

# include "../Logger.hpp"
# include "../HttpRequestHandler.hpp"

std::string         getQuery(std::string path)
{
    std::string query;
    size_t pos = path.find('?');
    query = path.substr(pos + 1);
    return query;
}

std::vector<char *> homeMadeSetEnv(HttpRequestHandler request, std::string cgiPath)
{
    std::vector<std::string> stringEnv;
    std::vector<char *> envp;
    
    stringEnv.push_back("REQUEST_METHOD=" + request.getMethod());
    stringEnv.push_back("SCRIPT_PATH=" + cgiPath);
    if (request.getMethod() == "GET")
        stringEnv.push_back("QUERY_STRING=" + getQuery(request.getPath()));
    if (request.getMethod() == "POST") {
        stringEnv.push_back("CONTENT_TYPE=" + request.getMimeTypeCgi(request.getPath()));
        stringEnv.push_back("CONTENT_LENGTH=" + request.getHeader("CENTENT_LENGTH"));
    }
    stringEnv.push_back("SERVER_PROTOCOL=" + request.getHttpVersion());
    stringEnv.push_back("SERVER_SOFTWARE=WebServ/1.0");
    stringEnv.push_back("SERVER_NAME=" + request.getServerName());

    for (size_t i = 0; i < stringEnv.size(); i++) {
        envp.push_back(const_cast<char *>(stringEnv[i].c_str()));
    }
    envp.push_back(nullptr);
    return envp;
}

void handleCGI(HttpRequestHandler request, std::string cgiPath) {
    int pid = fork();
    if (pid < 0) {
        // Gestion d'erreur : le fork a échoué
        Logger::log("Fork failed!");
        return;
    } else if (pid == 0) { // Processus enfant
        std::vector<char *> envp = homeMadeSetEnv(request, cgiPath);
        // Redirection des entrées/sorties si nécessaire

        char* args[] = {const_cast<char*>(cgiPath.c_str()), NULL};
        if (access(cgiPath.c_str(), X_OK) == 0) {
            execve(cgiPath.c_str(), args, envp.data());
            // Si execve échoue
            for (size_t i = 0; i < envp.size(); ++i) {
                free(envp[i]);
            }
            free(args[0]);
            exit(1);
        }
    } else { // Processus parent
        // Optionnel : attendre la fin du processus enfant
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            Logger::log("CGI script exited with code: " + std::to_string(WEXITSTATUS(status)));
        }
    }
}

HttpRequestHandler createMockRequest() {
    HttpRequestHandler request;
    request.setMethod("GET");
    request.setPath("/cgi-bin/random_quote.py?image=image3");
    request.setHttpVersion("HTTP/1.0");
    request.setHeader("HOST", "localhost:8080");
    request.setHeader("CONTENT_LENGTH", "0");
    request.setHeader("CONTENT_TYPE", "");
    return request;
}

int main() {
    HttpRequestHandler request = createMockRequest();
    handleCGI(request, "random_quote.py");
}
