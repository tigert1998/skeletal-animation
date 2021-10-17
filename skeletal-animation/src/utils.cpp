#include "utils.h"

std::string BaseName(const std::string &path) {
    int i;
    for (i = (int) path.length() - 1; i >= 0; i--) 
        if (path[i] == '\\' || path[i] == '/') break;
    return path.substr(i + 1);
}

std::string ParentPath(const std::string &path) {
    int i;
    for (i = path.size() - 2; i >= 0; i--) {
        if (path[i] == '/' || path[i] == '\\' && path[i + 1] == '\\') {
            break;   
        }
    }
    return path.substr(0, i);
}