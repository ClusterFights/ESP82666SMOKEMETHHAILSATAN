deps_config := \
	/home/pi/esp/esp-idf/components/app_trace/Kconfig \
	/home/pi/esp/esp-idf/components/aws_iot/Kconfig \
	/home/pi/esp/esp-idf/components/bt/Kconfig \
	/home/pi/esp/esp-idf/components/driver/Kconfig \
	/home/pi/esp/esp-idf/components/efuse/Kconfig \
	/home/pi/esp/esp-idf/components/esp32/Kconfig \
	/home/pi/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/pi/esp/esp-idf/components/esp_event/Kconfig \
	/home/pi/esp/esp-idf/components/esp_http_client/Kconfig \
	/home/pi/esp/esp-idf/components/esp_http_server/Kconfig \
	/home/pi/esp/esp-idf/components/esp_https_ota/Kconfig \
	/home/pi/esp/esp-idf/components/espcoredump/Kconfig \
	/home/pi/esp/esp-idf/components/ethernet/Kconfig \
	/home/pi/esp/esp-idf/components/fatfs/Kconfig \
	/home/pi/esp/esp-idf/components/freemodbus/Kconfig \
	/home/pi/esp/esp-idf/components/freertos/Kconfig \
	/home/pi/esp/esp-idf/components/heap/Kconfig \
	/home/pi/esp/esp-idf/components/libsodium/Kconfig \
	/home/pi/esp/esp-idf/components/log/Kconfig \
	/home/pi/esp/esp-idf/components/lwip/Kconfig \
	/home/pi/esp/esp-idf/components/mbedtls/Kconfig \
	/home/pi/esp/esp-idf/components/mdns/Kconfig \
	/home/pi/esp/esp-idf/components/mqtt/Kconfig \
	/home/pi/esp/esp-idf/components/nvs_flash/Kconfig \
	/home/pi/esp/esp-idf/components/openssl/Kconfig \
	/home/pi/esp/esp-idf/components/pthread/Kconfig \
	/home/pi/esp/esp-idf/components/spi_flash/Kconfig \
	/home/pi/esp/esp-idf/components/spiffs/Kconfig \
	/home/pi/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/pi/esp/esp-idf/components/unity/Kconfig \
	/home/pi/esp/esp-idf/components/vfs/Kconfig \
	/home/pi/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/pi/esp/esp-idf/components/app_update/Kconfig.projbuild \
	/home/pi/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/pi/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/pi/esp-idf-examples/protocols/mqtt/publish_test/main/Kconfig.projbuild \
	/home/pi/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/pi/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
