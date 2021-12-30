#pragma once

#include "../Types/Types.h"

namespace bs
{
    struct Material
    {
        /// Maps
        u16 texture_id = 0; //Texture
        u16 normal_id = 1;  //Normal
        u16 tex_id_1 = 0;   //extra id
        u16 tex_id_2 = 0;   //extra id
        
        /// Lighting
        vec4 diffuse; //Diffuse
        vec4 ambient; //Ambient
        vec4 specular; //Specular
    };
}