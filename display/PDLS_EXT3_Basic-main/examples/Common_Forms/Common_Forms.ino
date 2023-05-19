///
/// @file Common_Forms.ino
/// @brief Example of features for basic edition
///
/// @details Project Pervasive Displays Library Suite
/// @n Based on highView technology
///
/// @author Rei Vilo
/// @date 20 Mar 2023
/// @version 607
///
/// @copyright (c) Rei Vilo, 2010-2023
/// @copyright Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
///
/// @see ReadMe.txt for references
/// @n
///

// SDK
#if defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#else // Arduino general
#include "Arduino.h"
#endif // SDK

// Set parameters

// Include application, user and local libraries
#include "SPI.h"
#include "PDLS_EXT3_Basic.h"

// Define structures and classes

// Define variables and constants
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_154, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_213, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_266, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_271, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_287, boardRaspberryPiPico_RP2040);
Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_370, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_417, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_437, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_565, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_581, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_741, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_969, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_B98, boardRaspberryPiPico_RP2040);

// Prototypes

// Utilities
///
/// @brief Wait with countdown
/// @param second duration, s
///
void wait(uint8_t second)
{
    for (uint8_t i = second; i > 0; i--)
    {
        Serial.print(formatString(" > %i  \r", i));
        delay(1000);
    }
    Serial.print("         \r");
}

// Functions
///
/// @brief Basic forms test screen
/// @param flag true = default = perform flush, otherwise no
///
/// @image html T2_FORMS.jpg
/// @image latex T2_FORMS.PDF width=10cm
///
void displayForms(bool flag = true)
{
    myScreen.setOrientation(7);

    uint16_t x = myScreen.screenSizeX();
    uint16_t y = myScreen.screenSizeY();
    uint16_t z = min(x, y);

    myScreen.setPenSolid(false);
    myScreen.dRectangle(0, 0, x, y, myColours.black);
    myScreen.dLine(0, 0, x, y, myColours.red);

    myScreen.setPenSolid(true);
    myScreen.circle(x / 3, y / 3, z / 4, myColours.grey);
    // myScreen.triangle(x*2/3, y/3, x/2, y*2/3, x*3/4, y*2/3-10, myColours.black);
    myScreen.triangle(x * 2 / 3, y / 3, x * 3 / 4, y * 2 / 3 - 10, x - 10, 10, myColours.black);
    myScreen.dRectangle(x / 3, y * 2 / 3, x / 3, y / 4, myColours.red);

    myScreen.flush();
}

// Add setup code
///
/// @brief Setup
///
void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println();
    Serial.println("=== " __FILE__);
    Serial.println("=== " __DATE__ " " __TIME__);
    Serial.println();

    Serial.print("begin... ");
    myScreen.begin();
    Serial.println(formatString("%s %ix%i", myScreen.WhoAmI().c_str(), myScreen.screenSizeX(), myScreen.screenSizeY()));

    Serial.print("Palettes... ");
    myScreen.clear();
    displayForms();
    wait(8);

    Serial.print("White... ");
    myScreen.clear();
    myScreen.flush();

    Serial.println("=== ");
    Serial.println();

#if defined(ARDUINO_ARCH_PIDUINO)
    exit(0);
#endif
}

// Add loop code
///
/// @brief Loop, empty
///
void loop()
{
    delay(1000);
}
