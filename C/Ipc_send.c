#include "common_defs.h"
#include "gaspardos_user_x86.h"




void gmain() {


    gaspard_printf_term("gaspardos send message \n");


    uint32_t *buffer = gaspardapi_alloc(4096);

    if(!buffer) {

        gaspard_printf_term("echec alloc \n");

        while(1);
    }
    buffer[0] = 1;
    enum QueueType status = gaspardapi_getstatus();
    while(1) {


     int status_ipc =    gaspardapi_send_ipc(buffer);

     if(!status_ipc) {

        gaspard_printf_term("WOW AUCUN MESSAGE \n");

        break;
     }

    // gaspard_printf_term("status _ipc : \n");

   //  gaspard_printf_number(status_ipc);

    // gaspard_printf_term("\n");
    status = gaspardapi_getstatus();

    }



}