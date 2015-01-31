subdirs: ${SUBDIRS}

%:: subdirs ;

${SUBDIRS}:
	@${MAKE} -C "$@" ${MAKECMDGOALS}

.PHONY: ${SUBDIRS}
