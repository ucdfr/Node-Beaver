#ifndef USB_MANAGER_H
#define USB_MANAGER_H

#include <project.h>
#include <stdio.h>
#include "data.h"
#include "can_manager.h"


void usb_init();
void usb_put(const DataPacket* data_queue, const uint8_t data_pos);



#endif
