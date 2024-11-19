#include "DirectoryHandler.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iostream>

FileInfo::FileInfo(const std::string& n, const std::string& s, const std::string& m, bool d)
    : name(n), size(s), modified(m), is_directory(d) {}

DirectoryHandler::DirectoryHandler() {}
DirectoryHandler::~DirectoryHandler() {}

std::string DirectoryHandler::human_readable_size(size_t bytes)
{
    const char* units[] = {"B", "KB", "MB", "GB"};
    size_t unit = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024 && unit < 3)
    {
        size /= 1024;
        unit++;
    }
    
    std::ostringstream oss;
    oss.precision(2);
    oss.setf(std::ios::fixed);
    oss << size << " " << units[unit];
    return oss.str();
}

std::string DirectoryHandler::format_time(time_t time)
{
    char buffer[128];
    struct tm* timeinfo = localtime(&time);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

bool DirectoryHandler::compare_file_info(const FileInfo& a, const FileInfo& b)
{
    if (a.is_directory != b.is_directory)
    {
        return a.is_directory > b.is_directory;
    }
    return a.name < b.name;
}

std::string DirectoryHandler::generate_breadcrumbs(const std::string& path)
{
    std::string result = "<div class='breadcrumbs'><a href='/'>Home</a>";
    std::string current;
    std::istringstream iss(path);
    std::string token;
    
    while (std::getline(iss, token, '/'))
    {
        if (!token.empty())
        {
            current += "/" + token;
            result += " / <a href='" + current + "'>" + token + "</a>";
        }
    }
    return result + "</div>";
}

std::string DirectoryHandler::get_mime_type(const std::string& path)
{
    std::string ext;
    size_t dot_pos = path.find_last_of(".");
    
    if (dot_pos != std::string::npos)
    {
        ext = path.substr(dot_pos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }

    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".png") return "image/png";
    if (ext == ".gif") return "image/gif";
    if (ext == ".pdf") return "application/pdf";
    if (ext == ".txt") return "text/plain";
    
    return "application/octet-stream";
}

std::vector<FileInfo> DirectoryHandler::get_directory_listing(const std::string& dir_path)
{
    std::vector<FileInfo> files;
    DIR* dir = opendir(dir_path.c_str());
    
    if (dir != NULL)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
        {
            std::string name = entry->d_name;
            if (name != "." && name != "..")
            {
                std::string full_path = dir_path + "/" + name;
                struct stat st;
                
                if (stat(full_path.c_str(), &st) == 0)
                {
                    bool is_dir = S_ISDIR(st.st_mode);
                    std::string size;
                    if (is_dir)
                    {
                        size = "-";
                    }
                    else
                    {
                        size = human_readable_size(st.st_size);
                    }
                    std::string modified = format_time(st.st_mtime);
                    files.push_back(FileInfo(name, size, modified, is_dir));
                }
            }
        }
        closedir(dir);
    }
    
    std::sort(files.begin(), files.end(), compare_file_info);
    return files;
}

std::string DirectoryHandler::generate_directory_page(const std::string& path, const std::vector<FileInfo>& files)
{
    std::ostringstream html;
    
    html << "<!DOCTYPE html>\n"
         << "<html>\n<head>\n"
         << "<title>Directory Listing - " << path << "</title>\n"
         << "<style>\n"
         << "body { font-family: Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }\n"
         << ".container { max-width: 1200px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n"
         << ".breadcrumbs { margin-bottom: 20px; padding: 10px; background: #f8f9fa; border-radius: 4px; }\n"
         << "table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n"
         << "th, td { padding: 12px 8px; text-align: left; border-bottom: 1px solid #ddd; }\n"
         << "th { background-color: #f8f9fa; font-weight: bold; }\n"
         << "tr:hover { background-color: #f8f9fa; }\n"
         << ".folder { color: #2c5aa0; }\n"
         << ".file { color: #444; }\n"
         << ".upload-form { margin: 20px 0; padding: 20px; background: #f8f9fa; border-radius: 4px; border: 1px solid #ddd; }\n"
         << ".upload-form input[type='file'] { display: block; margin: 10px 0; }\n"
         << ".upload-form input[type='submit'] { background: #2c5aa0; color: white; border: none; padding: 8px 16px; border-radius: 4px; cursor: pointer; }\n"
         << ".upload-form input[type='submit']:hover { background: #1e4b8f; }\n"
         << "a { color: #2c5aa0; text-decoration: none; }\n"
         << "a:hover { text-decoration: underline; }\n"
         << "</style>\n"
         << "</head>\n<body>\n"
         << "<div class='container'>\n";

    html << generate_breadcrumbs(path);

    html << "<div class='upload-form'>\n"
         << "<h3>Upload File</h3>\n"
         << "<form action='" << path << "' method='POST' enctype='multipart/form-data'>\n"
         << "<input type='file' name='file' required>\n"
         << "<input type='submit' value='Upload'>\n"
         << "</form>\n"
         << "</div>\n";

    html << "<table>\n"
         << "<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>\n";

    std::vector<FileInfo>::const_iterator it;
    for (it = files.begin(); it != files.end(); ++it)
    {
        const FileInfo& file = *it;
        html << "<tr>\n<td>";
        
        if (file.is_directory)
        {
            html << "<span class='folder'>[] </span>";
        }
        else
        {
            html << "<span class='file'>|| </span>";
        }
        
        std::string link_path;
        if (path == "/")
        {
            link_path = path + file.name;
        }
        else
        {
            link_path = path + "/" + file.name;
        }
        
        html << "<a href='" << link_path << "'>" << file.name << "</a></td>\n"
             << "<td>" << file.size << "</td>\n"
             << "<td>" << file.modified << "</td>\n"
             << "</tr>\n";
    }
    
    html << "</table>\n</div>\n</body>\n</html>";
    return html.str();
}

bool DirectoryHandler::create_directory(const std::string& path)
{
    return mkdir(path.c_str(), 0755) == 0;
}

bool DirectoryHandler::is_directory(const std::string& path)
{
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) == 0)
    {
        return S_ISDIR(path_stat.st_mode);
    }
    return false;
}