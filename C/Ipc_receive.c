#include "common_defs.h"
#include "gaspardos_user_x86.h"




void gmain() {


    gaspard_printf_term("gaspardos receive message \n");


    uint32_t *buffer = gaspardapi_alloc(4096);

    if(!buffer) {

        gaspard_printf_term("echec alloc receive \n");

        while(1);
    }
   // ANCHOR : on traite que les event IPC pour évité la lecture inutile du buffer
    buffer[0] = 0xFFFFFFFF;
    while(1) {

        enum QueueType status = gaspardapi_getstatus();
        if(status != IPC) {
            continue;
        }

        gaspardapi_receive_ipc(buffer);

        if(buffer[0] != 0xFFFFFFFF) {

            gaspard_printf_term("message recu \n");
            buffer[0] = 0xFFFFFFFF;
        }

    }



}
