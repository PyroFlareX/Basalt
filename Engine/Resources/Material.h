#pragma once

#include "../Types/Types.h"

namespace vn
{
    struct Material
    {
        /// Maps
        //Texture
        short texture_id = 0;
        //Normal
        short normal_id = 1;
        
        /// Values
        vn::vec4 diffuse; //Diffuse
        vn::vec4 ambient; //Ambient
        vn::vec4 specular; //Specular


    };
}