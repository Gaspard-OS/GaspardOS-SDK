
section .text

bits 32


global gaspard_printf_term

gaspard_printf_term:
    mov eax, 50          ; numéro de syscall
    mov ebx, [esp+4]     ; argument 1 = pointeur vers la string
    int 0x80
    ret


global gaspard_printf_hex

gaspard_printf_hex:
    push ebp
    mov ebp, esp

    push ebx              ; EBX doit être préservé

    mov ebx, [ebp + 8]    ; 1er argument C
    mov eax, 51           ; syscall number
    int 0x80

    pop ebx
    pop ebp
    ret

global gaspard_printf_number

gaspard_printf_number:

    push ebp
    mov ebp, esp

    push ebx              ; EBX doit être préservé

    mov ebx, [ebp + 8]    ; 1er argument C
    mov eax, 52           ; syscall number
    int 0x80

    pop ebx
    pop ebp
    ret


global gaspard_printf_char

gaspard_printf_char:
    push ebp
    mov ebp, esp

    push ebx              ; EBX doit être préservé

    mov ebx, [ebp + 8]    ; 1er argument C
    mov eax, 53           ; syscall number
    int 0x80

    pop ebx
    pop ebp
    ret


global gaspardapi_init_network



gaspardapi_init_network:
    mov eax,54
    int 0x80
    ret

global gaspardapi_get_mac

gaspardapi_get_mac:
    mov eax, 55          ; numéro de syscall
    mov ebx, [esp+4]     ; argument 1 = pointeur vers la string
    int 0x80
    ret


global gaspardapi_alloc

gaspardapi_alloc:


    mov eax, 56          
    mov ebx, [esp+4]    ; size et eax renvoi le ptr
    int 0x80
    ret


global gaspardapi_free

gaspardapi_free:


    mov eax, 57          
    mov ebx, [esp+4]    ;
    int 0x80
    ret




global gaspardapi_send_sock

gaspardapi_send_sock:


    mov eax, 58          
    mov ebx, [esp+4]    ;
    mov ecx, [esp+8]    ;

    int 0x80
    ret

global gaspardapi_getstatus

gaspardapi_getstatus:

    mov eax,59
    int 0x80
    ret



global gaspardapi_getpacket

gaspardapi_getpacket:

    mov eax,60
    int 0x80
    ret


global gaspardapi_getlastkeyboard_key

gaspardapi_getlastkeyboard_key:


    mov eax,61
    int 0x80
    ret
global gaspardapi_init_video

gaspardapi_init_video:


    mov eax,62
    int 0x80
    ret


global gaspardapi_getlastmouse_pos

gaspardapi_getlastmouse_pos:


    mov eax,63
    int 0x80
    ret


global gaspardapi_movecursor

gaspardapi_movecursor:


    mov eax, 64         
    mov ebx, [esp+4]    ;
    mov ecx, [esp+8]    ;

    int 0x80
    ret



global gaspardapi_clearscreen

gaspardapi_clearscreen:


    mov eax,65
    int 0x80
    ret



global gaspardapi_demo_3d

gaspardapi_demo_3d:

    mov eax,66
    int 0x80
    ret

global gaspardapi_load_app

gaspardapi_load_app:

    push ebp
    mov ebp, esp

    mov eax, 67         
    mov ebx, [ebp+8]     

    int 0x80

    pop ebp
    ret


; gaspardapi_kill_app


global gaspardapi_stop_app

gaspardapi_stop_app:

    mov eax, 68         
    mov ebx, [esp+4]    ;

    int 0x80
    ret


global gaspardapi_put_pixel
gaspardapi_put_pixel:
    push ebp
    mov  ebp, esp

    mov eax, 69
    mov ebx, [ebp+8]     ; x
    mov ecx, [ebp+12]    ; y
    mov edx, [ebp+16]    ; color

    int 0x80

    pop ebp
    ret


global gaspardapi_send_ipc

gaspardapi_send_ipc:

    mov eax, 70         
    mov ebx, [esp+4]    ;

    int 0x80
    ret


global gaspardapi_receive_ipc
gaspardapi_receive_ipc:



    mov eax, 71         
    mov ebx, [esp+4]    ;

    int 0x80
    ret



global global_gmain

extern gmain

global_gmain: 

    call gmain
halt:
    jmp halt