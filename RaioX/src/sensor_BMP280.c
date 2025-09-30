#include "sensor_BMP280.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <math.h>

#define I2C_DEVICE       "/dev/i2c-1"
#define BMP280_ADDR      0x76

static int fd;
static uint16_t dig_T1;
static int32_t  dig_T2, dig_T3;
static uint16_t dig_P1;
static int32_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
static int64_t  t_fine;

static uint16_t read16(uint8_t msb_reg) {
    uint8_t buf[2];
    if (read(fd, buf, 2) != 2) return 0;
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

int sensor_temperatura_init(void) {
    // Abre o barramento I2C
    if ((fd = open(I2C_DEVICE, O_RDWR)) < 0) {
        perror("Failed to open I2C device");
        return -1;
    }
    if (ioctl(fd, I2C_SLAVE, BMP280_ADDR) < 0) {
        perror("Failed to set I2C address");
        close(fd);
        return -1;
    }
    // Le calibração de 24 bytes a partir de 0x88
    uint8_t reg = 0x88;
    if (write(fd, &reg, 1) != 1) {
        perror("I2C write error");
        return -1;
    }
    uint8_t calib[24];
    if (read(fd, calib, 24) != 24) {
        perror("I2C read calibration error");
        return -1;
    }
    // Converte calibrações (little-endian)
    dig_T1 = (uint16_t)(calib[1] << 8 | calib[0]);
    dig_T2 = (int16_t)(calib[3] << 8 | calib[2]);
    dig_T3 = (int16_t)(calib[5] << 8 | calib[4]);
    dig_P1 = (uint16_t)(calib[7] << 8 | calib[6]);
    dig_P2 = (int16_t)(calib[9] << 8 | calib[8]);
    dig_P3 = (int16_t)(calib[11] << 8 | calib[10]);
    dig_P4 = (int16_t)(calib[13] << 8 | calib[12]);
    dig_P5 = (int16_t)(calib[15] << 8 | calib[14]);
    dig_P6 = (int16_t)(calib[17] << 8 | calib[16]);
    dig_P7 = (int16_t)(calib[19] << 8 | calib[18]);
    dig_P8 = (int16_t)(calib[21] << 8 | calib[20]);
    dig_P9 = (int16_t)(calib[23] << 8 | calib[22]);
    printf("[BMP280] calibragem: T1=%u, T2=%d, T3=%d\n", dig_T1, dig_T2, dig_T3);
    // Configura registro de controle e config
    uint8_t cfg[2];
    cfg[0] = 0xF4; cfg[1] = 0x27; // oversampling x1, normal
    write(fd, cfg, 2);
    cfg[0] = 0xF5; cfg[1] = 0xA0; // standby 1000ms
    write(fd, cfg, 2);
    sleep(1);
    return fd;
}

double sensor_temperatura_read(void) {
    uint8_t reg = 0xF7;
    if (write(fd, &reg, 1) != 1) return NAN;
    uint8_t buf[8];
    if (read(fd, buf, 8) != 8) return NAN;
    int32_t adc_t = ((int32_t)buf[3] << 12) | ((int32_t)buf[4] << 4) | (buf[5] >> 4);
    double var1 = (((double)adc_t) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
    double var2 = ((((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0) * (((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0)) * ((double)dig_T3);
    t_fine = (int64_t)(var1 + var2);
    double cTemp = (var1 + var2) / 5120.0;
    return cTemp;
}

double sensor_pressao_read(void) {
    uint8_t reg = 0xF7;
    if (write(fd, &reg, 1) != 1) return NAN;
    uint8_t buf[8];
    if (read(fd, buf, 8) != 8) return NAN;
    int32_t adc_p = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);
    double var1 = ((double)t_fine / 2.0) - 64000.0;
    double var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
    var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
    if (var1 == 0.0) return NAN;
    double p = 1048576.0 - (double)adc_p;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)dig_P8) / 32768.0;
    double pressure = (p + (var1 + var2 + ((double)dig_P7)) / 16.0) / 100.0;
    return pressure;
}
