#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

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
