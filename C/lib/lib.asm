
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


    push ebp
    mov ebp, esp

    mov eax, 56         
    mov ebx, [ebp+8]     

    int 0x80
    ; le result est dans eax
    pop ebp
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


global test_vmx
test_vmx:



    mov eax, 72         

    int 0x80
    ret

global gaspardapi_ide_test
gaspardapi_ide_test:



    mov eax, 73         
    mov ebx, [esp+4]    ;

    int 0x80
    ret
global gaspardapi_ide_iso9660_init
gaspardapi_ide_iso9660_init:



    mov eax, 74         
    mov ebx, [esp+4]    ;

    int 0x80
    ret
global gaspardapi_gui_start
gaspardapi_gui_start:



    mov eax, 75         

    int 0x80
    ret
global gaspardapi_exit
gaspardapi_exit:



    mov eax, 76         
    mov ebx, [esp+4]    ;

    int 0x80
    ret

global gaspardapi_erase
gaspardapi_erase:



    mov eax, 77         

    int 0x80
    ret

global gaspardapi_ide_list

gaspardapi_ide_list:
    mov eax, 78          ; numéro de syscall
    mov ebx, [esp+4]     ; argument 1 = pointeur vers la string
    int 0x80
    ret
global gaspardapi_info_print

gaspardapi_info_print:
    mov eax, 79          ; numéro de syscall
    mov ebx, [esp+4]     ; 
    int 0x80
    ret
global gaspardapi_ide_status

gaspardapi_ide_status:
    mov eax, 73          ; numéro de syscall
    mov ebx, [esp+4]     ; 
    int 0x80
    ret

global gaspardapi_clear_screen

gaspardapi_clear_screen:
    mov eax, 80          ; numéro de syscall
    mov ebx, [esp+4]     ; 
    int 0x80
    ret

global gaspardapi_windows_create
gaspardapi_windows_create:
    push ebp
    mov  ebp, esp

    mov eax, 81
    mov ebx, [ebp+8]     ; struct


    int 0x80

    pop ebp
    ret
global gaspardapi_windows_button
gaspardapi_windows_button:
    push ebp
    mov  ebp, esp

    mov eax, 82
    mov ebx, [ebp+8]     ; struct


    int 0x80

    pop ebp
    ret
    

global gaspardapi_windows_destroy

gaspardapi_windows_destroy:

    push ebp
    mov  ebp, esp

    mov eax, 84
    mov ebx, [ebp+8]     ; struct


    int 0x80

    pop ebp
    ret

global gaspardapi_windows_button_destroy

gaspardapi_windows_button_destroy:

    push ebp
    mov  ebp, esp

    mov eax, 83
    mov ebx, [ebp+8]     ; struct


    int 0x80

    pop ebp
    ret
global gaspardapi_windows_update

gaspardapi_windows_update:

    push ebp
    mov  ebp, esp

    mov eax, 84


    int 0x80

    pop ebp
    ret

global gaspardapi_windows_button_clicked

gaspardapi_windows_button_clicked:

    push ebp
    mov  ebp, esp

    mov eax, 86


    int 0x80

    pop ebp
    ret

global global_gmain

extern gmain

global_gmain: 

    call gmain
halt:
    jmp halt