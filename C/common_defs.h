#pragma once

/**
 * @defgroup GaspardOSAPI The GaspardOS API's
 * @{
 */


/// @brief Type that GaspardOS return for event
enum QueueType {
    NOP = 0,
    ETH_PACKET_RECV, // reserved for future use
    MOUSE,// reserved for future use
    KEYBOARD,// reserved for future use
    IPC,
    

};


/**  @}*/