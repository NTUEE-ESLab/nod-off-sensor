/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string>
#include <iostream>
#include "stm32l475e_iot01_accelero.h"
#include <cmath>
#ifndef __BLE_BUTTON_SERVICE_H__
#define __BLE_BUTTON_SERVICE_H__
#define SCALE_MULTIPLIER    0.0004
using namespace std;
class ButtonService {
public:
    const static uint16_t BUTTON_SERVICE_UUID              = 0xA000;
    const static uint16_t BUTTON_STATE_CHARACTERISTIC_UUID = 0xA001;
    const static uint16_t BUTTON_STATE_CHARACTERISTIC_UUID2 = 0xA002;
    bool buttonpressed = false;
    ButtonService(BLE &_ble, bool buttonPressedInitial,string initialid="no data") :
        ble(_ble), buttonState(BUTTON_STATE_CHARACTERISTIC_UUID, &buttonPressedInitial, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        buttonStrState(BUTTON_STATE_CHARACTERISTIC_UUID2, &initialid, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)
    {
        GattCharacteristic *charTable[] = {&buttonState,&buttonStrState};
        GattService         buttonService(ButtonService::BUTTON_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.gattServer().addService(buttonService);
    }
    int number = 0;
    void updateButtonState1(bool newState) {
        buttonpressed = true;
    }
    void updateButtonState2(bool newState) {
        // ble.gattServer().write(buttonState.getValueHandle(), (uint8_t *)&newState, sizeof(bool));
    }
    void start_acc(bool state) {
        int16_t pDataXYZ[3] = {1,1,1};
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        int x = ((int)(pDataXYZ[0]*SCALE_MULTIPLIER*1000)) ,y = ((int)(pDataXYZ[1]*SCALE_MULTIPLIER*1000)), z=((int)(pDataXYZ[0]*SCALE_MULTIPLIER*100));
        if(buttonpressed == true)
        {
            if(is_sleeping(x,y,z))
            {
                string x_str = std::to_string(x),y_str = std::to_string(y),z_str =std::to_string(z);
                string message=x_str;
                message += ",";
                message+=y_str;
                message+=",";
                message+=z_str;
                ble.gattServer().write(buttonStrState.getValueHandle(), (uint8_t *)&message, sizeof(message));
            }
            else
            {
                string message="fuck";
                ble.gattServer().write(buttonStrState.getValueHandle(), (uint8_t *)&message, sizeof(message));
            }
        }
       
        
    }
    bool is_sleeping(int x,int y,int z){
        // double theta = acos(x/sqrt(x^2+y^2+z^2));
        if (z>1)
            return true;
        return false;
    }
private:
    BLE                              &ble;
    ReadWriteGattCharacteristic<bool>  buttonState;
    ReadWriteGattCharacteristic<string> buttonStrState;
};

#endif /* #ifndef __BLE_BUTTON_SERVICE_H__ */
