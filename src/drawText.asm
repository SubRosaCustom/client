global pushVarArgs
global clearStack

section .text

pushVarArgs:
	.readloop:
		mov r8d, dword [rcx + rdx * 8h]
		mov [rsp + 48h + rdx * 8h], r8d
		sub rdx, 1
	jnz .readloop
	ret
clearStack:
	.readloop:
		mov dword [rsp + 48h + rcx * 8h], 0000
		sub rcx, 1
	jnz .readloop
	ret