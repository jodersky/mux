# μx
Mux is a real-time preemptive kernel for 8-bit AVR microcontrollers. It is designed in a modular, easily extensible way.

## Requirements
- avr binutils (compiler and avr c library)

## Usage
Mux is divided into three types of components:
1.  kernel (in "kernel"), self explanatory, contains kernel sources
2.  application (in "apps"), can call any kernel functions and represent the actual logic in a firmware build
3.  hardware modules ("in modules"), custom hardware modules, such as specifically connected circuit boards, that may be used by applications

Creating an application that uses mux as a kernel is easy, simply copy the template application and edit the various entry points (best described by looking at the template source code) to use your business code. Compile the application and kernel to a firmware image with `APP=<appname> make`. Upload `make upload`.

## Credits
Mux was designed by Jakob Odersky. Context switching is inspired by the [FreeRTOS](http://www.freertos.org/) kernel and some parts, for example linked lists, from the Linux kernel.

## License
Mux is distributed under the GNU General Public License (GPL).