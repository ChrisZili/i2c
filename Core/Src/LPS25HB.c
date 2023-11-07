

#include "LPS25HB.h"
#include "math.h"


uint8_t LPS25HB_address = LPS25HB_DEVICE_ADDRESS_0;
uint16_t LPS25HB_PressureOffset;

uint8_t LPS25HB_read_byte(uint8_t reg_addr) {
	uint8_t data = 0;
	return *(master_read(&data, 1, reg_addr, LPS25HB_address, 0));
}

void LPS25HB_write_byte(uint8_t reg_addr, uint8_t value) {
	master_write(value, reg_addr, LPS25HB_address, 0);
}

void LPS25HB_pressure_calibration(void){
	LPS25HB_PressureOffset = LPS25HB_read_byte(LPS25HB_ADDRESS_RPDS_L) | (LPS25HB_read_byte(LPS25HB_ADDRESS_RPDS_H) << 8);
}

uint8_t LPS25HB_init(void) {

	uint8_t status = 0;

	if (LPS25HB_read_byte(LPS25HB_WHO_AM_I_ADDRESS) == LPS25HB_WHO_AM_I_VALUE) {
		status = 1;
	} else {
		LPS25HB_address = LPS25HB_DEVICE_ADDRESS_1;

		if (LPS25HB_read_byte(LPS25HB_WHO_AM_I_ADDRESS)
				== LPS25HB_WHO_AM_I_VALUE) {
			status = 1;
		} else {
			status = 0;
		}
	}

	uint8_t ctrl1 = 148;
	LPS25HB_write_byte(LPS25HB_ADDRESS_CTRL1, ctrl1);

	LPS25HB_pressure_calibration();

	return status;
}



float LPS25HB_get_pressure() {
	uint32_t preassure = LPS25HB_read_byte(LPS25HB_ADDRESS_PRESS_OUT_XL) | (LPS25HB_read_byte(LPS25HB_ADDRESS_PRESS_OUT_L) << 8) | (LPS25HB_read_byte(LPS25HB_ADDRESS_PRESS_OUT_H) << 16);


	return  preassure/4096.00;
}

float LPS25HB_get_altitude(float initial_preassure,float current_preasure,float temperature){
//	float ln = log(current_preasure/initial_preassure);
	float h = 0;
	float temp = temperature + 273.15;
	float ideal_gas_constant  = 8.314;
	float molar_mass = 0.029;
	float g_pull = 9.81;
	h = ((ideal_gas_constant * temp)/(g_pull * molar_mass)) * (1-(current_preasure/(initial_preassure))) ;
	return h;
}


