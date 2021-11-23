.equ O_RDONLY, 2

panic:
    mov x8, 93
    mov x0, 1
    svc 0
    ret
    .global main
main:
    stp fp, lr, [sp, -16]!
    mov fp, sp


    adr x0, file_name
    mov x1, O_RDONLY
    bl open
    cbz x0, panic
    mov x20, x0

    sub sp, sp, 16
    mov x0, x20
    mov x1, sp
    mov x2, 16
    bl  read
    cmp x0, -1
    beq panic


    mov x11, sp
    ldr w10, [x11], 4
1:  sub w10, w10, 1
    cmp w10, 0
    blt 1f
    ldrb w12, [x11], 1
    b   1b
1:

    add sp, sp, 16



    ldp fp, lr, [sp], 16
    mov x0, 0
    ret
.data
file_name: .asciz "numbers"
fmt: .asciz "%d\n"
