#pragma once

#include "../Types/Types.h"

namespace bs
{
    struct Material
    {
        /// Maps
        short texture_id = 0; //Texture
        short normal_id = 1; //Normal
        
        /// Values
        bs::vec4 diffuse; //Diffuse
        bs::vec4 ambient; //Ambient
        bs::vec4 specular; //Specular


    };
}