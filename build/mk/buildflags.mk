.buildflags: .FORCE
	@-rm -f $@.tmp
ifneq ($(flavor _saved_CFLAGS),undefined)
	@printf "_saved_CFLAGS?=${_saved_CFLAGS}\nCFLAGS?=\$${_saved_CFLAGS}\n" >> $@.tmp
endif
ifneq ($(flavor _saved_BOARD),undefined)
	@printf "_saved_BOARD?=${_saved_BOARD}\nBOARD?=\$${_saved_BOARD}\n" >> $@.tmp
endif
	@cmp $@ $@.tmp 2>/dev/null || mv $@.tmp $@ 2>/dev/null

.FORCE:

REALCLEANFILES+= .buildflags .buildflags.tmp
