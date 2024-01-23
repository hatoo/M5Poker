#include <M5Cardputer.h>

LGFX_Sprite sprite = LGFX_Sprite(&M5Cardputer.Display);

void setup()
{
    M5Cardputer.begin();
    sprite.createSprite(M5Cardputer.Display.width(), M5Cardputer.Display.height());
    sprite.setRotation(1);
}

void loop()
{
    static int counter = 0;
    M5Cardputer.update();
    if (M5Cardputer.BtnA.wasPressed())
    {
        counter++;
    }

    sprite.clear();
    sprite.setCursor(0, 0);
    sprite.printf("width = %d, height = %d", M5Cardputer.Display.width(), M5Cardputer.Display.height());
    sprite.drawLine(0, 0, 240, 135, TFT_WHITE);
    sprite.drawLine(0, 60, 120, 60, TFT_GREEN);
    sprite.drawCircle(0, 0, 16, TFT_RED);

    sprite.pushSprite(0, 0);
    delay(100);
}
