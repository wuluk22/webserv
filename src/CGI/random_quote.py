#!/usr/bin/env python
import os
import random

def main():
    # Définir le chemin du fichier contenant les phrases
    file_path = "phrases.txt"

    # Lire les phrases depuis le fichier
    try:
        with open(file_path, "r") as file:
            phrases = file.readlines()
        
        # Sélectionner une phrase aléatoire
        if phrases:
            random_phrase = random.choice(phrases).strip()
        else:
            random_phrase = "Aucune phrase disponible dans le fichier."
    except FileNotFoundError:
        random_phrase = "Le fichier 'phrases.txt' est introuvable."
    except Exception as e:
        random_phrase = f"Erreur : {str(e)}"

    # Générer la réponse HTTP
    print("Content-Type: text/html\n")
    print("<html>")
    print("<head><title>Phrase aléatoire</title></head>")
    print("<body>")
    print(f"<h1>Voici une phrase aléatoire :</h1>")
    print(f"<p>{random_phrase}</p>")
    print("</body>")
    print("</html>")

if __name__ == "__main__":
    main()
