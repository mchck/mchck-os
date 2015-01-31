enum {
	FTM_PS_DIV1   = 0,
	FTM_PS_DIV2   = 1,
	FTM_PS_DIV4   = 2,
	FTM_PS_DIV8   = 3,
	FTM_PS_DIV16  = 4,
	FTM_PS_DIV32  = 5,
	FTM_PS_DIV64  = 6,
	FTM_PS_DIV128 = 7
};

enum {
	FTM_CLKS_NONE	= 0,
	FTM_CLKS_SYSTEM = 1,
	FTM_CLKS_FIXED	= 2,
	FTM_CLKS_EXT	= 3
};

enum {
	FTM_FAULTM_DISABLED    = 0,
	FTM_FAULTM_EVEN_MANUAL = 1,
	FTM_FAULTM_ALL_MANUAL  = 2,
	FTM_FAULTM_ALL_AUTO    = 1
};

enum {
	FTM_DTPS_DIV1  = 0, /* both 0 and 1 cause a divisor of one */
	FTM_DTPS_DIV4  = 2,
	FTM_DTPS_DIV16 = 3
};

enum {
	/* XXX better names */
	FTM_BDMMODE_00,
	FTM_BDMMODE_01,
	FTM_BDMMODE_10,
	FTM_BDMMODE_11
};
