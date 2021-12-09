# ScheluderQG8

Implements a simple scheluder for a freescale microcontroller mc9s08qg8.

## Description

Uses a Round-Robin scheluder, to execute a list of "static" threads.

> This project is for "eduational" purposes, the microcontroller only
> haves 512 bytes of ram, so is no good enough to execute a lot of
> threads easly.

## Getting Started

Need to download the project, and add it to your CodeWarrior workspace, then add to your project paths the `Project_Headers` folders.
And add the `ScheluderLib.lib` from the compilation folder, usually `Debug` or `Release` to your project link libraries.

## Author

[Elemeants](https://github.com/Elemeants)

## Version History

* 1.0
    * Initial Release.

## License

This project is licensed under the MTI License - see the LICENSE.md file for details
