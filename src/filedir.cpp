#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <ctime>
#include <vector>
#include <sstream>

// File information structure
struct FileInfo {
    std::string name;
    std::string size;
    std::string modified;
    bool is_directory;
    
    FileInfo(const std::string& n, const std::string& s, const std::string& m, bool d)
        : name(n), size(s), modified(m), is_directory(d) {}
};

class DirectoryHandler {
private:
    static std::string human_readable_size(size_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB"};
        size_t unit = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024 && unit < 3) {
            size /= 1024;
            ++unit;
        }
        
        std::ostringstream oss;
        oss.precision(2);
        oss.setf(std::ios::fixed);
        oss << size << " " << units[unit];
        return oss.str();
    }

    static std::string format_time(time_t time) {
        char buffer[128];
        struct tm* timeinfo = localtime(&time);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string(buffer);
    }

    static bool compare_file_info(const FileInfo& a, const FileInfo& b) {
        if (a.is_directory != b.is_directory) {
            return a.is_directory > b.is_directory;
        }
        return a.name < b.name;
    }

    static std::string generate_breadcrumbs(const std::string& path) {
        std::string result = "<div class='breadcrumbs'><a href='/'>Home</a>";
        std::string current;
        std::istringstream iss(path);
        std::string token;
        
        while (std::getline(iss, token, '/')) {
            if (!token.empty()) {
                current += "/" + token;
                result += " / <a href='" + current + "'>" + token + "</a>";
            }
        }
        return result + "</div>";
    }

