# STL-Inspector

Lightweight program written in C to check exported 3D model .stl file

|       To-Do List      |     Tracker    |
|-----------------------|----------------|
| Volume                | :green_circle: |
| Triangle Count        | :green_circle: |
| Bounding Box          | :green_circle: |
| Surface Area          | :green_circle: |
| Center of Mass        | :red_circle:   |
| Watertight Check      | :red_circle:   |
| Overhang Detection    | :green_circle: |
| Weight Estimate       | :red_circle:   |
| File Validation       | :red_circle:   |
| Negative Volume       | :red_circle:   |
| Stability Check       | :red_circle:   |
| Layer Count           | :red_circle:   |
| Duplicate Triangles   | :red_circle:   |
| Thin Wall Detection   | :red_circle:   |
| Command Options       | :red_circle:   |
| Export Report         | :red_circle:   |
| Makefile              | :red_circle:   |
| Mesh Density          | :red_circle:   |
| Header Info           | :red_circle:   |

## Build

- git clone https://github.com/wajlem/stl-inspector
- gcc -std=gnu99 src/script.c -lm -o script

## Usage

- ./script path/to/model.stl

## Example output

$ ./script tests/torus.stl
    Volume is 58.376444
    There are 2304 triangles
    Lenght X: 8.000000

## Limitations

- binary STL only (ASCII is not supported yet)
- volume is correct only for watertight models
- overhang does not distinguish surface lying on the print bed
