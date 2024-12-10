#!/usr/bin/env python
import os
import random

def main():
    file_path = "phrases.txt"

    try:
        with open(file_path, "r") as file:
            phrases = file.readlines()
        if phrases:
            random_phrase = random.choice(phrases).strip()
        else:
            random_phrase = "Aucune phrase disponible dans le fichier."
    except FileNotFoundError:
        random_phrase = "Le fichier 'phrases.txt' est introuvable."
    except Exception as e:
        random_phrase = "Erreur : " + str(e)

    print("Content-Type: text/html\n")
    print("<html>")
    print("<head><title>Phrase aleatoire</title></head>")
    print("<body>")
    print("<h1>Voici une phrase aleatoire :</h1>")
    print("<p>{}</p>".format(random_phrase))
    print("</body>")
    print("</html>")

if __name__ == "__main__":
    main()
