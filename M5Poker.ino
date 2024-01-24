#include <M5Cardputer.h>

const int cardY = 10;

LGFX_Sprite sprite = LGFX_Sprite(&M5Cardputer.Display);
u8_t deck[13 * 4];
int card_index = 0;

static u8_t hand[5] = {};

void setup()
{
    M5Cardputer.begin();
    sprite.createSprite(M5Cardputer.Display.width(), M5Cardputer.Display.height());
    // width = 240
    // height = 135
    sprite.setRotation(1);

    for (int i = 0; i < 13 * 4; i++)
    {
        deck[i] = i;
    }

    shuffle();

    for (int i = 0; i < 5; i++)
    {
        hand[i] = dealCard();
    }
}

void loop()
{
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
                    hand[i] = dealCard();
                }
            }
            holds = 0;
            shuffle();
        }
    }

    sprite.clear();

    drawCards(hand, holds);

    sprite.pushSprite(0, 0);
    delay(40);
}

void shuffle()
{
    card_index = 0;
    for (int i = 0; i < 51; i++)
    {
        int j = random(i, 52);
        u8_t tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }
}

u8_t dealCard()
{
    return deck[(card_index++) % 52];
}

void drawCards(u8_t hand[5], u8_t holds)
{
    for (int i = 0; i < 5; i++)
    {
        drawCard(hand[i], i);
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
    sprite.drawString(numbers[card / 4], x + 5, cardY + 5);

    sprite.setTextDatum(middle_center);
    sprite.drawString(suits[card % 4], x + 21, cardY + 35);
}
