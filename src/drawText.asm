public pushVarArgs
public clearStack

.code

pushVarArgs proc
	readloop:
		mov r8d, dword ptr [rcx + rdx * 8h]
		mov [rsp + 48h + rdx * 8h], r8d
		sub rdx, 1
	jnz readloop
	ret
pushVarArgs endp

clearStack proc
	readloop:
		mov qword ptr [rsp + 40h + rcx * 8h], 0
		sub rcx, 1
	jnz readloop
	ret
clearStack endp

end