#include "Helper/Helper.h"

namespace Helper
{
    const int DEFAULT_ENUM_VALUE_LENGTH = 2;

    String GET_TWO_DIGIT_STRING(int i)
    {
        String output((char *)0);
        output.reserve(3);
        if (i < 10)
            output += "0";
        output += String(i);
        return output;
    };

    String GET_TWO_DIGIT_STRING(char *cstr)
    {
        return GET_TWO_DIGIT_STRING(atoi(cstr));
    };

    String GET_TWO_DIGIT_STRING(String str)
    {
        return GET_TWO_DIGIT_STRING(str.toInt());
    };

    String GEN_UUID()
    {
        String uuid((char *)0);
        uuid.reserve(8);

        randomSeed(millis());

        for (int i = 0; i < 5; ++i)
        {
            byte randVal = random(0, 36);
            char letter = randVal + 'a';
            if (randVal >= 26)
                letter = (randVal - 26) + '0';
            uuid += letter;
        }
        return uuid;
    };
}; // namespace Helper