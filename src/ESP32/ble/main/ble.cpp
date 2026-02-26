// ble_minimal.cpp
#include <cstring>
#include <cassert>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"

static const char *TAG = "BLE_MIN";
static const char *DEVICE_NAME = "InfoDrone";

// BLE advertising params — connectable, undirected
static esp_ble_adv_params_t adv_params = {
    .adv_int_min       = 0x20,
    .adv_int_max       = 0x40,
    .adv_type          = ADV_TYPE_IND,
    .own_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    .channel_map       = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// Command packet
typedef struct {
    int8_t signals[12];
} drone_cmd_t;

static QueueHandle_t control_queue;
static uint16_t service_handle, char_handle;
static esp_gatt_if_t g_gatts_if = 0;

// UUIDs
#define SERVICE_UUID 0xABF0
#define CHAR_UUID    0xABF1
#define GATTS_NUM_HANDLES 4

/* ========== FLIGHT TASK ========== */
void flight_task(void *arg) {
    drone_cmd_t cmd{};
    while (true) {
        if (xQueueReceive(control_queue, &cmd, portMAX_DELAY) == pdTRUE) {
            // Core 1 processing
            // --- FLIGHT LOGIC HERE ---
            int8_t m1 = cmd.signals[5];
            ESP_LOGI(TAG, " %d", m1);
        }
    }
}

/* ========== GATT CALLBACK ========== */
static void gatts_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    switch (event) {

    case ESP_GATTS_REG_EVT: {
        g_gatts_if = gatts_if;
        esp_ble_gap_set_device_name(DEVICE_NAME);

        static esp_ble_adv_data_t adv_data = {};
        adv_data.set_scan_rsp = false;
        adv_data.include_name = true;
        adv_data.include_txpower = true;
        adv_data.flag = ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT;

        esp_ble_gap_config_adv_data(&adv_data);

        static esp_gatt_srvc_id_t service_id = {};
        service_id.is_primary = true;
        service_id.id.inst_id = 0;
        service_id.id.uuid.len = ESP_UUID_LEN_16;
        service_id.id.uuid.uuid.uuid16 = SERVICE_UUID;

        esp_ble_gatts_create_service(gatts_if, &service_id, GATTS_NUM_HANDLES);
        break;
    }

    case ESP_GATTS_CREATE_EVT: {
        service_handle = param->create.service_handle;
        static esp_bt_uuid_t char_uuid = {};
        char_uuid.len = ESP_UUID_LEN_16;
        char_uuid.uuid.uuid16 = CHAR_UUID;
        printf("%x\n", char_uuid.uuid.uuid16);

        esp_ble_gatts_add_char(
            service_handle,
            &char_uuid,
            ESP_GATT_PERM_WRITE,
            ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_WRITE_NR,
            nullptr,
            nullptr
        );
        break;
    }

    case ESP_GATTS_ADD_CHAR_EVT: {
        char_handle = param->add_char.attr_handle;
        esp_ble_gatts_start_service(service_handle);
        esp_ble_gap_start_advertising(&adv_params);
        break;
    }

    case ESP_GATTS_WRITE_EVT: {
        if (!param->write.is_prep && param->write.len >= 12) {
            drone_cmd_t incoming{};
            memcpy(incoming.signals, param->write.value, 12);
            xQueueOverwrite(control_queue, &incoming); // overwrite previous if exists
        }
        break;
    }

    default: break;
    }
}

/* ========== GAP CALLBACK ========== */
static void gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&adv_params);
        break;
    default: break;
    }
}

/* ========== MAIN ========== */
extern "C" void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());

    // 1-item queue, no buffer
    control_queue = xQueueCreate(1, sizeof(drone_cmd_t));
    assert(control_queue);

    // Flight task pinned to core 1
    xTaskCreatePinnedToCore(flight_task, "flight_task", 4096, nullptr, 10, nullptr, 1);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_cb));
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_cb));
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(0));

    ESP_LOGI(TAG, "BLE minimal ready");
}
