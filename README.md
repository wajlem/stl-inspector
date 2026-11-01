# STL-Inspector

Lightweight program written in C to check exported 3D model .stl file

|  Done :green_circle:| Planned :red_circle:  |
|---------------------|-----------------------|
| Volume              | Center of Mass        |
| Trinangle Count     | File Validation       |
| Bounding Box        | Stability Check       |
| Surface Area        | Duplicate Triangles   |
| Watertight Check    | Thin Wall Detection   |
| Overhang Detection  | Makefile              |
| Weight Estimate     |
| Negative Volume     |
| Layer Count         |
| Command Options     |
| Export Report       |
| Mesh Density        |
| Header Info         |

## Build

- git clone https://github.com/wajlem/stl-inspector
- gcc -std=gnu99 src/stl-inspector.c -lm -o stl-inspector

## Usage

- ./stl-inspector path/to/model.stl

## Example output

$ ./stl-inspector path/to/torus.stl -water -header
-   Header: Torus build for test
-   Model is watertight
-   Volume is 58.376444
-   There are 2304 triangles
-   ...

## Command options
- -water "print out watertightness check"
- -header "print out firt 10 bytes of text"
- -export "print out all parameters in .txt"

## Limitations

- binary STL only
- volume is correct only for watertight models
- overhang does not distinguish surface lying on the print bed
