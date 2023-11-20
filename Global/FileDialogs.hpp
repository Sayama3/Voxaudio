//
// Created by ianpo on 04/11/2023.
//

#pragma once

#include <string>
#include <vector>

namespace Voxymore::Audio {

    class FileDialogs {
    public:
        static std::string OpenFile(const std::vector<std::string>& filter);
        static std::string SaveFile(const std::vector<std::string>& filter);
    };

} // Voxymore
// Audio
