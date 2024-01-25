#include <M5Cardputer.h>

const int cardY = 30;

LGFX_Sprite sprite = LGFX_Sprite(&M5Cardputer.Display);
u8_t deck[13 * 4];
int card_index = 0;

int credit = 100;

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
}

void loop()
{
    static int state = 0;
    static u8_t hand[5] = {};
    static u8_t holds = 0;
    M5Cardputer.update();

    sprite.clear();

    drawInfo();

    switch (state)
    {
    case 0:
        // Pre deal
        for (int i = 0; i < 5; i++)
        {
            drawCardBack(i);
        }

        if (M5Cardputer.Keyboard.isChange())
        {
            if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER))
            {
                state = 1;
                credit -= 1;
                shuffle();
                for (int i = 0; i < 5; i++)
                {
                    hand[i] = dealCard();
                }
                holds = 0;
            }
        }

        break;

    case 1:
        drawCards(hand, holds);
        // Post deal
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
                state = 2;
            }
        }
        break;
    case 2:
        // Post draw
        char buf[80];
        char *hand_name;

        drawCards(hand, 0);
        // TODO
        int back = score(hand, &hand_name);
        sprite.setTextDatum(middle_center);
        sprintf(buf, "%s: %d", hand_name, back);
        sprite.drawString(buf, 120, 90);

        if (M5Cardputer.Keyboard.isChange())
        {
            if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER))
            {
                credit += back;
                state = 0;
            }
        }
    }

    sprite.pushSprite(0, 0);
    delay(40);
}

void sort(u8_t hand[5])
{
    // simple bubble sort
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 4 - i; j++)
        {
            if (hand[j] > hand[j + 1])
            {
                u8_t tmp = hand[j];
                hand[j] = hand[j + 1];
                hand[j + 1] = tmp;
            }
        }
    }
}

bool is_flush(u8_t hand[5])
{
    u8_t suit = hand[0] % 4;
    for (int i = 1; i < 5; i++)
    {
        if (hand[i] % 4 != suit)
        {
            return false;
        }
    }
    return true;
}

bool is_ace_high_straight(u8_t hand[5])
{
    return hand[0] / 4 == 0 && hand[1] / 4 == 9 && hand[2] / 4 == 10 && hand[3] / 4 == 11 && hand[4] / 4 == 12;
}

bool is_straight(u8_t hand[5])
{
    for (int i = 0; i < 4; i++)
    {
        if (hand[i] / 4 + 1 != hand[i + 1] / 4)
        {
            return is_ace_high_straight(hand);
        }
    }
    return true;
}

int max_same_number(u8_t hand[5])
{
    int last = -1;
    int count = 0;

    int max_count = 0;

    for (int i = 0; i < 5; i++)
    {
        if (hand[i] / 4 == last)
        {
            count++;
        }
        else
        {
            if (count > max_count)
            {
                max_count = count;
            }
            count = 1;
            last = hand[i] / 4;
        }
    }

    if (count > max_count)
    {
        max_count = count;
    }

    return max_count;
}

bool is_two_pair(u8_t hand[5])
{
    int last = -1;
    int count = 0;
    int pairs = 0;

    for (int i = 0; i < 5; i++)
    {
        if (hand[i] / 4 == last)
        {
            count++;
        }
        else
        {
            if (count == 2)
            {
                pairs++;
            }
            count = 1;
            last = hand[i] / 4;
        }
    }

    if (count == 2)
    {
        pairs++;
    }

    return pairs == 2;
}

bool is_jacks_or_better(u8_t hand[5])
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = i + 1; j < 5; j++)
        {
            if (hand[i] / 4 == hand[j] / 4 && (hand[i] / 4 >= 10 || hand[i] / 4 == 0))
            {
                return true;
            }
        }
    }
    return false;
}

int score(u8_t hand[5], char **hand_name)
{
    u8_t h[5] = {hand[0], hand[1], hand[2], hand[3], hand[4]};
    sort(h);

    // royal flush
    if (is_flush(h) && is_ace_high_straight(h))
    {
        *hand_name = "Royal Flush";
        return 800;
    }

    // straight flush
    if (is_flush(h) && is_straight(h))
    {
        *hand_name = "Straight Flush";
        return 50;
    }

    // four of a kind
    if (max_same_number(h) == 4)
    {
        *hand_name = "Four of a Kind";
        return 25;
    }

    // full house
    if (
        (h[0] / 4 == h[1] / 4 && h[1] / 4 == h[2] / 4 && h[3] / 4 == h[4] / 4) ||
        (h[0] / 4 == h[1] / 4 && h[2] / 4 == h[3] / 4 && h[3] / 4 == h[4] / 4))
    {
        *hand_name = "Full House";
        return 9;
    }

    // flush
    if (is_flush(h))
    {
        *hand_name = "Flush";
        return 6;
    }

    // straight
    if (is_straight(h))
    {
        *hand_name = "Straight";
        return 4;
    }

    // three of a kind
    if (max_same_number(h) == 3)
    {
        *hand_name = "Three of a Kind";
        return 3;
    }

    // two pair
    if (is_two_pair(h))
    {
        *hand_name = "Two Pair";
        return 2;
    }

    // jacks or better
    if (is_jacks_or_better(h))
    {
        *hand_name = "Jacks or Better";
        return 1;
    }

    *hand_name = "Nothing";
    return 0;
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

void drawInfo()
{
    sprite.setTextDatum(top_left);
    sprite.setTextSize(1.5);
    sprite.drawString("Jacks or better", 5, 5);
    sprite.drawString("Credit: ", 5, 120);
    sprite.drawString(String(credit), 68, 120);
}

void drawCardBack(int pos)
{
    int x = 5 + pos * (42 + 5);
    sprite.drawRect(x, cardY, 42, 60, TFT_WHITE);
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
