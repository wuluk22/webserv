#!/usr/bin/env python3
import os
import random
import cgi
import cgitb

# Activer le mode debug pour les erreurs CGI
cgitb.enable()

PHRASES_FILE = "/home/salowie/Documents/webserv/public/phrases.txt"
IMAGES_DIR = "/home/salowie/Documents/webserv/public/Images/"

def get_random_phrase(file_path):
    """Retourne une phrase aléatoire à partir d'un fichier."""
    try:
        with open(file_path, "r") as file:
            phrases = file.readlines()
        if phrases:
            return random.choice(phrases).strip()
        else:
            return "Aucune phrase disponible dans le fichier."
    except FileNotFoundError:
        return "Le fichier 'phrases.txt' est introuvable."
    except Exception as e:
        return f"Erreur : {e}"

def main():
    # Récupérer les données de la query string
    form = cgi.FieldStorage()
    selected_image = form.getvalue("image", "Aucune image sélectionnée")

    # Obtenir une phrase aléatoire
    random_phrase = get_random_phrase(PHRASES_FILE)

    # Vérifier si l'image existe
    image_path = os.path.join(IMAGES_DIR, selected_image)
    if not os.path.isfile(image_path):
        selected_image = "Image non trouvée"

    # Générer la page HTML
    print("Content-Type: text/html\r\n\r\n") 
    print("<html>")
    print("<head><title>Phrase et Image</title></head>")
    print("<body>")
    print(f"<p>{random_phrase}</p>")
    print(f"<p>{selected_image}</p>")
    if selected_image != "Aucune image sélectionnée" and selected_image != "Image non trouvée":
        print(f'<img src="/Images/{selected_image}" alt="Image sélectionnée" style="max-width:300px;">')
    print("</body>")
    print("</html>")

if __name__ == "__main__":
    main()
