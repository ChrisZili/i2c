#include "HTS221.h"

uint8_t HTS221_address = HTS221_DEVICE_ADDRESS_0;
float HTS221_HumiditySlope;
float HTS221_HumidityZero;
float HTS221_TemperatureSlope;
float HTS221_TemperatureZero;

uint8_t HTS221_read_byte(uint8_t reg_addr) {
	uint8_t data = 0;
	return *(master_read(&data, 1, reg_addr, HTS221_address, 0));
}

void HTS221_write_byte(uint8_t reg_addr, uint8_t value) {
	master_write(value, reg_addr, HTS221_address, 0);
}

void HTS221_humidity_calibration(void) {
	uint8_t h0rH = HTS221_read_byte(HTS221_ADDRESS_H0_rH_x2);
	uint8_t h1rH = HTS221_read_byte(HTS221_ADDRESS_H1_rH_x2);

	int16_t h0t0Out = (int16_t) HTS221_read_byte(HTS221_ADDRESS_H0_T0_OUT_L) | (int16_t) (HTS221_read_byte(HTS221_ADDRESS_H0_T0_OUT_H) << 8);
	int16_t h1t0Out = (int16_t) HTS221_read_byte(HTS221_ADDRESS_H1_T0_OUT_L) | (int16_t) (HTS221_read_byte(HTS221_ADDRESS_H1_T0_OUT_H) << 8);

	HTS221_HumiditySlope = (h1rH - h0rH) / (2.0 * (h1t0Out - h0t0Out));
	HTS221_HumidityZero = (h0rH / 2.0) - HTS221_HumiditySlope * h0t0Out;
}

void HTS221_temperature_calibration(void) {
	uint16_t t0degC = HTS221_read_byte(HTS221_ADDRESS_T0_degC_x8) | ((HTS221_read_byte(HTS221_ADDRESS_T1_T0_MSB) & 0x03) << 8);
	uint16_t t1degC = HTS221_read_byte(HTS221_ADDRESS_T1_degC_x8) | ((HTS221_read_byte(HTS221_ADDRESS_T1_T0_MSB) & 0x0c) << 8);

	int16_t t0Out = (int16_t) HTS221_read_byte(HTS221_ADDRESS_T0_OUT_L) | (int16_t) (HTS221_read_byte(HTS221_ADDRESS_T0_OUT_H) << 8);
	int16_t t1Out = (int16_t) HTS221_read_byte(HTS221_ADDRESS_T1_OUT_L) | (int16_t) (HTS221_read_byte(HTS221_ADDRESS_T1_OUT_H) << 8);

	HTS221_TemperatureSlope = (t1degC - t0degC) / (8.0 * (t1Out - t0Out));
	HTS221_TemperatureZero = (t0degC / 8.0) - HTS221_TemperatureSlope * t0Out;
	LL_mDelay(100);
}

uint8_t HTS221_init(void) {

	uint8_t status = 0;

	if (HTS221_read_byte(HTS221_WHO_AM_I_ADDRESS) == HTS221_WHO_AM_I_VALUE) {
		status = 1;
	} else {
		HTS221_address = HTS221_DEVICE_ADDRESS_1;

		if (HTS221_read_byte(HTS221_WHO_AM_I_ADDRESS) == HTS221_WHO_AM_I_VALUE) {
			status = 1;
		} else {
			status = 0;
		}
	}
	//device init
	uint8_t ctrl_setting = 134;
	HTS221_write_byte(HTS221_ADDRESS_CTRL1, ctrl_setting);

	//get calibration data
	HTS221_humidity_calibration();
	HTS221_temperature_calibration();

	return status;
}


uint8_t HTS221_get_humidity() {
	int16_t h_out = (int16_t) HTS221_read_byte(HTS221_ADDRESS_HUM_OUT_L)
			| (int16_t) (HTS221_read_byte(HTS221_ADDRESS_HUM_OUT_H) << 8);

	return (h_out * HTS221_HumiditySlope + HTS221_HumidityZero);
}



float HTS221_get_temperature() {
	int16_t t_out = (int16_t) HTS221_read_byte(HTS221_ADDRESS_TEMP_OUT_L)
			| (int16_t) (HTS221_read_byte(HTS221_ADDRESS_TEMP_OUT_H) << 8);

	return (t_out * -HTS221_TemperatureSlope
			+ HTS221_TemperatureZero);
}
