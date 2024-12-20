#ifndef DIRECTORYHANDLER_HPP
# define DIRECTORYHANDLER_HPP

# include <string>
# include <vector>
# include <dirent.h>
# include <sys/stat.h>
# include <algorithm>
# include <ctime>
# include <sstream>
# include <iostream>

struct FileInfo
{
    std::string name;
    std::string size;
    std::string modified;
    bool isDirectory;
    
    FileInfo(const std::string& n, const std::string& s, const std::string& m, bool d);
};

class DirectoryHandler
{
    private:
        static std::string				humanReadableSize(size_t bytes);
        static std::string				formatTime(time_t time);
        static bool						compareFileInfo(const FileInfo& a, const FileInfo& b);
        static std::string				generateBreadcrumbs(const std::string& path);
        static std::string				getMimeType(const std::string& path);

    public:
        DirectoryHandler();
        ~DirectoryHandler();

        static std::vector<FileInfo>	getDirectoryListing(const std::string& dir_path);
        static std::string				generateDirectoryPage(const std::string& path, const std::vector<FileInfo>& files);
        static bool						createDirectory(const std::string& path);
        static bool						isDirectory(const std::string& path);
};

#endif
