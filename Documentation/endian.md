# Endianness  

The order in which a computer reads bytes stored in memory. This can be from left to right (like English) or right to left (Arabic). This really only matters when we handle multibyte data values.  

## Endianness in RISC-V  

RISC-V is natively a little-endian architecture. However, the RISC-V specification has evolved to define bi-endian capability, which means the computer can support big-endian as well (under certain conditions).  

## Little Endian 

Defined as the byter order where the least significant byte ('little end') of a multibyte data value is stored at the lowest memory address.  

## Big Endian  

Defined as the byte order where the most significant bit ('big end') of a multibyte data value is stored at the lowestmemory address.  

It is to be noted that the bit ordering stays the same. There is no difference between the ordering of bits. Only difference is the ordering of bytes.  

### Example

Look at the following C-code:  
```
#include <stdio.h>
int main()
{
   unsigned int i = 1;
   char *c = (char*)&i;
   if (*c)   
       printf("Little endian");
   else
       printf("Big endian");
   getchar();
   return 0;
}

```  

The snippet of code above, will tell us if our machine is little endian or big endian.  

We know 1 in binary is 0000 0000 0000 0000 0000 0000 0000 0001 (int i = 1;).  
When we try to fetch one byte of i, either we will fetch the MSB or LSB, so 0000 (0)  or 0001 (1). If we retrive 
1 that means we fetched the LSB, at the lowest address of where the integer is stored at. This matches the little endian definition, and using the same logic if we fetch 0, the machine is big endian.



