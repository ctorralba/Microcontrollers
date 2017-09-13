#include <reg932.inc>
;Christopher Torralba
;Jason Tracy
;Joshua Zygmunt

dseg at 0x30
    counter_A: ds 1

cseg at 0x00        
    mov P2M1,#0
    mov P1M1,#0
    mov P0M1,#0

;LED=
;p2.4 Bit1 (lowest bit)
;p0.5 Bit2
;p2.7 Bit3                       
;p0.6 Bit4 (highest bit)
;SOUND = p1.7
;SWITCH = p2.0 (increment)
;	= p0.1 (decrement)

incr_input bit p2.0
decr_input bit p0.1 
clock_input bit p2.3
music_input bit p2.1
sound_output bit p1.7
led1 bit p2.4
led2 bit p0.5
led3 bit p2.7
led4 bit p0.6

;initialization
mov counter_A, #0
setb incr_input
setb decr_input
mov p2, #0xFF
mov p0, #0xFF

;reading switch increment
Loop:
      		jb incr_input, skip_incr ; basically skipping everything until i get input
wait: 		jnb decr_input, reset ; if the first and second button are pressed the counter is reset
      		jnb incr_input, wait ; waiting for the increment button to be released
      		sjmp increment ; the increment button has been released
reset:		mov counter_A, #0
r_wait:		mov c, incr_input  ;waits for both buttons to be released
      		anl c, decr_input ;as long as one the buttons are pressed the program will wait
      		jnc r_wait
	      	sjmp skip_music ;when both buttons are released the changes are made

;increment counter
increment: 	mov A, counter_A
		add A, #1
		cjne A, #16, SKIP_ROLL; when the counter go over its limit it is set back to zero
		ACALL Sound
		mov counter_A, #0
		clr A
SKIP_ROLL:
		mov counter_A, A

;reading switch decrement
skip_incr:
	       	jb decr_input, skip_decr
wait1:		jnb incr_input, reset; if both buttons are pressed reset the counter
		jnb decr_input, wait1

;decrement counter
		mov A, counter_A
		clr c ; clear carry  for normal subtraction
		subb A, #1
		cjne a, #0xFF, SKIP_ROLL1 ;when the counter goes from 0 to 0xFF the counter is set to 15
		mov counter_A, #15
		mov A, #15
		ACALL Sound
SKIP_ROLL1:
		mov counter_A, A

skip_decr:
;reading test music
		jb music_input, skip_music		;if sw7 is pressed it doesn't skip music
		ctwait: jnb music_input, ctwait		;debouncing

		acall music				;plays the music

		skip_music:				;if i didn't press the switch, I end up here

		;display
		acall display
		sjmp Main_clock

;clock
Main_clock:
		jb clock_input, skip_clock
wait2:		jnb clock_input, wait2
 
clock:
		mov A, counter_A
		add A, #1
		clr c
		cjne A, #16, CLK_SKIP_ROLL
		mov counter_A, #0
		clr A
CLK_SKIP_ROLL:
		mov counter_A, A
		mov R0, 255
jtloop1:    	mov R1, 255
jtloop2:
	        jnb clock_input, skip_clock              
	        djnz R1, jtloop2
	        djnz R0, jtloop1

;display
		acall display ;updates counter display
		sjmp clock
 
skip_clock:
		jtwait2: jnb clock_input, jtwait2
 
		ljmp loop

;creates the turnover sound
Sound:
		mov R3, #128
loop4:		mov R2, #2
loop3:    	cpl sound_output
		mov R1, #10
loop2:    	mov R0, #90
loop1:    	djnz r0, loop1
	    	djnz r1, loop2
		djnz r2, loop3
		djnz r3, loop4
		clr sound_output
ret

music: 
;note 1
		mov R3, #196		;these numbers extend the frequency for .25 secs(250 msec).
n1loop4:	mov R2, #1
n1loop3:	cpl sound_output
		mov R1, #235		;these two numbers create the frequency
n1loop2:	mov R0, #5
n1loop1:	djnz r0, n1loop1
		djnz r1, n1loop2
    		djnz r2, n1loop3
		djnz r3, n1loop4
;note 2
    		mov R3, #185
n2loop4:	mov R2, #1
n2loop3:	cpl sound_output
		mov R1, #83
n2loop2:	mov R0, #15
n2loop1:	djnz r0, n2loop1
    		djnz r1, n2loop2
		djnz r2, n2loop3
		djnz r3, n2loop4
;note 3
		mov R3, #155
n3loop4:	mov R2, #1
n3loop3:	cpl sound_output
		mov R1, #185
n3loop2:	mov R0, #8
n3loop1:	djnz r0, n3loop1
		djnz r1, n3loop2
		djnz r2, n3loop3
		djnz r3, n3loop4
;note 4
		mov R3, #110
n4loop4:	mov R2, #1
n4loop3:	cpl sound_output
		mov R1, #161
n4loop2:	mov R0, #13
n4loop1:	djnz r0, n4loop1
		djnz r1, n4loop2
		djnz r2, n4loop3
		djnz r3, n4loop4	
;note 5
    		mov R3, #103
n5loop4:	mov R2, #1
n5loop3:	cpl sound_output
    		mov R1, #111
n5loop2:	mov R0, #20
n5loop1:	djnz r0, n5loop1
		djnz r1, n5loop2
		djnz r2, n5loop3
		djnz r3, n5loop4
;note 6
		mov R3, #164
n6loop4:	mov R2, #1
n6loop3:	cpl sound_output
		mov R1, #233
n6loop2:	mov R0, #6
n6loop1:	djnz r0, n6loop1
		djnz r1, n6loop2
		djnz r2, n6loop3
		djnz r3, n6loop4
;note 7
	    	mov R3, #207
n7loop4:	mov R2, #1
n7loop3:	cpl sound_output
		mov R1, #222
n7loop2:	mov R0, #5
n7loop1:	djnz r0, n7loop1
		djnz r1, n7loop2
		djnz r2, n7loop3
		djnz r3, n7loop4
;note 8
    		mov R3, #140
n8loop4:	mov R2, #2
n8loop3:	cpl sound_output
		mov R1, #100
n8loop2:	mov R0, #10
n8loop1:	djnz r0, n8loop1
		djnz r1, n8loop2
		djnz r2, n8loop3
		djnz r3, n8loop4
ret

display:        ;updates counter display
		mov A, counter_A	;count goes to the acc
		cpl a			;0's light up LED's
		mov c, acc.0		
		mov led1, c		;puts the value of the lowest bit into switch1		
		mov c, acc.1		
		mov led2, c		;second from lowest bit gets put in switch2
		mov c, acc.2		
		mov led3, c		;second from highest bit gets put in switch3
		mov c, acc.3
		mov led4, c		;highest bit gets put in switch4
ret

end
