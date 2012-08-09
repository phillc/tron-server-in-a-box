#!/bin/sh

# see http://www.imagemagick.org/script/index.php
convert -delay 5 -loop 0 *.bmp animate.gif
rm *.bmp
