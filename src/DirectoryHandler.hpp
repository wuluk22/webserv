#ifndef DIRECTORYHANDLER_HPP
# define DIRECTORYHANDLER_HPP

# include <string>
# include <vector>

struct FileInfo
{
    std::string name;
    std::string size;
    std::string modified;
    bool is_directory;
    
    FileInfo(const std::string& n, const std::string& s, const std::string& m, bool d);
};

class DirectoryHandler
{
    private:
        static std::string human_readable_size(size_t bytes);
        static std::string format_time(time_t time);
        static bool compare_file_info(const FileInfo& a, const FileInfo& b);
        static std::string generate_breadcrumbs(const std::string& path);
        static std::string get_mime_type(const std::string& path);

    public:
        DirectoryHandler();
        ~DirectoryHandler();

        static std::vector<FileInfo> get_directory_listing(const std::string& dir_path);
        static std::string generate_directory_page(const std::string& path, const std::vector<FileInfo>& files);
        static bool create_directory(const std::string& path);
        static bool is_directory(const std::string& path);
};

#endif