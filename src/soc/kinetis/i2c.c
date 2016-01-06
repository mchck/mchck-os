#include <mchck.h>

struct i2c_ctx {
	I2C_Type * const i2c;
	struct i2c_transaction *cur;

	size_t index;
	enum i2c_state {
		I2C_STATE_IDLE, I2C_STATE_TX, I2C_STATE_RX, I2C_STATE_RX_START
	} state;
};

struct i2c_ctx i2c0_ctx = {I2C0};
struct i2c_ctx i2c1_ctx = {I2C1};

// set up the registers for a new transaction
static void
i2c_start_transaction(struct i2c_ctx *ctx)
{
	while (bf_get_reg(I2C_S_REG(ctx->i2c), I2C_S_BUSY))
		/* NOTHING */; // ensure STOP symbol has been sent
	ctx->index = 0;
	if (ctx->cur->direction == I2C_READ) {			// if this is a read transaction
		ctx->state = I2C_STATE_RX_START;
		I2C_C1_REG(ctx->i2c) =
			I2C_C1_IICEN_MASK |
			I2C_C1_IICIE_MASK |
			I2C_C1_MST_MASK |
			I2C_C1_TX_MASK |
			(ctx->cur->length == 1 ? I2C_C1_TXAK_MASK : 0);
		I2C_D_REG(ctx->i2c) = (ctx->cur->address << 1) | 1;
	} else {
		ctx->state = I2C_STATE_TX;
		I2C_C1_REG(ctx->i2c) =
			I2C_C1_IICEN_MASK |
			I2C_C1_IICIE_MASK |
			I2C_C1_MST_MASK |
			I2C_C1_TX_MASK;
		I2C_D_REG(ctx->i2c) = ctx->cur->address << 1;
	}
}

// end the current transaction
static void
i2c_end_transaction(struct i2c_ctx *ctx)
{
	// send STOP condition if requested or nothing left to send, otherwise repeat START condition
	if (ctx->cur->next == NULL || ctx->cur->stop == I2C_STOP)
		bf_set_reg(I2C_C1_REG(ctx->i2c), I2C_C1_MST, 0);
	else
		bf_set_reg(I2C_C1_REG(ctx->i2c), I2C_C1_RSTA, 1);
}

// start the next transaction
static void
i2c_transaction_next(struct i2c_ctx *ctx, enum i2c_result result)
{
	struct i2c_transaction *prev = ctx->cur;	// so we can do the callback last

	// if last transaction, go idle, otherwise start next transaction
	ctx->cur = ctx->cur->next;	// this is NULL if no next transaction
	if (ctx->cur == NULL)
		ctx->state = I2C_STATE_IDLE;
	else
		i2c_start_transaction(ctx);

	// invoke callback if specified
	if (prev->cb != NULL)
		prev->cb(result, prev);
}

static void
i2c_interrupt_handler(struct i2c_ctx *ctx)
{
	bf_set_reg(I2C_S_REG(ctx->i2c), I2C_S_IICIF, 1);
	enum i2c_result result = I2C_RESULT_SUCCESS;
	switch (ctx->state) {
	case I2C_STATE_TX:
		if (ctx->index < ctx->cur->length) {
			if (bf_get_reg(I2C_S_REG(ctx->i2c), I2C_S_RXAK)) {	// if not acked, consider the transaction done
				result = I2C_RESULT_NACK;
				i2c_end_transaction(ctx);
				i2c_transaction_next(ctx, result);
			} else {
				I2C_D_REG(ctx->i2c) = ctx->cur->buffer[ctx->index++];	// transmit next byte
			}
		} else {
			if (bf_get_reg(I2C_S_REG(ctx->i2c), I2C_S_RXAK))	// if not acked, report it
				result = I2C_RESULT_NACK;
			i2c_end_transaction(ctx);
			i2c_transaction_next(ctx, result);
		}
		break;
	case I2C_STATE_RX_START:
		if (bf_get_reg(I2C_S_REG(ctx->i2c), I2C_S_RXAK)) {		// report premature nack
			result = I2C_RESULT_NACK;
			i2c_end_transaction(ctx);
			i2c_transaction_next(ctx, result);
		} else {
			bf_set_reg(I2C_C1_REG(ctx->i2c), I2C_C1_TX, 0);
			ctx->state = I2C_STATE_RX;
			// throw away the first byte read from the device.
			(void)I2C_D_REG(ctx->i2c);
		}
		break;
	case I2C_STATE_RX:
		if (ctx->index == ctx->cur->length-1) {		// last byte has been received
			i2c_end_transaction(ctx);
			ctx->cur->buffer[ctx->index++] = I2C_D_REG(ctx->i2c);
			i2c_transaction_next(ctx, result);
		} else {
			if (ctx->index == ctx->cur->length - 2)	// last-but-one byte has been received, send NACK with last byte
				bf_set_reg(I2C_C1_REG(ctx->i2c), I2C_C1_TXAK, 1);		// to signal to the device that we're done receiving
			ctx->cur->buffer[ctx->index++] = I2C_D_REG(ctx->i2c);
		}
		break;
	default:
		// XXX this shouldn't happen, assert or something
		break;
	}
}

void
I2C0_Handler(void)
{
	i2c_interrupt_handler(&i2c0_ctx);
}

void
I2C1_Handler(void)
{
	i2c_interrupt_handler(&i2c1_ctx);
}

void
i2c_init(struct i2c_ctx *ctx, enum i2c_rate rate)
{
	/* Enable clock */
	switch ((uintptr_t)ctx->i2c) {
	case I2C0_BASE:
		bf_set_reg(SIM_SCGC4, SIM_SCGC4_I2C0, 1);
		break;
	case I2C1_BASE:
		bf_set_reg(SIM_SCGC4, SIM_SCGC4_I2C1, 1);
		break;
	}

	//                   I2C0_F values, indexed by enum i2c_rate.
	//                   100kHz 400   600   800   1000  1200  1500  2000  2400kHz
	static uint8_t f[] = {0x27, 0x85, 0x14, 0x45, 0x0D, 0x0B, 0x09, 0x02, 0x00};

	if (rate < 0 || rate >= sizeof(f))
		rate = I2C_RATE_100;
	I2C_F_REG(ctx->i2c) = f[rate];

	bf_set_reg(I2C_C1_REG(ctx->i2c), I2C_C1_IICEN, 1);
	ctx->state = I2C_STATE_IDLE;
	ctx->cur = NULL;

	switch ((uintptr_t)ctx->i2c) {
	case I2C0_BASE:
		int_enable(IRQ_I2C0);
		break;
	case I2C1_BASE:
		int_enable(IRQ_I2C1);
		break;
	}
}

void
i2c_queue(struct i2c_ctx *ctx, struct i2c_transaction *transaction)
{
	crit_enter();
	transaction->next = NULL;		// make sure the new transaction has no leftover 'next' value, since the library handles queueing

	if (ctx->cur == NULL) {			// if no current transaction, start this one right away
		ctx->cur = transaction;
		i2c_start_transaction(ctx);
	} else {				// otherwise, go to end of queue and add transaction there
		struct i2c_transaction *it = ctx->cur;
		while (it->next != NULL)
			it = it->next;
		it->next = transaction;
	}
	crit_exit();
}
