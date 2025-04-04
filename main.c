
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <errno.h>

// I2C cihazý ve LCD adresi
#define I2C_DEV "/dev/i2c-2"
#define LCD_ADDR 0x27

// LCD komutlarý
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME   0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_FUNCTIONSET  0x20

// LCD modlari
#define LCD_BACKLIGHT 0x08
#define ENABLE_BIT    0x04

// Fonksiyon prototipleri
void lcd_write_nibble(int fd, uint8_t nibble, uint8_t mode);
void lcd_send_byte(int fd, uint8_t data, uint8_t mode);
void lcd_send_cmd(int fd, uint8_t cmd);
void lcd_send_data(int fd, uint8_t data);
void lcd_init(int fd);
void lcd_clear(int fd);
void lcd_set_cursor(int fd, int line, int col);
void lcd_print(int fd, const char *str);
int read_adc_value(void);

// LCD’ye nibble (4 bit) yazma: Enable sinyalini tetikleyerek veriyi gönderir.
void lcd_write_nibble(int fd, uint8_t nibble, uint8_t mode) {
    uint8_t data = nibble | mode | LCD_BACKLIGHT;
    // Ýlk yazým
    if (write(fd, &data, 1) != 1) {
        perror("I2C: nibble write error");
    }
    usleep(500);
    // Enable'ý aktif et
    data |= ENABLE_BIT;
    if (write(fd, &data, 1) != 1) {
        perror("I2C: nibble write error");
    }
    usleep(500);
    // Enable'ý pasif yap
    data &= ~ENABLE_BIT;
    if (write(fd, &data, 1) != 1) {
        perror("I2C: nibble write error");
    }
    usleep(500);
}

// Veriyi önce yüksek nibble, sonra düþük nibble olarak gönderir.
void lcd_send_byte(int fd, uint8_t data, uint8_t mode) {
    uint8_t high_nibble = data & 0xF0;
    uint8_t low_nibble = (data << 4) & 0xF0;
    lcd_write_nibble(fd, high_nibble, mode);
    lcd_write_nibble(fd, low_nibble, mode);
}

void lcd_send_cmd(int fd, uint8_t cmd) {
    lcd_send_byte(fd, cmd, 0);
}

void lcd_send_data(int fd, uint8_t data) {
    lcd_send_byte(fd, data, 1);
}

// LCD baþlatma dizisi: 4-bit mod ayarlamasý ve temel komutlar
void lcd_init(int fd) {
    usleep(50000); // Güç uygulandýktan sonra bekle (>40ms)
    
    // Function set: 8-bit mod komutlarýný üç kez gönder
    lcd_write_nibble(fd, 0x30, 0);
    usleep(4500);
    lcd_write_nibble(fd, 0x30, 0);
    usleep(4500);
    lcd_write_nibble(fd, 0x30, 0);
    usleep(150);
    
    // 4-bit mode'a geçiþ
    lcd_write_nibble(fd, 0x20, 0);
    
    // Artýk tam komutlar gönderilebilir:
    lcd_send_cmd(fd, LCD_FUNCTIONSET | 0x08);      // 4-bit, 2 satýr, 5x8 nokta
    lcd_send_cmd(fd, LCD_DISPLAYCONTROL | 0x04);     // Ekran açýk, imleç kapalý
    lcd_clear(fd);
    lcd_send_cmd(fd, LCD_ENTRYMODESET | 0x02);       // Artan adres modunda
}

void lcd_clear(int fd) {
    lcd_send_cmd(fd, LCD_CLEARDISPLAY);
    usleep(2000);
}

void lcd_set_cursor(int fd, int line, int col) {
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_send_cmd(fd, 0x80 | (col + row_offsets[line]));
}

void lcd_print(int fd, const char *str) {
    while (*str) {
        lcd_send_data(fd, *str++);
    }
}

// ADC okumasý: sysfs üzerinden analog deðeri alýr (AIN0)
int read_adc_value(void) {
    FILE *fp = fopen("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", "r");
    if (!fp) {
        perror("ADC sysfs acilamadi");
        return -1;
    }
    int value;
    fscanf(fp, "%d", &value);
    fclose(fp);
    return value;
}

int main(void) {
    int i2c_fd;
    // I2C cihazýný aç
    if ((i2c_fd = open(I2C_DEV, O_RDWR)) < 0) {
        perror("I2C cihazý açýlamadý");
        exit(1);
    }
    // LCD'nin I2C slave adresini ayarla
    if (ioctl(i2c_fd, I2C_SLAVE, LCD_ADDR) < 0) {
        perror("I2C slave ayarlanamadý");
        close(i2c_fd);
        exit(1);
    }
    
    // LCD'yi baþlat
    lcd_init(i2c_fd);
    lcd_clear(i2c_fd);
    
    while (1) {
        int adc_raw = read_adc_value();
        if (adc_raw < 0) {
            break;
        }
        /*  
         * ADC deðeri 12-bit (0 - 4095) olup, referans 1.8V'dir.
         * Gerilim = (adc_raw / 4095.0) * 1.8
         * LM35: 10mV/°C -> Sýcaklýk (°C) = Gerilim (V) * 100
         */
        double voltage = (adc_raw / 4095.0) * 1.8;
        double temp_c = voltage * 100.0;
        
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Temp: %.1f C", temp_c);
        
        lcd_clear(i2c_fd);
        lcd_set_cursor(i2c_fd, 0, 0);
        lcd_print(i2c_fd, buffer);
        
        sleep(1);
    }
    
    close(i2c_fd);
    return 0;
}

