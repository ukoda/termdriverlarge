This a modifed version of the jamesbowman termdriver2, https://github.com/jamesbowman/termdriver2, which is for the TermDriver 2 https://www.crowdsupply.com/excamera/termdriver-2.
That project uses a tiny LCD and like other tools from Excamera Labs https://www.crowdsupply.com/excamera is well worth investing in.

The tiny screen is hard to read with aging eyes so this project ports it to a larger 4" LCD, https://www.lcdwiki.com/4.0inch_SPI_Module_ST7796, with a higher resolution and easier to read fonts.

It should be noted no effort is being made to support conditional builds between the original display and the new display because:
* Sub pixel rendering will not work on the 4" LCD in landscape mode.
* Sub pixel rendering should not be needed for the planned larger font size.
* The new controller does no support the 4:4:4 RGB pixel format of the original code so this code will focus use 5:6:5 format.
* The 4:4:4 pixel format will be retained for the character buffer so the elements can still fit in an uint32_t but 5:6:5 will be used in pixel related code.
* The combination sub pixel handling and 4:4:4 using 1.5 bytes per pixel makes the original pixel rendering code 'intersting'.  This code will will be much simpler and based losely on some Ada Fruit code.

Several other additions are likely to be added as I use it.

## To do
* Remove OpenSCAD files as these only work for the original Excamera Labs.  The folder will remain as I may make new version for the new hardware.
* The Excamera Labs splash image will be removed as I assume they will not want their logo used on derived works.
* Look at changing ``#define USBD_MANUFACTURER "Excamera"`` and ``#define USBD_PRODUCT "td2"`` if doesn't break things.
* Change the LCD driver code to support the ST7796s instead of the ST7789.
* Change font files to suit the simpler encoding.
* Add support for multiple size fonts to trade off readablity with the amount of info on screen.
* May consider portrait mode support.
