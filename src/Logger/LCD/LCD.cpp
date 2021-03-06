#include "Logger/LCD/LCD.h"

namespace LCD
{
    // -------------------------------------------
    // LCD PRIVATE VARIABLES
    // -------------------------------------------
    // lcd screen variable
    WROVER_KIT_LCD tft;

    // screen size
    uint16_t lcdScreenHeight = tft.height(); // 320
    uint16_t lcdScreenWidth = tft.width();   // 240

    // scroll control
    uint16_t scrollTopFixedArea = 0;    // top fixed area
    uint16_t scrollBottomFixedArea = 0; // bottom fixed area
    uint16_t yStart = scrollTopFixedArea;
    uint16_t w_tmp, h_tmp;
    int16_t x1_tmp, y1_tmp;
    uint16_t yArea = lcdScreenHeight - scrollTopFixedArea - scrollBottomFixedArea;
    int scrollPosY = scrollTopFixedArea; // keeps track of the cursor position
    int scrollPosX = -1;

    // -------------------------------------------
    // LCD PRIVATE METHODS
    // -------------------------------------------
    int LcdDoScroll(int lines, int wait)
    {
        int yTemp = yStart;
        scrollPosY = -1;
        for (int i = 0; i < lines; i++)
        {
            yStart++;
            if (yStart == lcdScreenHeight - scrollBottomFixedArea)
                yStart = scrollTopFixedArea;
            tft.scrollTo(yStart);
            delay(wait);
        }
        return yTemp;
    };

    int LcdScrollText(const char *str)
    {
        // retrieves last char in input string. if it is not newline char,
        // will print and additional newline after printing input

        // *note: print will overflow to top of screen if it exceeds screen width

        // char endChar = str[strlen(str) - 1];

        if (scrollPosY == -1)
            scrollPosY = tft.getCursorY();
        scrollPosX = tft.getCursorX();
        if (scrollPosY >= (lcdScreenHeight - scrollBottomFixedArea))
            scrollPosY = (scrollPosY % (lcdScreenHeight - scrollBottomFixedArea)) + scrollTopFixedArea;
        tft.getTextBounds(str, scrollPosX, scrollPosY, &x1_tmp, &y1_tmp, &w_tmp, &h_tmp);
        if (scrollPosX == 0)
            tft.fillRect(0, scrollPosY, lcdScreenWidth, h_tmp, WROVER_BLACK);
        else
            tft.fillRect(0, scrollPosY, w_tmp, h_tmp, WROVER_BLACK);
        tft.setCursor(scrollPosX, scrollPosY);
        LcdDoScroll(h_tmp, 5); // Scroll lines, 5ms per line
        tft.print(str);
        // if (endChar != '\n')
        //     tft.print("\n");
        scrollPosY = tft.getCursorY();
        return h_tmp;
    };

    void setLcdTextColorError()
    {
        tft.setTextColor(WROVER_RED);
    };

    void setLcdTextColorPrimary()
    {
        tft.setTextColor(WROVER_WHITE);
    };

    // -------------------------------------------
    // LCD PUBLIC METHODS
    // -------------------------------------------
    bool init()
    {
        tft.begin();
        tft.setRotation(0); // requried portrait mode for scrolling
        tft.setTextColor(WROVER_WHITE);

        // screen header
        tft.fillScreen(WROVER_BLUE);
        // tft.setTextSize(3);
        // tft.setCursor(85, 4);
        // tft.print(F("0000"));
        // tft.setTextSize(1);

        // start from top of screen
        tft.setCursor(0, scrollTopFixedArea);
        // setup scrollable area
        tft.setupScrollArea(scrollTopFixedArea, scrollBottomFixedArea);
        // clear screen
        tft.fillRect(0, scrollTopFixedArea, lcdScreenWidth, yArea, WROVER_BLACK);

        return true;
    };

    void display(String output)
    {
        if (!output.endsWith("\n"))
            output += '\n';
        LcdScrollText(output.c_str());
    };

    void displayError(String output){
        setLcdTextColorError();
        display(output);
        setLcdTextColorPrimary();
    };

}; // namespace LCD