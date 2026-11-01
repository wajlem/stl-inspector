# STL-Inspector

Lightweight program written in C to check exported 3D model .stl file

|       To-Do List      |     Tracker    |
|-----------------------|----------------|
| Volume                | :green_circle: |
| Triangle Count        | :green_circle: |
| Bounding Box          | :green_circle: |
| Surface Area          | :green_circle: |
| Center of Mass        | :red_circle:   |
| Watertight Check      | :green_circle: |
| Overhang Detection    | :green_circle: |
| Weight Estimate       | :green_circle: |
| File Validation       | :red_circle:   |
| Negative Volume       | :green_circle: |
| Stability Check       | :red_circle:   |
| Layer Count           | :green_circle: |
| Duplicate Triangles   | :red_circle:   |
| Thin Wall Detection   | :red_circle:   |
| Command Options       | :green_circle: |
| Export Report         | :green_circle: |
| Makefile              | :red_circle:   |
| Mesh Density          | :green_circle: |
| Header Info           | :green_circle: |

## Build

- git clone https://github.com/wajlem/stl-inspector
- gcc -std=gnu99 src/script.c -lm -o script

## Usage

- ./script path/to/model.stl

## Example output

$ ./script path/to/torus.stl -water -header
-   Header: Torus build for test
-   Model is watertight
-   Volume is 58.376444
-   There are 2304 triangles
-   Lenght X: 8.000000
-   ...

## Command options
- -water
- -header
- -export

## Limitations

- binary STL only
- volume is correct only for watertight models
- overhang does not distinguish surface lying on the print bed
