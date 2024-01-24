#include <M5Cardputer.h>

const int cardY = 10;

LGFX_Sprite sprite = LGFX_Sprite(&M5Cardputer.Display);

void setup()
{
    M5Cardputer.begin();
    sprite.createSprite(M5Cardputer.Display.width(), M5Cardputer.Display.height());
    // width = 240
    // height = 135
    sprite.setRotation(1);
}

void loop()
{
    static u8_t cards[5] = {0x00, 0x11, 0x22, 0x33, 0x40};
    static u8_t holds = 0;
    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isChange())
    {
        if (M5Cardputer.Keyboard.isKeyPressed('1'))
        {
            holds ^= 0x01;
        }

        if (M5Cardputer.Keyboard.isKeyPressed('2'))
        {
            holds ^= 0x02;
        }

        if (M5Cardputer.Keyboard.isKeyPressed('3'))
        {
            holds ^= 0x04;
        }

        if (M5Cardputer.Keyboard.isKeyPressed('4'))
        {
            holds ^= 0x08;
        }

        if (M5Cardputer.Keyboard.isKeyPressed('5'))
        {
            holds ^= 0x10;
        }

        if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER))
        {
            for (int i = 0; i < 5; i++)
            {
                if (!(holds & (1 << i)))
                {
                    cards[i] = randCard();
                }
            }
            holds = 0;
        }
    }

    sprite.clear();

    drawCards(cards, holds);

    sprite.pushSprite(0, 0);
    delay(40);
}

u8_t randCard()
{
    int n = random(13);
    int s = random(4);

    return (n << 4) | s;
}

void drawCards(u8_t cards[5], u8_t holds)
{
    for (int i = 0; i < 5; i++)
    {
        drawCard(cards[i], i);
    }

    sprite.setTextDatum(middle_center);
    sprite.setTextSize(1.5);
    for (int i = 0; i < 5; i++)
    {
        if (holds & (1 << i))
        {
            sprite.drawString("HOLD", 5 + i * (42 + 5) + 21, cardY + 60 + 5);
        }
    }
}

void drawCard(u8_t card, int pos)
{
    const char *numbers[13] = {"A", "2", "3", "4", "5", "6",
                               "7", "8", "9", "10", "J", "Q", "K"};
    // TODO: Draw graphical suites
    const char *suits[4] = {"S", "D", "C", "H"};

    int x = 5 + pos * (42 + 5);

    sprite.drawRect(x, cardY, 42, 60, TFT_WHITE);

    sprite.setTextDatum(top_left);
    sprite.setTextSize(2);
    sprite.drawString(numbers[card >> 4], x + 5, cardY + 5);

    sprite.setTextDatum(middle_center);
    sprite.drawString(suits[card & 0x0F], x + 21, cardY + 35);
}
