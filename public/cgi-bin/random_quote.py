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
        selected_image = None  # Pas d'image valide

    # Générer la page HTML avec le contenu dynamique
    html_content = f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>CGI Response</title>
        <style>
            body {{
                font-family: Arial, sans-serif;
                text-align: center;
                margin: 20px;
            }}
            h1 {{
                color: #333;
            }}
            .image-container {{
                position: relative;
                display: inline-block;
                margin: 20px;
            }}
            .image-container img {{
                max-width: 100%;
                height: auto;
                border: 2px solid #ddd;
                border-radius: 8px;
                box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
            }}
            .image-container .caption {{
                position: absolute;
                top: 50%;
                left: 50%;
                transform: translate(-50%, -50%);
                background-color: rgba(0, 0, 0, 0.6); /* Fond semi-transparent */
                color: white;
                padding: 5px 10px;
                border-radius: 5px;
                font-size: 18px;
                text-align: center; /* Centre le texte si plusieurs lignes */
                white-space: nowrap; /* Empêche le retour à la ligne (optionnel) */
            }}
            .home-link {{
                display: inline-block;
                margin-top: 20px;
                padding: 10px 20px;
                font-size: 16px;
                color: white;
                background-color: #007bff;
                text-decoration: none;
                border-radius: 5px;
            }}
            .home-link:hover {{
                background-color: #0056b3;
            }}
        </style>
    </head>
    <body>
        <h1>CGI Response</h1>

        <!-- Conteneur de l'image et du texte -->
        <div class="image-container">
            {"<img src='/Images/" + selected_image + "' alt='Selected Image'>" if selected_image else "<p>Aucune image sélectionnée</p>"}
            <div class="caption">{random_phrase}</div>
        </div>

        <a href="/" class="home-link">Return Home</a>
    </body>
    </html>
    """

    # Afficher la réponse
    print("Content-Type: text/html\r\n\r\n")
    print(html_content)
    # while True:
    #     print("Hello World!")
if __name__ == "__main__":
    main()
