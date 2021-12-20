global pushVarArgs
global clearStack

section .text

pushVarArgs:
	.readloop:
		mov r8d, dword [rcx + rdx * 8h]
		%ifdef UNIX
			mov [rsp - 0b8h + 10h + rdx * 8h], r8d ; this probably doesn't work, i need it to be tested
		%else
			mov [rsp + 48h + rdx * 8h], r8d
		%endif
		sub rdx, 1
	jnz .readloop
	ret
clearStack:
	.readloop:
		%ifdef UNIX
			mov dword [rsp - 0b8h + 10h + rcx * 8h], 0000 ; this probably doesn't work, i need it to be tested
		%else
			mov dword [rsp + 48h + rcx * 8h], 0000
		%endif
		sub rcx, 1
	jnz .readloop
	ret