.global call_trusted

call_trusted:
	stmfd sp!, {r4-r12,lr}
	mov r6, #0xff
	mov r12, #0
	dsb
	isb
	dmb
.arch_extension sec
	smc 1
	ldmfd sp!, {r4-r12,lr}
	bx lr

