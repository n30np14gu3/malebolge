.code
hook_stub proc
pop     r11
sub     rsp, 28h
mov     rax, [rdx+0B8h]
mov     r9, [rcx+40h]
movzx   r8d, byte ptr [rax]
mov     rax, [r9+1C0h]
mov     rax, [rax+r8*8]
jmp     r11
hook_stub endp
end