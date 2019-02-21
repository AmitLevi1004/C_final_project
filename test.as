; test.as

STR:		.string 	"abcdef"
LENGTH-:		.data		6,-9 , 15
K:			.data		22

			.entry		LOOP
	 		.entry		LENGTH
			.extern		L3
			.extern		W

MAIN:		cmp			#56, W
			ad			23, r4
			add			r2,STR
			sub			r1, r4
			not			K
			clr			r2
			lea			LENGTH, L3
			dec			K
			jmp			END
			bne			LENGTH
			re			r1
			prn			#4
			jsr			K
			rts			
			add			r2,ST
LOOP:		jmp			W
			prn 		#-5
			inc 		K

			bn 		L3
END:		stop


