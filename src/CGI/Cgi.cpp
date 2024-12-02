#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

# include "../Logger.hpp"
# include "../HttpRequestHandler.hpp"

std::vector<char *> homeMadeSetEnv(HttpRequestHandler request)
{
    std::vector<std::string> stringEnv;
    std::vector<char *> envp;
    
    stringEnv.push_back("REQUEST_METHOD=" + request.getMethod());
    stringEnv.push_back("SCRIPT_PATH=" + request.getPath()); // getPath is correct ?
    if (request.getMethod() == "GET")
        stringEnv.push_back("QUERY_STRING=" + request.getQuery());
    if (request.getMethod() == "POST") {
        stringEnv.push_back("CONTENT_TYPE=" + request.getMimeType(request.getPath())); // possile ?
        stringEnv.push_back("CONTENT_LENGTH=" + request.getContentLength());
    }
    stringEnv.push_back("SERVER_PROTOCOL=" + request.getHttpVersion());
    stringEnv.push_back("SERVER_SOFTWARE=WebServ/1.0");
    stringEnv.push_back("SERVER_NAME=" + request.getServerName());

    for (size_t i = 0; i < stringEnv.size(); i++) {
        envp.push_back(const_cast<char *>(stringEnv[i].c_str()));
    }
    return envp;
}

void handleCGI(HttpRequestHandler request) {
    int pid = fork();
    if (pid < 0) {
        // Gestion d'erreur : le fork a échoué
        Logger::log("Fork failed!");
        return;
    } else if (pid == 0) {
        // Processus enfant
        // Configurer les variables d'environnement pour CGI
        std::vector<char *> envp = homeMadeSetEnv(request);
        // Redirection des entrées/sorties si nécessaire
        // ...
        // Exécuter le script CGI
        char* args[] = {const_cast<char*>(scriptPath.c_str()), NULL};
        execve(scriptPath.c_str(), args, environ);
        // Si execve échoue
        exit(1);
    } else {
        // Processus parent
        // Optionnel : attendre la fin du processus enfant
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            Logger::log("CGI script exited with code: " + std::to_string(WEXITSTATUS(status)));
        }
    }
}

int main() {
    // Préparer les variables d'environnement
    char* env[] = {
        strdup("REQUEST_METHOD=GET"),
        strdup("QUERY_STRING=name=Salowie&age=25"),
        strdup("CONTENT_LENGTH=0"),
        strdup("CONTENT_TYPE=text/html"),
        strdup("SCRIPT_NAME=/cgi-bin/random_quote.py"),
        strdup("SERVER_NAME=localhost"),
        strdup("SERVER_PORT=8080"),
        strdup("REMOTE_ADDR=127.0.0.1"),
        strdup("SERVER_PROTOCOL=HTTP/1.1"),
        nullptr
    };

    // Chemin vers le script CGI
    const char* scriptPath = "random_quote.py";

    // Exécution du script CGI
    char* args[] = {strdup(scriptPath), nullptr};

    // Exécute le script CGI avec les variables d'environnement
    if (execve(scriptPath, args, env) == -1) {
        perror("Erreur lors de l'exécution du script CGI");
    }

    // Libérer la mémoire des variables d'environnement
    for (int i = 0; env[i] != nullptr; ++i) {
        free(env[i]);
    }

    return 0;
}
