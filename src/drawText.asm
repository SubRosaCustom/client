global pushVarArgs
global clearStack

section .text

pushVarArgs:
	.readloop:
		%ifdef UNIX
			mov r8d, dword [rdi + rsi * 8h]
			mov [rsp + 8h + rsi * 8h], r8d ; this probably doesn't work, i need it to be tested
			sub rsi, 1
		%else
			mov r8d, dword [rcx + rdx * 8h]
			mov [rsp + 48h + rdx * 8h], r8d
			sub rdx, 1
		%endif
	jnz .readloop
	ret
clearStack:
	.readloop:
		%ifdef UNIX
			mov dword [rsp + 8h + rdi * 8h], 0000 ; this probably doesn't work, i need it to be tested
			sub rdi, 1
		%else
			mov dword [rsp + 48h + rcx * 8h], 0000
			sub rcx, 1
		%endif
	jnz .readloop
	ret