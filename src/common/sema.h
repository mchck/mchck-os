struct sema {
        uint32_t count;
};

#define SEMA_INIT {0}

void sema_wait_count(struct sema *sema, int count);
void sema_wait(struct sema *sema);
void sema_wake_count(struct sema *sema, int count);
void sema_wake(struct sema *sema);
