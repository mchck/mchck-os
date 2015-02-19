#include <mchck.h>

struct i2c_ctx {
	struct i2c_transaction *cur;

	size_t index;
	enum i2c_state {
		I2C_STATE_IDLE, I2C_STATE_TX, I2C_STATE_RX, I2C_STATE_RX_START
	} state;
};

static struct i2c_ctx ctx;

// set up the registers for a new transaction
static void
i2c_start_transaction(void)
{
	while (bf_get_reg(I2C0_S, I2C_S_BUSY))
		/* NOTHING */; // ensure STOP symbol has been sent
	ctx.index = 0;
	if (ctx.cur->direction == I2C_READ) {			// if this is a read transaction
		ctx.state = I2C_STATE_RX_START;
		I2C0_C1 =
			I2C_C1_IICEN_MASK |
			I2C_C1_IICIE_MASK |
			I2C_C1_MST_MASK |
			I2C_C1_TX_MASK |
			(ctx.cur->length == 1 ? I2C_C1_TXAK_MASK : 0);
		I2C0_D = (ctx.cur->address << 1) | 1;
	} else {
		ctx.state = I2C_STATE_TX;
		I2C0_C1 =
			I2C_C1_IICEN_MASK |
			I2C_C1_IICIE_MASK |
			I2C_C1_MST_MASK |
			I2C_C1_TX_MASK;
		I2C0_D = ctx.cur->address << 1;
	}
}

// end the current transaction
static void
i2c_end_transaction()
{
	// send STOP condition if requested or nothing left to send, otherwise repeat START condition
	if (ctx.cur->next == NULL || ctx.cur->stop == I2C_STOP)
		bf_set_reg(I2C0_C1, I2C_C1_MST, 0);
	else
		bf_set_reg(I2C0_C1, I2C_C1_RSTA, 1);
}

// start the next transaction
static void
i2c_transaction_next(enum i2c_result result)
{
	struct i2c_transaction *prev = ctx.cur;	// so we can do the callback last

	// if last transaction, go idle, otherwise start next transaction
	ctx.cur = ctx.cur->next;	// this is NULL if no next transaction
	if (ctx.cur == NULL)
		ctx.state = I2C_STATE_IDLE;
	else
		i2c_start_transaction();

	// invoke callback if specified
	if (prev->cb != NULL)
		prev->cb(result, prev);
}

void
I2C0_Handler(void)
{
	bf_set_reg(I2C0_S, I2C_S_IICIF, 1);
	enum i2c_result result = I2C_RESULT_SUCCESS;
	switch (ctx.state) {
	case I2C_STATE_TX:
		if (ctx.index < ctx.cur->length) {
			if (bf_get_reg(I2C0_S, I2C_S_RXAK)) {	// if not acked, consider the transaction done
				result = I2C_RESULT_NACK;
				i2c_end_transaction();
				i2c_transaction_next(result);
			} else {
				I2C0_D = ctx.cur->buffer[ctx.index++];	// transmit next byte
			}
		} else {
			if (bf_get_reg(I2C0_S, I2C_S_RXAK))	// if not acked, report it
				result = I2C_RESULT_NACK;
			i2c_end_transaction();
			i2c_transaction_next(result);
		}
		break;
	case I2C_STATE_RX_START:
		if (bf_get_reg(I2C0_S, I2C_S_RXAK)) {		// report premature nack
			result = I2C_RESULT_NACK;
			i2c_end_transaction();
			i2c_transaction_next(result);
		} else {
			bf_set_reg(I2C0_C1, I2C_C1_TX, 0);
			ctx.state = I2C_STATE_RX;
			// throw away the first byte read from the device.
			(void)I2C0_D;
		}
		break;
	case I2C_STATE_RX:
		if (ctx.index == ctx.cur->length-1) {		// last byte has been received
			i2c_end_transaction();
			ctx.cur->buffer[ctx.index++] = I2C0_D;
			i2c_transaction_next(result);
		} else {
			if (ctx.index == ctx.cur->length - 2)	// last-but-one byte has been received, send NACK with last byte
				bf_set_reg(I2C0_C1, I2C_C1_TXAK, 1);		// to signal to the device that we're done receiving
			ctx.cur->buffer[ctx.index++] = I2C0_D;
		}
		break;
	default:
		// XXX this shouldn't happen, assert or something
		break;
	}
}

void
i2c_init(enum i2c_rate rate)
{
	// Enable clocks for I2C and PORTB
	bf_set_reg(SIM_SCGC4, SIM_SCGC4_I2C0, 1);
	bf_set_reg(SIM_SCGC5, SIM_SCGC5_PORTB, 1);

	//                   I2C0_F values, indexed by enum i2c_rate.
	//                   100kHz 400   600   800   1000  1200  1500  2000  2400kHz
	static uint8_t f[] = {0x27, 0x85, 0x14, 0x45, 0x0D, 0x0B, 0x09, 0x02, 0x00};

	if (rate < 0 || rate >= sizeof(f))
		rate = I2C_RATE_100;
	I2C0_F = f[rate];

	bf_set_reg(I2C0_C1, I2C_C1_IICEN, 1);
	ctx.state = I2C_STATE_IDLE;
	ctx.cur = NULL;
	int_enable(IRQ_I2C0);
}

void
i2c_queue(struct i2c_transaction *transaction)
{
	crit_enter();
	transaction->next = NULL;		// make sure the new transaction has no leftover 'next' value, since the library handles queueing

	if (ctx.cur == NULL) {			// if no current transaction, start this one right away
		ctx.cur = transaction;
		i2c_start_transaction();
	} else {				// otherwise, go to end of queue and add transaction there
		struct i2c_transaction *it = ctx.cur;
		while (it->next != NULL)
			it = it->next;
		it->next = transaction;
	}
	crit_exit();
}
