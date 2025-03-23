#include "handle_sensor.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "global_params.h"

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));

    printf("✅ I2C driver installerad!\n");
}



// 📌 Skriver till ett register
void write_register(uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

// 📌 Läser 6 byte från MPU6050 (Accelerometer eller Gyroskop)
void read_mpu6050_data(uint8_t start_reg, int16_t *data) {
    uint8_t raw_data[6];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, start_reg, true);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, raw_data, 6, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    // Konvertera från två bytes till en 16-bitars signed integer
    for (int i = 0; i < 3; i++) {
        data[i] = (int16_t)((raw_data[i * 2] << 8) | raw_data[i * 2 + 1]);
    }
}

// 📌 Kalibreringsfunktion för gyroskopet
// Funktionen samlar in ett antal mätvärden och beräknar medelvärdet (bias) för varje axel.
// Dessa bias används sedan för att korrigera de råa gyromätningarna.
void calibrate_gyro(float *bias_x, float *bias_y, float *bias_z) {
    const int samples = 1000; // Antal samples för kalibreringen
    int32_t sum_x = 0, sum_y = 0, sum_z = 0;
    int16_t gyro_raw[3];

    // Samla in 'samples' antal mätningar
    for (int i = 0; i < samples; i++) {
        // Läser gyroskopdata (start_reg 0x43 är registeradressen för gyroskopdata)
        read_mpu6050_data(0x43, gyro_raw);
        // Ackumulera mätvärdena
        sum_x += gyro_raw[0];
        sum_y += gyro_raw[1];
        sum_z += gyro_raw[2];
        // Kort fördröjning mellan varje mätning
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    
    // Konvertera medelvärdet till grader per sekund (131 är sensitiviteten för ±250°/s)
    *bias_x = (sum_x / (float)samples) / 131.0;
    *bias_y = (sum_y / (float)samples) / 131.0;
    *bias_z = (sum_z / (float)samples) / 131.0;

    printf("Gyro kalibrerad: bias_x=%.2f, bias_y=%.2f, bias_z=%.2f\n", *bias_x, *bias_y, *bias_z);
}

void handle_sensor_task(void* params) {
    i2c_master_init();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Aktivera MPU6050 genom att skriva 0x00 till PWR_MGMT_1 (default är sleep mode)
    write_register(PWR_MGMT_1, 0x00);
    printf("🔋 MPU6050 aktiverad!\n");

    int16_t accel[3], gyro[3];

    task_params_t *task_params = (task_params_t*) params;

    float bias_x = 0, bias_y = 0, bias_z = 0;

    // Kalibrera gyroskopet
    printf("🔧 Kalibrerar gyroskop...\n");
    calibrate_gyro(&bias_x, &bias_y, &bias_z);
    printf("🔧 Gyro kalibrerad!\n");

    while (1) {
        // Läs accelerometerdata
        
        // read_mpu6050_data(ACCEL_XOUT_H, accel);
        // float ax = accel[0] / 16384.0;  // Skala till g-krafter
        // float ay = accel[1] / 16384.0;
        // float az = accel[2] / 16384.0;

        // Läs gyroskopdata
        read_mpu6050_data(0x43, gyro);
        float gx = gyro[0] / 131.0;  // Skala till grader per sekund
        float gy = gyro[1] / 131.0;
        float gz = gyro[2] / 131.0;


        
        // Korrigera mätvärdena genom att subtrahera bias från kalibreringen
        gx -= bias_x;
        gy -= bias_y;
        gz -= bias_z;
        
        // Skicka data till UDP-tasken
        sensor_payload_t sensor_data = {
            .type = 1,
            .player_id = 1, // TEMPORARY
            .gyro_x = gx,
            .gyro_y = gy,
            .gyro_z = gz
        };

        

        //vänta tills kön är tom innan vi skickar nästa data
        //Vi väntar kort tid för att skulle den vara full vill vi hämta NY data och inte sitta på gammal data
        xQueueSend(task_params->sensor_data_queue, &sensor_data, pdMS_TO_TICKS(50));
    }
}