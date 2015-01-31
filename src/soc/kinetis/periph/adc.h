enum {
        ADC_ADCH_TEMP = 0b11010,
        ADC_ADCH_BANDGAP = 0b11011,
        ADC_ADCH_VREFSH = 0b11101,
        ADC_ADCH_VREFSL = 0b11110,
        ADC_ADCH_DISABLED = 0b11111
};

enum {
        ADC_CLK_BUS = 0b00,
        ADC_CLK_BUS_HALF = 0b01,
        ADC_CLK_ALTCLK = 0b10,
        ADC_CLK_ADACK = 0b11
};

enum {
        ADC_BIT_8 = 0b00,
        ADC_BIT_12 = 0b01,
        ADC_BIT_10 = 0b10,
        ADC_BIT_16 = 0b11
};

enum {
        ADC_DIV_1 = 0b00,
        ADC_DIV_2 = 0b01,
        ADC_DIV_4 = 0b10,
        ADC_DIV_8 = 0b11
};

enum {
        ADC_SAMPLE_CLKS_24 = 0b00,
        ADC_SAMPLE_CLKS_16 = 0b01,
        ADC_SAMPLE_CLKS_10 = 0b10,
        ADC_SAMPLE_CLKS_6 = 0b11
};

enum {
        ADC_REF_DEFAULT = 0b00,
        ADC_REF_ALTERNATE = 0b01
};

enum {
        ADC_AVG_4 = 0b00,
        ADC_AVG_8 = 0b01,
        ADC_AVG_16 = 0b10,
        ADC_AVG_32 = 0b11
};
