.data
one DWORD 11111h, 22222h, 33333h, 44444h
two WORD 0AEFFH, 23CAH, 1156H
myList DWORD 10h, 20h, 30h, 40h

arrayType DWORD TYPE one ;Store the TYPE of the array being operated on in L1
arrayLength DWORD LENGTHOF one ;Store the LENGTH of the array being operated on in L1

.code
main proc
	mov EAX, 0 ;Set EAX to 0 to use as the sum
	add EAX, one ;Add first element of first array
	add EAX, one + 4 ;Add second element, located 4 bytes away because elements are DWORDs 
	add EAX, one + 8
	add EAX, one + 12
	add EAX, DWORD PTR two ;Add first element of second array (use DWORD PTR because EAX is a DWORD and the elements are WORDs)
	add EAX, DWORD PTR two + 2 ;Add second element, located 2 bytes away because elements are WORDs
	add EAX, DWORD PTR two + 4 ;EAX now equals the sum of all of the elements from arrays one and two
	
	call DumpRegs ;Output EAX

	mov EAX, 0 ;Set EAX back to 0 to use as the sum
	mov ECX, 2 ;L1 should loop twice, once for each array
	mov ESI, OFFSET one ;Set the source to the first array
	L1: ;Create a loop to handle each array
		mov EBX, ECX ;Store L1's loop counter into EBX so ECX can be used in L2
		mov ECX, arrayLength ;Set L2's loop counter to the amount of elements in the current array
		L2:
			add EAX, DWORD PTR [ESI] ;Add the current element to EAX (use DWORD PTR to enforce adding a DWORD when using second array)
			add ESI, DWORD PTR arrayType ;Move the source to the next element
			loop L2
		mov arrayType, TYPE two ;Store the TYPE of the array being operated on in L1
		mov arrayLength, LENGTHOF two ;Store the LENGTH of the array being operated on in L1
		mov ESI, OFFSET two ;Set the source to the second array
		mov ECX, EBX ;Use EBX's value as loop counter for L1
		loop L1

	call DumpRegs ;Output EAX again, should be the same value as the first time

	mov EAX, myList ;EAX = 10h
	xchg EAX, myList + 8 ;EAX = 30h, myList = 10h 20h 10h 40h
	xchg EAX, myList + 12 ;EAX = 40h, myList = 10h 20h 10h 30h
	xchg EAX, myList + 4 ;EAX = 20h, myList = 10h 40h 10h 30h
	xchg EAX, myList ;EAX = 10h, myList = 20h 40h 10h 30h

	mov EAX, 0; Now that myList is rearranged, EAX can be used to store the new order it
	mov AH, BYTE PTR myList ;Put the first element into AH
	mov AL, BYTE PTR myList + 4 ;Put the second element into AL
	imul EAX, 10000h ;Shift AX into the leftmost (more significant) side of EAX
	mov AH, BYTE PTR myList + 8 ;Put the third element into AH
	mov AL, BYTE PTR myList + 12 ;Put the fourth element into AL

	call DumpRegs ;EAX = 20401030h
	invoke ExitProcess,0
main endp
end main
