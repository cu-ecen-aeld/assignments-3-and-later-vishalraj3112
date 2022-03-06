# Kernal oops analysis of faulty module

## Output trace of oops for faulty write in qemu


`Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000
Mem abort info:
  ESR = 0x96000046
  EC = 0x25: DABT (current EL), IL = 32 bits
  SET = 0, FnV = 0
  EA = 0, S1PTW = 0
Data abort info:
  ISV = 0, ISS = 0x00000046
  CM = 0, WnR = 1
user pgtable: 4k pages, 39-bit VAs, pgdp=000000004201a000
[0000000000000000] pgd=0000000041fe1003, p4d=0000000041fe1003, pud=0000000041fe1003, pmd=0000000000000000
Internal error: Oops: 96000046 [#1] SMP
Modules linked in: faulty(O) hello(O) scull(O) [last unloaded: faulty]
CPU: 0 PID: 152 Comm: sh Tainted: G           O      5.10.7 #1
Hardware name: linux,dummy-virt (DT)
pstate: 80000005 (Nzcv daif -PAN -UAO -TCO BTYPE=--)
pc : faulty_write+0x10/0x20 [faulty]
lr : vfs_write+0xc0/0x290
sp : ffffffc010c53db0
x29: ffffffc010c53db0 x28: ffffff8001ffd780 
x27: 0000000000000000 x26: 0000000000000000 
x25: 0000000000000000 x24: 0000000000000000 
x23: 0000000000000000 x22: ffffffc010c53e30 
x21: 00000000004c98e0 x20: ffffff8001fa6b00 
x19: 000000000000000c x18: 0000000000000000 
x17: 0000000000000000 x16: 0000000000000000 
x15: 0000000000000000 x14: 0000000000000000 
x13: 0000000000000000 x12: 0000000000000000 
x11: 0000000000000000 x10: 0000000000000000 
x9 : 0000000000000000 x8 : 0000000000000000 
x7 : 0000000000000000 x6 : 0000000000000000 
x5 : ffffff80020482f8 x4 : ffffffc00867c000 
x3 : ffffffc010c53e30 x2 : 000000000000000c 
x1 : 0000000000000000 x0 : 0000000000000000 
Call trace:
 faulty_write+0x10/0x20 [faulty]
 ksys_write+0x6c/0x100
 __arm64_sys_write+0x1c/0x30
 el0_svc_common.constprop.0+0x9c/0x1c0
 do_el0_svc+0x70/0x90
 el0_svc+0x14/0x20
 el0_sync_handler+0xb0/0xc0
 el0_sync+0x174/0x180
Code: d2800001 d2800000 d503233f d50323bf (b900003f) 
---[ end trace 386727d5243ab2cd ]---`

## Analysis and explanation of oops message 

The oops messages are generated null-pointer derefrences or by use of an incorrect pointer value.
In the above case, the oops message resulted due to a null-pointer dereference as shown in the first line of the message-
*Unable to handle kernal pointer dereference.*

All address in Linux are virtual addresses which are mapped to physical address through page tables. When an invalid 
pointer dereference occurs, the pagin mechanism fails to map the pointer to a physical address and a page fault signal
is given to the operating system.

In the above message, the processor state at the time the fault occured is shown. The various value of registers specific 
to the hardware like link register, program counter, stack pointer and other general purpose registers are shown. The value 
of program counter regsiter is shown which is useful to find the location of the fault.

The following message - *pc : faulty_write+0x10/0x20 [faulty]* provides us the information that the fault occured at the 
faulty_write function and specifically 16(0x10) bytes inside it which appers to be 0x20 bytes long and during the time the
'faulty' module was loaded.

We can look at the actual code causing the problem:<br>
*
`ssize_t faulty_write (struct file *filp, const char __user *buf, size_t count,
		loff_t *pos)
{
	/* make a simple fault by dereferencing a NULL pointer */
	*(int *)0 = 0;
	return 0;
}`
*
Here we can see that in the above code, we are trying to dereference a 0 or NULL pointer, which cannot be a valid pointer value.
Because of this invalid pointer access, a fault occurs and the kernal provides an oops message. The process also gets killed after
this and often the way to recover is to reboot.

The other message shown at the end of the oops message is the call trace dump. In our case the function call is not very deep and 
the faulty_write function only calls ksys_write function and the corresposding location along with code memory dump upto the fault
location is displayed.

