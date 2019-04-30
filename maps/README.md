If create_map.exe will not build because the lodepng folder is empty, it's because you didn't run `git submodule init` and `git submodule update`.

To use, open a command prompt window and run `create_map/create_map.exe input_png.png output_folder`.

It will mkdir output_folder for you and within it create several text files.

Both files are whitespace delimited lists of decimal (base 10) numbers. The first two numbers are always the width and height, respectively.

Each element of heights.txt is one of 3 numbers: 0 for bottom, 1 for ramp, 2 for second level.

The elements of walls.txt and ramps.txt are direction bitmasks.
