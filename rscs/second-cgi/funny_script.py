#!/usr/bin/env python3
import cgi
import os

def main():
    form = cgi.FieldStorage()
    
    name = form.getvalue("name", "")
    age = form.getvalue("age", "")
    path = os.environ.get("PATH", "")
    SCRIPTPATH = os.environ.get("SCRIPT_NAME", "")
    clean_path = SCRIPTPATH.replace(path, "")
    
    if name and age:
        # If name and age are provided, display the result
        message = f"Hello, my name is {name} and I'm {age} years old."
    else:
        # If name or age is missing, prompt the user to fill the form
        message = f"""
        <h1>Please enter your details</h1>
        <form method="GET" action="{clean_path}">
            <label for="name">Name:</label><br>
            <input type="text" id="name" name="name" required><br><br>
            <label for="age">Age:</label><br>
            <input type="number" id="age" name="age" required><br><br>
            <button type="submit">Submit</button>
        </form>
        """
    
    # Output the message to the user
    print("Content-type: text/html\n")
    print(f"<html><body>{message}</body></html>")

    
    # Output HTML content
    print("Content-Type: text/html\r\n\r\n")
    print(f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>User Info</title>
        <style>
            body {{
                font-family: Arial, sans-serif;
                margin: 20px;
                text-align: center;
            }}
            form {{
                margin-top: 20px;
            }}
        </style>
    </head>
    <body>
        <button class="home-button" onclick="location.href='/'">Home</button>
        <h1>Welcome</h1>
        <p>{message}</p>
    </body>
    </html>
    """)

if __name__ == "__main__":
    main()
