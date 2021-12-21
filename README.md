# big-unsigned-nums
This C++ header is a templated, relatively straightforward method to extend a 64-bit system's unsigned integer capabilities. It provides addition, subtraction, bitwise shifting, bitwise AND, bitwise XOR, bitwise OR, and multiplication with 64-bit unsigned integer multiplicands. It was designed using the <cstdint> library's built-in fast 64-, 16-, and 8-bit unsigned integers. 
  
## Overview
By including the file "erc-biguint.hpp" in your project, it enables you to use the class <CODE>ERCLIB::ExtendedUInt</CODE> as well as a few typedefs for specific integer types.

## Operations
As mentioned, this version supports addition (to both standard 64-bit uints and to ExtendedUints), subtraction (ditto), bitwise shifts, ANDs, XORs, and ORs; and multiplication by 64-bit uints. It also has a <CODE>print()</CODE> function to spit it out to stdout, and your typical increment/decrement operators. <B>However,</B> these operations exist solely as "apply to me" operators (<CODE>+=</CODE>, not <CODE>+</CODE>). This will be changed in a later version.
  
### Limitations
This first version's operators are limited to self-changing instead of inline. It also lacks division and cross ExtendedUInt multiplication, as well as comparison operators -- this is the tested proof-of-concept, currently, and not a polished prototype. Bear with me!
  
# Why?
Well, it was an idea I had while going through binary addition, etc., for a side project with Logisim. Worked on it on and off for a bit, and now we're here. It <I>may</I> also useful for RSA (it can easily support a 4096 bit uint)?
