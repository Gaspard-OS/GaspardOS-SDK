# the GaspardOS API's 

## prequesites to use GaspardOS API


For debian/ubuntu :

```
sudo apt install clang llvm nasm xorriso mtools qemu-x86
```

Only LLVM and CLANG no gcc.

## Get started : 


```
cd C
make SRC="Hello.c Hello2.c" all #for displaying very simple message
make SRC="Hello.c Hello2.c" run # run will open qemu
```



The ISO output is always : YOURGaspardOS.iso.



