#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_common_api.h"

static const char* TAG = "BLE128";

// 128-bit UUIDs
static const uint8_t SERVICE_UUID[16] = {
    0x69, 0xfc, 0x82, 0x56,
    0xd3, 0xca,
    0x91, 0x12,
    0xbc, 0xe0,
    0x38, 0x8d, 0xe2, 0x48, 0x60, 0xd3
};

static const uint8_t CHAR_UUID[16] = {
    0xeb, 0xc4, 0x9f, 0x11,
    0x9f, 0xfc,
    0x96, 0xb2,
    0x2a, 0x4e,
    0x69, 0x94, 0xa9, 0x91, 0x75, 0xb5
};

static uint16_t serviceHandle = 0;
static uint16_t charHandle = 0;

static uint8_t testValue[4] = {0xDE, 0xAD, 0xBE, 0xEF};

static void gapHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "Advertising data set");
            break;
        default:
            break;
    }
}

static void gattHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                        esp_ble_gatts_cb_param_t* param) {
    switch (event) {

        case ESP_GATTS_REG_EVT: {
            ESP_LOGI(TAG, "GATT registered");

            esp_ble_gap_set_device_name("ESP32-InfoDrone");

            esp_ble_adv_data_t adv = {};
            adv.include_name = true;
            adv.flag = ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT;
            esp_ble_gap_config_adv_data(&adv);

            esp_gatt_srvc_id_t sid = {};
            sid.is_primary = true;
            sid.id.inst_id = 0;
            sid.id.uuid.len = ESP_UUID_LEN_128;
            memcpy(sid.id.uuid.uuid.uuid128, SERVICE_UUID, 16);

            esp_ble_gatts_create_service(gatts_if, &sid, 4);
            break;
        }

        case ESP_GATTS_CREATE_EVT: {
            serviceHandle = param->create.service_handle;
            ESP_LOGI(TAG, "Service created");

            esp_bt_uuid_t char_uuid = {};
            char_uuid.len = ESP_UUID_LEN_128;
            memcpy(char_uuid.uuid.uuid128, CHAR_UUID, 16);

            esp_gatt_char_prop_t props = ESP_GATT_CHAR_PROP_BIT_READ;

            esp_ble_gatts_add_char(serviceHandle, &char_uuid,
                                   ESP_GATT_PERM_READ, props, nullptr, nullptr);

            esp_ble_gatts_start_service(serviceHandle);
            break;
        }

        case ESP_GATTS_ADD_CHAR_EVT:
            charHandle = param->add_char.attr_handle;
            ESP_LOGI(TAG, "Characteristic added");
            break;

        case ESP_GATTS_READ_EVT: {
            ESP_LOGI(TAG, "Read request");

            esp_gatt_rsp_t rsp = {};
            rsp.attr_value.handle = param->read.handle;
            rsp.attr_value.len = sizeof(testValue);
            memcpy(rsp.attr_value.value, testValue, sizeof(testValue));

            esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                                        param->read.trans_id, ESP_GATT_OK, &rsp);
            break;
        }

        default:
            break;
    }
}

extern "C" void app_main() {
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    esp_ble_gap_register_callback(gapHandler);
    esp_ble_gatts_register_callback(gattHandler);
    esp_ble_gatts_app_register(0);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

