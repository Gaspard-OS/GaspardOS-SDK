# the GaspardOS API's 

## prequesites to use GaspardOS API


For debian/ubuntu :

```
sudo apt install clang llvm nasm xorriso mtools qemu-system-x86 make
```

Only LLVM and CLANG no gcc.

## Get started : 


```
cd C
make SRC="Hello.c Hello2.c" all #for displaying very simple message
make SRC="Hello.c Hello2.c" run # run will open qemu
```

## Complex example (kernel driver developement)

Some examples have their own makefile rule, like pci
```
cd C
make pci # to compile
make pci-run # run qemu to test
```
This example show VendorId and DeviceId PCI ID.


The ISO output is always : YOURGaspardOS.iso.



