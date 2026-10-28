#pragma once


#include "GamesEngineeringBase.hpp"
#include "vec2f.hpp"
#include "shapes.hpp"


inline void Renderer_draw_image(GamesEngineeringBase::Window& window,
    Vec2f position,
    const GamesEngineeringBase::Image& image)
{

    if (image.data == nullptr)
    {
        return;
    }

    // std::cout << "Drawing image at position (" << position.x_ << ", " << position.y_ << ")" << std::endl;

    for (unsigned int row = 0; row < image.height; ++row)
    {
        const int y = position.y_ + static_cast<int>(row);

        if (y < 0 || y >= window.getHeight())
        {
            // std::cout << "Skipping row " << row << " as it is outside the window bounds." << std::endl;
            continue;
        }

        for (unsigned int col = 0; col < image.width; ++col)
        {
            const int x = position.x_ + static_cast<int>(col);

            if (x < 0 || x >= window.getWidth())
            {
                // std::cout << "Skipping column " << col << " as it is outside the window bounds." << std::endl;  
                continue;
            }

            const unsigned int pixelIndex = (row * image.width + col) * image.channels;

            if (image.channels == 3 || image.data[pixelIndex + 3] != 0)
            {
                window.draw(x, y, &image.data[pixelIndex]);
            }
        }
    }
}



   
