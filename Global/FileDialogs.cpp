//
// Created by ianpo on 04/11/2023.
//

#include "FileDialogs.hpp"
#include "portable-file-dialogs.h"

namespace Voxymore::Audio {
        std::string FileDialogs::OpenFile(const std::vector<std::string>& filter)
        {
            std::vector<std::string> result = pfd::open_file("Select a file", ".", filter).result();
            if(result.empty()) return {};
            else return result[0];
        }

        std::string FileDialogs::SaveFile(const std::vector<std::string>& filter)
        {
            std::string result = pfd::save_file("Select a file", ".", filter).result();
            return result;
        }
} // Voxymore::Audio