Aşağıda, BeagleBone Black üzerinde Linux çalışırken, LM35 gibi analog sıcaklık sensöründen ADC değerini okuyup, I²C üzerinden bağlı 16x2 LCD’ye sıcaklık bilgisini yazdıran örnek bir C kodu bulabilirsiniz. Bu örnek; sysfs arayüzüyle ADC okuma, i2c-dev arayüzüyle LCD kontrolü ve temel LCD komutları içermektedir.

Notlar:

ADC okuması için ADC sysfs dosyası (örn. /sys/bus/iio/devices/iio:device0/in_voltage0_raw) kullanılmıştır. BeagleBone Black’te ADC pinleri 12-bit çözünürlüktedir ve referans gerilimi genellikle 1.8V’dur.

LM35, 10 mV/°C ölçeklendirmeye sahiptir. Bu nedenle ADC’den elde edilen değerin voltaj karşılığı hesaplanıp, sıcaklık °C’ye çevrilir.

LCD, PCF8574 tabanlı I²C adaptör kullanılarak 4-bit modda çalıştırılmıştır. LCD’nin adresi örnekte 0x27 olarak ayarlanmıştır; donanımınıza göre değişebilir.

Kodda hata kontrolü ve gecikme (usleep/sleep) kullanılmıştır.

Açıklamalar
I2C LCD Sürücü İşlemleri:

LCD'ye veri göndermek için 4-bit modunda yüksek ve düşük nibbles gönderilir.

lcd_init() fonksiyonu, LCD’yi 4-bit moda almak ve temel yapılandırmaları yapmak için gerekli komutları gönderir.

ADC Okuma:

read_adc_value() fonksiyonu, sysfs üzerinden ADC değerini okur.

ADC değeri, LM35’in ürettiği gerilime çevrilir; LM35 10 mV/°C oranındadır.

Ana Döngü:

Sürekli olarak ADC değeri okunur, sıcaklık hesaplanır ve LCD’ye yazdırılır.

Projeyi derlemek için aşağıdaki komutu kullanabilirsiniz:


gcc -o temp_lcd_app temp_lcd_app.c

Ardından programı çalıştırmak için:


sudo ./temp_lcd_app