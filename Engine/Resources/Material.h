#pragma once

#include "../Types/Types.h"

namespace vn
{
    struct Material
    {
        /// Maps
        short texture_id = 0; //Texture
        short normal_id = 1; //Normal
        
        /// Values
        vn::vec4 diffuse; //Diffuse
        vn::vec4 ambient; //Ambient
        vn::vec4 specular; //Specular


    };
}