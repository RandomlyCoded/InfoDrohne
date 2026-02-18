//TODO clean this shit up
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_timer.h"

static const char *DEVICE_NAME = "ESP_Drone";
static const char *TAG = "DRONE_CORE";

// Structure to hold our 12 signals
typedef struct {
    int8_t signals[12];
} drone_cmd_t;


// Queue handle
static QueueHandle_t xControlQueue = NULL;


// FLIGHT CONTROL ON CORE 1
void flight_control_task(void *pvParameters) {
	// one binary param is send, unpacking it into 12 signals with the following mappings
	//
	//
	// MAPPING
	//
	// Flight	
	// signals[0]: bool start(1)/stop(0)
	//
	// signals[1]: x s/t^2
	// signals[2]: y s/t^2
	// signals[3]: z s/t^2
	// signals[4]: x-rotate
	// signals[5]: y-rotate
	// signals[6]: z-rotate
	//
	//
	// DEBUG
	// signals[8]: speed motor 1
	// signals[9]: speed motor 2
	// signals[10]: speed motor 3
	// signals[11]: speed motor 4
    drone_cmd_t received_cmd;
    uint32_t count = 0;
    int64_t last_log_time = esp_timer_get_time();
    
    while (1) {
        // Wait indefinitely for a new packet from the queue
        if (xQueueReceive(xControlQueue, &received_cmd, portMAX_DELAY)) {
		count++;
            
            // --- FUNKY CALCULATIONS HERE ---
            // Example: Accessing your mappings

	    //for (int i = 0; i < 13; i++){
		//ESP_LOGI(TAG, "signal %d: %d", i, received_cmd.signals[i]);
		//}
		int64_t now = esp_timer_get_time();
            	if (now - last_log_time >= 1000000) {
                	printf("Actual PPS Received on Core 1: %ld\n", count);
                	count = 0;
                	last_log_time = now;
            	}
	    
        }
    }
}



// This is the core callback for SPP events
// BLUETOOTH CALLBACK ON CORE 0
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
	// make calls as fast as possible here
    switch (event) {
	case ESP_SPP_INIT_EVT: {
	    ESP_LOGI(TAG, "SPP Initialized. Starting server...");
	    // Starts the acceptor (listener)
	    esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, DEVICE_NAME);
	    break;
	}

	case ESP_SPP_SRV_OPEN_EVT: {
	    ESP_LOGI(TAG, "Client connected! Handle: %lu", param->srv_open.handle);
	    esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
	    break;

	}

	case ESP_SPP_CLOSE_EVT: {
	    ESP_LOGI(TAG, "Connection closed.");
	esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
	    break;
	}

	case ESP_SPP_DATA_IND_EVT: {
            // validation (Header + 12 signals)
            if (param->data_ind.len >= 13) {
                drone_cmd_t incoming;
                
                // Skip the header byte [0], copy the 12 signals
                memcpy(incoming.signals, &param->data_ind.data[1], 12);

                // Overwrite ensures we always have 
                // the most recent command with zero queue lag.
                xQueueOverwrite(xControlQueue, &incoming);
            }
            break;
        }

	default:
	    break;
    }
}

// Handles pairing and discovery events
static void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    if (event == ESP_BT_GAP_AUTH_CMPL_EVT) {
	if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
	    ESP_LOGI(TAG, "Authentication success: %s", param->auth_cmpl.device_name);
	}
    }
}


extern "C" void app_main(void) {
    // 1. Initialize NVS (Bluetooth needs it for pairing keys)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	ESP_ERROR_CHECK(nvs_flash_erase());
	ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    xControlQueue = xQueueCreate(1, sizeof(drone_cmd_t));
    if (xControlQueue == NULL) {
        ESP_LOGE(TAG, "Queue creation failed!");
        return;
    }

    xTaskCreatePinnedToCore(flight_control_task, "flight_task", 4096, NULL, 10, NULL, 1);

    // 2. Clear BLE memory (Classic BT only)
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    // 3. Setup BT Controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));

    // 4. Setup Bluedroid Host Stack
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    // 5. Register callbacks and Init SPP
    ESP_ERROR_CHECK(esp_bt_gap_register_callback(esp_bt_gap_cb));
    ESP_ERROR_CHECK(esp_spp_register_callback(esp_spp_cb));

    esp_spp_cfg_t spp_cfg = {
	.mode = ESP_SPP_MODE_CB, 
	.enable_l2cap_ertm = true, 
	.tx_buffer_size = 0 
    };
    ESP_ERROR_CHECK(esp_spp_enhanced_init(&spp_cfg));

    // 6. Make device visible to others
    esp_bt_gap_set_device_name(DEVICE_NAME);
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

    ESP_LOGI(TAG, "Acceptor is up and running.");
}
