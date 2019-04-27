To use, open a command prompt window and run `create_map/create_map.exe input_png.png output_folder`.

It will mkdir output_folder for you and within it create 2 files: heights.txt and walls.txt

Both files are whitespace delimited lists of decimal (base 10) numbers. The first two numbers are always the width and height, respectively.

Each element of heights.txt is two numbers: first the height (0 for bottom, 1 for ramp, 2 for second level) and next the ramp direction as a direction bitmask.

Each element of walls.txt is a direction bitmask.
