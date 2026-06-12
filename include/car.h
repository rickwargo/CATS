#pragma once

#define SKY_COLOR TFT_BLUE
#define GROUND_COLOR TFT_OLIVE

void drawCar(TFT_eSprite& sprite, int top, int width, bool drawHeadlights = false)
{
    int height = 7 * width / 3;
    int x = FACE_CENTER_X - width/2;
    int y = top >= 0 ? top : FACE_CENTER_Y + top;

    if (y < 0) y = FACE_CENTER_Y - height/2;    // centered vertically on screen if initial top < -FACE_CENTER_X

    // 1. Tires (4 rectangles protruding from the body) height proportional to tire width
    sprite.fillRect( x - width/5,  y + height/5, width/5, 2*width/5, TFT_BLACK); // Top-left
    sprite.fillRect( x - width/5, y + 3*height/5+2 + (height/5-2*width/5), width/5, 2*width/5, TFT_BLACK); // Bottom-left
    sprite.fillRect(x + width,  y + height/5, width/5, 2*width/5, TFT_BLACK); // Top-right
    sprite.fillRect(x + width, y + 3*height/5+2 + (height/5-2*width/5), width/5, 2*width/5, TFT_BLACK); // Bottom-right

    // 2. Main Car Body
    sprite.fillRect(x, y, width, height, TFT_RED); // Chassis

    // 3. Windshield and Roof
    sprite.fillRect(x + width/7, y + height/5, width - 2*width/7, height/5, TFT_LIGHTGREY);     // Windshield
    sprite.fillRect(x + width/7, y + 2*height/5, width - 2*width/7, height/3, TFT_RED);  // Roof
    sprite.fillRect(x + width/7, y + 2*height/5+height/3, width - 2*width/7, height/3 - height/5, TFT_LIGHTGREY);     // Windshield

    // 4. Headlights (tft_transparent just adds a bit of hood rounding)
    sprite.fillRect(x, y, width/5, 2*width/5/3, drawHeadlights ? TFT_YELLOW : TFT_TRANSPARENT);
    sprite.fillRect(x + 4*width/5, y, width/5, 2*width/5/3, drawHeadlights ? TFT_YELLOW : TFT_TRANSPARENT);
}

void drawRoad(TFT_eSprite& sprite, int carWidth, int percentage = 20)
{
    const int tireWidth = carWidth / 5;
    const int streetWidth = carWidth + 2 * tireWidth*2;
    const int streetHeight = sprite.getViewportHeight();

    sprite.fillRect(FACE_CENTER_X - streetWidth/2 - tireWidth, 0, tireWidth, streetHeight, TFT_WHITE);
    sprite.fillRect(FACE_CENTER_X - carWidth/2 - tireWidth*2, 0, streetWidth, streetHeight, TFT_DARKGREY);
    sprite.fillRect(FACE_CENTER_X + streetWidth/2, 0, tireWidth, streetHeight, TFT_WHITE);

    const int dashWidth = tireWidth / 2;
    const int dashHeight = dashWidth * 2;
    const int dashCount = streetHeight / dashHeight*2 + 1;
    const int offset = percentage * dashHeight * 2 / 100;
    for (int i = -1; i < dashCount; i++)
    {
        sprite.fillRect(FACE_CENTER_X - dashWidth/2, offset + i * dashHeight*2, dashWidth, dashHeight, TFT_WHITE);
    }
}