public:
    static std::vector<FileInfo> get_directory_listing(const std::string& dir_path) {
        std::vector<FileInfo> files;
        DIR* dir = opendir(dir_path.c_str());
        
        if (dir != NULL) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL) {
                std::string name = entry->d_name;
                if (name != "." && name != "..") {
                    std::string full_path = dir_path + "/" + name;
                    struct stat st;
                    
                    if (stat(full_path.c_str(), &st) == 0) {
                        bool is_dir = S_ISDIR(st.st_mode);
                        std::string size = is_dir ? "-" : human_readable_size(st.st_size);
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

    static std::string generate_directory_page(const std::string& path, const std::vector<FileInfo>& files) {
        std::ostringstream html;
        
        html << "<!DOCTYPE html>\n"
             << "<html>\n<head>\n"
             << "<title>Directory Listing - " << path << "</title>\n"
             << "<style>\n"
             << "body { font-family: Arial, sans-serif; margin: 20px; }\n"
             << ".breadcrumbs { margin-bottom: 20px; padding: 10px; background: #f5f5f5; }\n"
             << "table { width: 100%; border-collapse: collapse; }\n"
             << "th, td { padding: 8px; text-align: left; border-bottom: 1px solid #ddd; }\n"
             << "th { background-color: #f5f5f5; }\n"
             << ".folder { color: #2c5aa0; }\n"
             << ".upload-form { margin: 20px 0; padding: 20px; background: #f9f9f9; border: 1px solid #ddd; }\n"
             << "</style>\n"
             << "</head>\n<body>\n";

        // Add breadcrumbs
        html << generate_breadcrumbs(path);

        // Add upload form
        html << "<div class='upload-form'>\n"
             << "<h3>Upload File</h3>\n"
             << "<form action='" << path << "' method='POST' enctype='multipart/form-data'>\n"
             << "<input type='file' name='file' required>\n"
             << "<input type='submit' value='Upload'>\n"
             << "</form>\n"
             << "</div>\n";

        // Directory listing
        html << "<table>\n"
             << "<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>\n";

        for (size_t i = 0; i < files.size(); ++i) {
            const FileInfo& file = files[i];
            html << "<tr>\n"
                 << "<td>";
            
            if (file.is_directory) {
                html << "<span class='folder'>📁 </span>";
            } else {
                html << "📄 ";
            }
            
            html << "<a href='" << path 
                 << (path == "/" ? "" : "/") 
                 << file.name 
                 << "'>" << file.name << "</a></td>\n"
                 << "<td>" << file.size << "</td>\n"
                 << "<td>" << file.modified << "</td>\n"
                 << "</tr>\n";
        }
        
        html << "</table>\n</body>\n</html>";
        return html.str();
    }

    static bool create_directory(const std::string& path) {
        return mkdir(path.c_str(), 0755) == 0;
    }
};

// Add these methods to your HttpRequestHandler class
class HttpRequestHandler {
    // ... (existing code) ...

private:
    static std::string extract_boundary(const std::string& content_type) {
        size_t pos = content_type.find("boundary=");
        if (pos != std::string::npos) {
            return content_type.substr(pos + 9);
        }
        return "";
    }

    static std::string generate_error_response(const std::string& message) {
        std::ostringstream response;
        response << "HTTP/1.1 400 Bad Request\r\n"
                << "Content-Type: text/plain\r\n"
                << "Content-Length: " << message.length() << "\r\n"
                << "\r\n"
                << message;
        return response.str();
    }

public:
    void handle_directory_request(const std::string& path, HttpResponseHandler& response) {
        std::string dir_path = "public" + path;
        std::vector<FileInfo> files = DirectoryHandler::get_directory_listing(dir_path);
        std::string content = DirectoryHandler::generate_directory_page(path, files);
        
        response.setStatusCode(200);
        response.setStatusMsg("OK");
        response.setHeader("Content-Type", "text/html");
        response.setHeader("Content-Length", toString(content.length()));
        response.setBody(content);
    }

    void handle_file_upload(const std::string& request_data, const std::string& path, HttpResponseHandler& response) {
        std::string content_type = getHeader("Content-Type");
        std::string boundary = extract_boundary(content_type);
        
        if (boundary.empty()) {
            response.setStatusCode(400);
            response.setStatusMsg("Bad Request");
            response.setBody("Missing or invalid boundary in multipart form data");
            return;
        }

        // Parse multipart form data
        std::string delimiter = "--" + boundary + "\r\n";
        size_t pos = request_data.find(delimiter);
        if (pos == std::string::npos) {
            response.setStatusCode(400);
            response.setStatusMsg("Bad Request");
            response.setBody("Invalid multipart form data");
            return;
        }

        // Find filename
        pos = request_data.find("filename=\"", pos);
        if (pos == std::string::npos) {
            response.setStatusCode(400);
            response.setStatusMsg("Bad Request");
            response.setBody("No filename found in upload");
            return;
        }

        pos += 10;
        size_t end_pos = request_data.find("\"", pos);
        std::string filename = request_data.substr(pos, end_pos - pos);

        // Find start of file content
        pos = request_data.find("\r\n\r\n", end_pos) + 4;
        end_pos = request_data.find("--" + boundary + "--", pos);
        if (end_pos == std::string::npos) {
            response.setStatusCode(400);
            response.setStatusMsg("Bad Request");
            response.setBody("Invalid file content");
            return;
        }

        // Extract file content
        std::string file_content = request_data.substr(pos, end_pos - pos - 2);

        // Save file
        std::string upload_path = "public" + path + "/" + filename;
        std::ofstream file(upload_path.c_str(), std::ios::binary);
        if (!file) {
            response.setStatusCode(500);
            response.setStatusMsg("Internal Server Error");
            response.setBody("Failed to save uploaded file");
            return;
        }

        file.write(file_content.c_str(), file_content.length());
        file.close();

        // Redirect back to directory listing
        response.setStatusCode(302);
        response.setStatusMsg("Found");
        response.setHeader("Location", path);
    }

    HttpResponseHandler handlePath(const HttpRequestHandler& request, HttpResponseHandler& response) {
        const std::string static_dir = "public";
        std::string file_path = static_dir + request.getPath();
        
        // Handle file upload
        if (request.getMethod() == "POST") {
            handle_file_upload(request.getBody(), request.getPath(), response);
            return response;
        }

        // Check if path is a directory
        struct stat path_stat;
        if (stat(file_path.c_str(), &path_stat) == 0) {
            if (S_ISDIR(path_stat.st_mode)) {
                handle_directory_request(request.getPath(), response);
                return response;
            }
        }

        // Handle regular file requests (existing code)
        // ...
    }
};