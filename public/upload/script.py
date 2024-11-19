import os
import subprocess

def delete_first_12_lines(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    
    with open(file_path, 'w') as file:
        file.writelines(lines[12:])

def process_files_in_directory(directory_path):
    for filename in os.listdir(directory_path):
        file_path = os.path.join(directory_path, filename)
        if os.path.isfile(file_path):
            delete_first_12_lines(file_path)
            subprocess.call(['vim', '-c', 'normal \\<F1>', file_path])

if __name__ == "__main__":
    directory_path = input("Enter the directory path: ")
    print("Directory path entered: {}".format(directory_path))
    process_files_in_directory(directory_path)