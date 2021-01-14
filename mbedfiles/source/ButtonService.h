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
#include <cstdio>
#include <string>
#include <iostream>
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include <cmath>
#include "ble/BLE.h"
#ifndef __BLE_BUTTON_SERVICE_H__
#define __BLE_BUTTON_SERVICE_H__
#define SCALE_MULTIPLIER    0.0004
using namespace std;
// PwmOut tone (D5);
class ButtonService {
public:
    const static uint16_t BUTTON_SERVICE_UUID              = 0xA000;
    const static uint16_t BUTTON_STATE_CHARACTERISTIC_UUID = 0xA001;
    const static uint16_t BUTTON_STATE_CHARACTERISTIC_UUID2 = 0xA002;
    bool buttonpressed = false;
    bool isSleeping = false;
    
    ButtonService(BLE &_ble, bool buttonPressedInitial,bool initialid=false) :
        ble(_ble), buttonState(BUTTON_STATE_CHARACTERISTIC_UUID, &buttonPressedInitial, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        buttonSleepState(BUTTON_STATE_CHARACTERISTIC_UUID2, &initialid, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)
    {
        GattCharacteristic *charTable[] = {&buttonState,&buttonSleepState};
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
        float pGyroDataXYZ[3] = {0};
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        BSP_GYRO_GetXYZ(pGyroDataXYZ);
        int x = ((int)(pDataXYZ[0]*SCALE_MULTIPLIER*1000)) ,y = ((int)(pDataXYZ[1]*SCALE_MULTIPLIER*1000)), z=((int)(pDataXYZ[0]*SCALE_MULTIPLIER*100));
        int xg = ((int)(pGyroDataXYZ[0]*SCALE_MULTIPLIER*1000)) ,yg = ((int)(pGyroDataXYZ[1]*SCALE_MULTIPLIER*1000)), zg=((int)(pGyroDataXYZ[0]*SCALE_MULTIPLIER*100));
        // printf("%d\n",xg);
        
        if(buttonpressed == true)
        {
            // printf("button pressed");
            // Tune(tone,Fa4,4);
            isSleeping=is_sleeping(x,y,z,xg,yg,zg);
            ble.gattServer().write(buttonSleepState.getValueHandle(), (uint8_t *)&isSleeping, sizeof(isSleeping));
        }
    }
    bool is_sleeping(int x,int y,int z, int xg, int yg, int zg){
        double theta = 0;
        
        if ((x^2+y^2+z^2)!=0)
        {
            printf("xg= %d\n",xg);
            printf("yg= %d\n",yg);
            printf("zg= %d\n",zg);
        }   
        if (abs(xg)>=13000||abs(yg)>=12000||abs(zg)>=12000)
            return true;
        return false;
    }
private:
    BLE                              &ble;
    ReadWriteGattCharacteristic<bool>  buttonState;
    ReadWriteGattCharacteristic<bool> buttonSleepState;
};

#endif /* #ifndef __BLE_BUTTON_SERVICE_H__ */
