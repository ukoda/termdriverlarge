# Term Driver Large

This a modifed version of the jamesbowman [termdriver2](https://github.com/jamesbowman/termdriver2) which is for the [TermDriver 2](https://www.crowdsupply.com/excamera/termdriver-2).
That project uses a tiny LCD and, like other tools from [Excamera Labs](https://www.crowdsupply.com/excamera), is well worth investing in.

The tiny screen is hard to read with aging eyes so this project ports it to a larger [4" LCD](https://www.lcdwiki.com/4.0inch_SPI_Module_ST7796) with a higher resolution and easier to read fonts.

It should be noted no effort is being made to support conditional builds between the original display and the new display because:
* Sub pixel rendering will not work on the 4" LCD in landscape mode.
* Sub pixel rendering is less benifit for the planned larger font size.
* The new controller does not support the 4:4:4 RGB pixel format of the original code so this code will focus use 5:6:5 format.
* The 4:4:4 pixel format will be retained for the character buffer so the elements can still fit in an uint32_t but 5:6:5 will be used in pixel related code.
* The combination sub pixel handling and 4:4:4 using 1.5 bytes per pixel makes the original pixel rendering code 'intersting'.  This code will will be much simpler and based losely on some Ada Fruit code.

Several other additions have been added, or are likely to be added as I use it.  Currently implmented are:
* The USB descriptor has been changed since support for this hardware/software should not be directed to Excamera Labs.

## To do

* The Excamera Labs splash image will be removed as I assume they will not want their logo used on derived works.
* Change font files to suit the simpler encoding.
* Add support for multiple size fonts to trade off readablity with the amount of info on screen.
* May consider portrait mode support.
* The `go` script is probably broken by the changes made in `usb_descriptors.c`.
* CMake / make still have `td2` as the target.  This should probably be changed to `tdl`.

### Splash image changes

As mentioned above the intent is to remove the Excamera Labs splash image.  I will likley replace with the OSH logo or something generic.  Once image support is working it would be good if users could add their own prefered logo without git getting in the way.  The `__has_include` directive could be used so a repo file such as `default_logo.h` could be overridden with a local `custom_logo.h`.

To do this we would first add `custom_logo.h` to `.gitignore`, then add something like this where `custom_logo.h` would normally be included:

```
#if defined __has_include
#  if __has_include (<custom_logo.h>)
#    include <custom_logo.h>
#  else
#    include <default_logo.h>
#  endif
#else
#  include <default_logo.h>
#endif
```
