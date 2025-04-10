
irq.o：     文件格式 elf64-littleaarch64


Disassembly of section .text:

0000000000000000 <button_interrupt>:
   0:	a9bf7bfd 	stp	x29, x30, [sp,#-16]!
   4:	910003fd 	mov	x29, sp
   8:	aa1e03e0 	mov	x0, x30
   c:	94000000 	bl	0 <_mcount>
  10:	52800ea0 	mov	w0, #0x75                  	// #117
  14:	94000000 	bl	0 <gpio_to_desc>
  18:	94000000 	bl	0 <gpiod_get_raw_value>
  1c:	2a0003e1 	mov	w1, w0
  20:	90000000 	adrp	x0, 0 <button_interrupt>
  24:	91000000 	add	x0, x0, #0x0
  28:	94000000 	bl	0 <printk>
  2c:	52800020 	mov	w0, #0x1                   	// #1
  30:	a8c17bfd 	ldp	x29, x30, [sp],#16
  34:	d65f03c0 	ret

Disassembly of section .init.text:

0000000000000000 <init_module>:
   0:	a9be7bfd 	stp	x29, x30, [sp,#-32]!
   4:	910003fd 	mov	x29, sp
   8:	a90153f3 	stp	x19, x20, [sp,#16]
   c:	90000014 	adrp	x20, 0 <init_module>
  10:	aa1e03e0 	mov	x0, x30
  14:	91000294 	add	x20, x20, #0x0
  18:	94000000 	bl	0 <_mcount>
  1c:	aa1403e1 	mov	x1, x20
  20:	52800ea0 	mov	w0, #0x75                  	// #117
  24:	94000000 	bl	0 <gpio_request>
  28:	2a0003f3 	mov	w19, w0
  2c:	34000140 	cbz	w0, 54 <init_module+0x54>
  30:	2a0003e2 	mov	w2, w0
  34:	52800ea1 	mov	w1, #0x75                  	// #117
  38:	90000000 	adrp	x0, 0 <init_module>
  3c:	91000000 	add	x0, x0, #0x0
  40:	94000000 	bl	0 <printk>
  44:	2a1303e0 	mov	w0, w19
  48:	a94153f3 	ldp	x19, x20, [sp,#16]
  4c:	a8c27bfd 	ldp	x29, x30, [sp],#32
  50:	d65f03c0 	ret
  54:	52800ea0 	mov	w0, #0x75                  	// #117
  58:	94000000 	bl	0 <gpio_to_desc>
  5c:	94000000 	bl	0 <gpiod_direction_input>
  60:	2a0003f3 	mov	w19, w0
  64:	34000120 	cbz	w0, 88 <init_module+0x88>
  68:	2a0003e2 	mov	w2, w0
  6c:	52800ea1 	mov	w1, #0x75                  	// #117
  70:	90000000 	adrp	x0, 0 <init_module>
  74:	91000000 	add	x0, x0, #0x0
  78:	94000000 	bl	0 <printk>
  7c:	52800ea0 	mov	w0, #0x75                  	// #117
  80:	94000000 	bl	0 <gpio_free>
  84:	17fffff0 	b	44 <init_module+0x44>
  88:	90000013 	adrp	x19, 0 <init_module>
  8c:	52800ea0 	mov	w0, #0x75                  	// #117
  90:	94000000 	bl	0 <gpio_to_desc>
  94:	94000000 	bl	0 <gpiod_to_irq>
  98:	b9000260 	str	w0, [x19]
  9c:	2a0003e1 	mov	w1, w0
  a0:	36f80100 	tbz	w0, #31, c0 <init_module+0xc0>
  a4:	90000000 	adrp	x0, 0 <init_module>
  a8:	91000000 	add	x0, x0, #0x0
  ac:	94000000 	bl	0 <printk>
  b0:	52800ea0 	mov	w0, #0x75                  	// #117
  b4:	94000000 	bl	0 <gpio_free>
  b8:	b9400273 	ldr	w19, [x19]
  bc:	17ffffe2 	b	44 <init_module+0x44>
  c0:	90000000 	adrp	x0, 0 <init_module>
  c4:	91000000 	add	x0, x0, #0x0
  c8:	94000000 	bl	0 <printk>
  cc:	b9400260 	ldr	w0, [x19]
  d0:	d2800005 	mov	x5, #0x0                   	// #0
  d4:	aa1403e4 	mov	x4, x20
  d8:	d2800063 	mov	x3, #0x3                   	// #3
  dc:	d2800002 	mov	x2, #0x0                   	// #0
  e0:	90000001 	adrp	x1, 0 <init_module>
  e4:	91000021 	add	x1, x1, #0x0
  e8:	94000000 	bl	0 <request_threaded_irq>
  ec:	2a0003f3 	mov	w19, w0
  f0:	340000c0 	cbz	w0, 108 <init_module+0x108>
  f4:	2a0003e1 	mov	w1, w0
  f8:	90000000 	adrp	x0, 0 <init_module>
  fc:	91000000 	add	x0, x0, #0x0
 100:	94000000 	bl	0 <printk>
 104:	17ffffde 	b	7c <init_module+0x7c>
 108:	90000000 	adrp	x0, 0 <init_module>
 10c:	91000000 	add	x0, x0, #0x0
 110:	94000000 	bl	0 <printk>
 114:	17ffffcc 	b	44 <init_module+0x44>

Disassembly of section .exit.text:

0000000000000000 <cleanup_module>:
   0:	a9bf7bfd 	stp	x29, x30, [sp,#-16]!
   4:	90000000 	adrp	x0, 0 <cleanup_module>
   8:	d2800001 	mov	x1, #0x0                   	// #0
   c:	910003fd 	mov	x29, sp
  10:	b9400000 	ldr	w0, [x0]
  14:	94000000 	bl	0 <free_irq>
  18:	52800ea0 	mov	w0, #0x75                  	// #117
  1c:	94000000 	bl	0 <gpio_free>
  20:	90000000 	adrp	x0, 0 <cleanup_module>
  24:	91000000 	add	x0, x0, #0x0
  28:	94000000 	bl	0 <printk>
  2c:	a8c17bfd 	ldp	x29, x30, [sp],#16
  30:	d65f03c0 	ret
