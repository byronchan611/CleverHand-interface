# CleverHand-library
This is the CleverHand library, a library for the CleverHand interface.


## Serial protocol
Each request from the computer to the controller is a 8 bytes long frame. The first byte is always the command, the rest of the bytes depend on the command. 

### Read 'r' request
Request to read a register from a module attached to the controller. 
1. 'r': read commad
2. id : id of the module to address. **Note**: 0xFF is broadcast to all modules
3. reg: register to read
4. n  : number of bytes to read starting from reg
5. reserved: reserved for future use
6. reserved: reserved for future use
```wavedrom
{ signal: [
  { name: 'Tx', wave: 'x=====.xxxxxxx', data: ['r', 'id', 'reg', 'n','reserved'] },
  { name: 'Rx', wave: 'xxxxxxx=|==.|x', data: ['ts', 'len', 'val']},
  {                              node: '.......A.BC..D'}
],
    head: { text: 'Read command' },
    edge: [ 'A+B 8bytes', 'C+D len bytes' ]
}
```

### Write 'w' request
Request to write a register from a module attached to the controller. There is no response to this command.
1. 'w': write command
2. id : id of the module to address. **Note**: 0xFF is broadcast to all modules
3. reg: register to write
4. val: value to write
5. reserved: reserved for future use
6. reserved: reserved for future use
```wavedrom
{ signal: [
  { name: 'Tx', wave: 'x=====.x', data: ['w', 'id', 'reg', 'val','reserved'] }
],
    head: { text: 'Write command' },
}
```

### Number of modules 'n' request
Request to know the number of modules attached to the controller.
1. 'n': number of modules command
2. reserved: reserved for future use
3. reserved: reserved for future use
4. reserved: reserved for future use
5. reserved: reserved for future use
6. reserved: reserved for future use
```wavedrom
{ signal: [
  { name: 'Tx', wave: 'x==....xxxxx', data: ['n', 'reserved'] },
  { name: 'Rx', wave: 'xxxxxxx=|==x', data: ['ts', '1', 'nb']},
  {                              node: '.......A.B'}
],
    head: { text: 'Number of modules command' },
    edge: [ 'A+B 8bytes' ]
}
```

### Mirror 'm' request
Request to test the communication with the controller. The expected response is the same frame sent by the computer.
1. 'm': mirror command
2. v1 : value 1
3. v2 : value 2
4. v3 : value 3
5. reserved: reserved for future use
6. reserved: reserved for future use
```wavedrom
{ signal: [
  { name: 'Tx', wave: 'x=====.xxxxxxx', data: ['m', 'v1', 'v2', 'v3','reserved'] },
  { name: 'Rx', wave: 'xxxxxxx=|====x', data: ['ts', '3', 'v1', 'v2', 'v3']},
  {                              node: '.......A.B.C.D'}
],
    head: { text: 'Mirror command' },
    edge: [ 'A+B 8bytes' ]
}
```

### Version 'v' request
Request to know the version of the controller.
1. 'v': version command
2. reserved: reserved for future use
3. reserved: reserved for future use
4. reserved: reserved for future use
5. reserved: reserved for future use
6. reserved: reserved for future use
```wavedrom
{ signal: [
  { name: 'Tx', wave: 'x==....xxxxxx', data: ['v', 'reserved'] },
  { name: 'Rx', wave: 'xxxxxxx=|===x', data: ['ts', '2', 'V.M', 'V.m']},
  {                              node: '.......A.BCDE'}
],
    head: { text: 'Version command' },
    edge: [ 'A+B 8bytes', 'C+D Major', 'D+E Minor' ]
}
```

## Cpp library
The library is written in C++ and is composed of a namespace `ClvHd` and two classes `Controller` and `Module`. The `Controller` class is used to communicate with a cleverhand controller. The controller class contains a list of `Module` objects. The `Module` class is used to represent the possible modules and their registers. As there is several types of modules, the `Module` class is an abstract class and each module type is represented by a derived class.

### Controller class
```cpp
class Controller 
{
    public:
      void readRegister(uint8_t id, uint8_t reg, uint8_t n, uint8_t* data);
      void writeRegister(uint8_t id, uint8_t reg, uint8_t val);
      uint8_t getNbModules();
      bool testCommunication();
    private:
      uint16_t _sendCommand(uint8_t* cmd, uint8_t* resp) 
}
```

### Building the library
#### Linux
#### Requirements
- CMake
- Make
- build-essential

#### Building
```bash
mkdir build
cd build
cmake ..
make
```

### Windows
#### Requirements
- CMake

#### Building     
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
