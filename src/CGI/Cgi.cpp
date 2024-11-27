#include "Cgi.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Fonction pour configurer les variables d'environnement pour le script CGI
void setEnv(const std::string &key, const std::string &value) {
    setenv(key.c_str(), value.c_str(), 1);  // Définit ou remplace la variable d'environnement
}

// Fonction pour exécuter le script CGI
void executeCGI(const std::string &scriptPath, const std::string &queryString) {
    int pipe_stdout[2];

    if (pipe(pipe_stdout) == -1) {
        std::cerr << "Erreur lors de la création du pipe" << std::endl;
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Erreur lors du fork" << std::endl;
        return;
    }

    if (pid == 0) { // Processus enfant
        // Redirection de la sortie standard vers le pipe
        dup2(pipe_stdout[1], STDOUT_FILENO);
        close(pipe_stdout[0]);

        // Définir les variables d'environnement pour le CGI
        setEnv("QUERY_STRING", queryString);

        // Exécuter le script CGI (ici un script Python)
        char *args[] = { const_cast<char*>(scriptPath.c_str()), NULL };
        execve(scriptPath.c_str(), args, environ);

        // Si execve échoue
        std::cerr << "Échec de l'exécution du script CGI" << std::endl;
        exit(1);
    } else { // Processus parent
        close(pipe_stdout[1]);
        
        // Lire la réponse du script CGI
        char buffer[1024];
        ssize_t bytesRead;
        std::string response;
        while ((bytesRead = read(pipe_stdout[0], buffer, sizeof(buffer))) > 0) {
            response.append(buffer, bytesRead);
        }
        
        // Afficher la réponse du script CGI
        std::cout << "Réponse du script CGI :\n" << response << std::endl;

        // Attendre que le processus enfant se termine
        int status;
        waitpid(pid, &status, 0);
    }
}

int main() {
    // Emplacement du script CGI (un fichier Python ici)
    std::string scriptPath = "/path/to/your/script.py"; // Remplace avec le chemin réel
    std::string queryString = "name=Salowie";  // Paramètre GET de la requête

    executeCGI(scriptPath, queryString);
    
    return 0;
}
