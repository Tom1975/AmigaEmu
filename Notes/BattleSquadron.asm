
; Bitplane : 
; 0 : #$3E620
; 1 : #$41620
; 2 : #$44620

3C000: bsr 3C082                 	; call Init function
3C004: bsr 3C1A6                 	; call Init starfield
3C008: bsr 3C2CC                 61 00 02 C2 
3C00C: move.w #$0, 3C3DA         33 FC 00 00 00 03 C3 DA 
main_loop:
3C014: bsr 3C1FC                 61 00 01 E6 
3C018: addi.w #$C3DA, 30003      06 79 00 03 00 03 C3 DA 
3C020: lea DFF000, A5            4B F9 00 DF F0 00 
3C026: bsr 3C3DC                 61 00 03 B4 
3C02A: bsr 3C512                 61 00 04 E6 
3C02E: bsr 3C552                 61 00 05 22 
3C032: bsr 3C532                 61 00 04 FE 
3C036: bsr 3C486                 61 00 04 4E 
3C03A: bsr 3C4C8                 61 00 04 8C 
3C03E: btst #6, BFE001           	; check left mousebutton
3C046: beq.s 3C062               	
3C04A: move.w DFF012, D0         	; check POT0
3C050: cmp.w 3C080, D0           	; Compare with previously saved value
3C056: bne.s 3C05A               	; not equal ?
3C058: bra.s 3C014               	; no change : return to main_loop
mouse_not_equal:
3C05A: sra 7FF00                 50 F9 00 07 FF 00 
3C060: bra.s 3C06A               60 08 

3C062: ssr 7FF00                 51 F9 00 07 FF 00 
3C068: nop                       4E 71 
3C06A: move.l 3C1A2, 6C          23 F9 00 03 C1 A2 00 00 00 6C 
3C074: rts                       	; quit trainer function

3C076: addX.b D0, D2             D5 00 
3C078: bsr 3C152                 61 00 00 D8 
3C07C: moveq #$0, D0             70 00 
3C07E: rts                       4E 75 

3C080: 3F 3F 						; data : POT0 value

; Init
; Set bitplanes, copperlist, etc
3C082: move.w DFF012, 3C080      	; Get POT0DAT
3C08C: move.l 6C, 3C1A2          	
3C096: move.l #$3C18C, 6C        	; set ... ?
3C0A0: move.w #$C020, DFF09A     	; set INTENA
3C0A8: nop                       4E 71 
3C0AA: nop                       4E 71 
3C0AC: nop                       4E 71 
3C0AE: andi.b #$F7, BFD100       	; PRB : Sel0, stop motor
3C0B6: ori.b #$88, BFD100        	; unsel Sel0, motor still stopped
3C0BE: lea 3E620, A0             	
3C0C4: lea 47620, A1             	
3C0CA: clr.l (A0)+               	; clear from 3e620 to 47620 (3x bitplanes used)
3C0CC: cmpa.l A0, A1             B3 C8 
3C0CE: bne.s 3C0CA               66 FA 
3C0D0: move.l #$3E620, D0        ; set bitplane for copperlist
3C0D6: move.w D0, 3CE7A          33 C0 00 03 CE 7A 
3C0DC: swap  D0                  48 40 
3C0DE: move.w D0, 3CE76          33 C0 00 03 CE 76 
3C0E4: move.l #$41620, D0        ; set bitplane for copperlist
3C0EA: move.w D0, 3CE82          33 C0 00 03 CE 82 
3C0F0: swap  D0                  48 40 
3C0F2: move.w D0, 3CE7E          33 C0 00 03 CE 7E 
3C0F8: move.l #$44620, D0        ; set bitplane for copperlist
3C0FE: move.w D0, 3CE8A          33 C0 00 03 CE 8A 
3C104: swap  D0                  48 40 
3C106: move.w D0, 3CE86          33 C0 00 03 CE 86 
3C10C: move.l #$3CE44, DFF080    ; set Copperlist
3C116: clr.w DFF088              ; start COPPER
3C11C: moveq #$0, D0             70 00 
3C11E: nop                       4E 71 
3C120: nop                       4E 71 
3C122: nop                       4E 71 
3C124: move.w #$420, DFF096      ; 	DMACON : clear SPREN, BLTPRI         
3C12C: move.w #$83D0, DFF096     ; 	DLACON : set DMAEN, BPLEN, COPEN, BLTEN, DSKEN
3C134: move.w #$0, DFF142        ; 	Sprite 0 => 0
3C13C: bclr #$1, BFE201          ;   Led is set as input
3C144: nop                       4E 71 
3C146: rts                       4E 75 

3C148: ori.b #$D0, D3            00 03 B0 D0 
3C14C: ori.b #$6C, D0            00 00 00 6C 
3C150: rts                       4E 75 
3C152: jsr 16208                 4E B9 00 01 62 08 
3C158: ori.b #$6C, D0            00 00 00 6C 
3C15C: jsr 304AC                 4E B9 00 03 04 AC 
3C162: move.l 3C3D6, A6          2C 79 00 03 C3 D6 
3C168: move.l ($26,A6), DFF080   23 EE 00 26 00 DF F0 80 
3C170: move.l 4, A6              2C 78 00 04 
3C174: jsr ($FF76,A6)            4E AE FF 76 
3C178: move.l 3C3D6, A1          22 79 00 03 C3 D6 
3C17E: jsr ($FE62,A6)            4E AE FE 62 
3C182: move.w #$8020, DFF096     33 FC 80 20 00 DF F0 96 
3C18A: rts                       4E 75 
3C18C: movem.l D0/D1/D2/D3/D4/D5/D6/D7/A0/A1/A2/A3/A4/A5/A6, -(SP)48 E7 FF FE 
3C190: jsr 3C340                 4E B9 00 03 C3 40 
3C196: nop                       4E 71 
3C198: nop                       4E 71 
3C19A: nop                       4E 71 
3C19C: movem.l (SP)+, D0/D1/D2/D3/D4/D5/D6/D7/A0/A1/A2/A3/A4/A5/A64C DF 7F FF 
3C1A0: jmp FC0CD8                4E F9 00 FC 0C D8 

; Init starfied value
; write at 3E094 what seems to be starfields
3C1A6: move.w #$46, D1           32 3C 00 46 
3C1AA: lea 3E094, A0             41 F9 00 03 E0 94 
3C1B0: lea 3E35A, A1             43 F9 00 03 E3 5A 
3C1B6: bsr 3C2B0                 61 00 00 F8 
3C1BA: move.w D0, (A0)+          30 C0 
3C1BC: clr.w (A1)+               42 59 
3C1BE: bsr 3C2B0                 61 00 00 F0 
3C1C2: move.w D0, (A0)+          30 C0 
3C1C4: clr.w (A1)+               42 59 
3C1C6: bsr 3C2B0                 61 00 00 E8 
3C1CA: andi.w #$1FF, D0          02 40 01 FF 
3C1CE: move.w D0, (A0)+          30 C0 
3C1D0: dbra.s D1, 3C1B6          51 C9 FF E4 
3C1D4: rts                       4E 75 

3C1D6: mulu #$30, D1             C2 FC 00 30 
3C1DA: move.w D0, D2             34 00 
3C1DC: asr.w  #3, D2             E6 42 
3C1DE: add.w D2, D1              D2 42 
3C1E0: asl.w  #3, D2             E7 42 
3C1E2: sub.w  D0, D2             94 40 
3C1E4: subq.b #1, D2             53 02 
3C1E6: lea 3E620, A1             43 F9 00 03 E6 20 
3C1EC: add.l D1, A1              D3 C1 
3C1EE: bclr D2, (A1)             05 91 
3C1F0: lea 44620, A1             43 F9 00 04 46 20 
3C1F6: add.l D1, A1              D3 C1 
3C1F8: bclr D2, (A1)             05 91 
3C1FA: rts                       4E 75 

; display starfield
3C1FC: lea 3E094, A4             49 F9 00 03 E0 94 
3C202: lea 3E35A, A3             47 F9 00 03 E3 5A 
3C208: move.w #$46, D3           36 3C 00 46 
3C20C: move.w (A4)+, D4          38 1C 
3C20E: move.w (A4)+, D5          3A 1C 
3C210: move.w (A4), D6           3C 14 
3C212: subi.w #$4, (A4)+         04 5C 00 04 
3C216: tst.w  D6                 4A 46 
3C218: ble.s 3C29E               ; renew this star !
3C21C: ext.l D4                  48 C4 
3C21E: divs D6, D4               89 C6 
3C220: addi.w #$BF, D4           06 44 00 BF 
3C224: ext.l D5                  48 C5 
3C226: divs D6, D5               8B C6 
3C228: addi.w #$69, D5           06 45 00 69 
3C22C: tst.w  D4                 4A 44 
3C22E: blt.s 3C29E               ; renew this star !
3C230: tst.w  D5                 4A 45 
3C232: blt.s 3C29E               ; renew this star !
3C234: cmp.w #$17F, D4           0C 44 01 7F 
3C238: bgt.s 3C29E               ; renew this star !
3C23A: cmp.w #$D3, D5            0C 45 00 D3 
3C23E: bgt.s 3C29E               ; renew this star !
3C240: move.w (A3), D0           30 13 
3C242: move.w D4, (A3)+          36 C4 
3C244: move.w (A3), D1           32 13 
3C246: move.w D5, (A3)+          36 C5 
3C248: bsr 3C1D6                 61 8C 
3C24A: move.w D4, D0             30 04 
3C24C: move.w D5, D1             32 05 
3C24E: mulu #$30, D1             C2 FC 00 30 
3C252: move.w D0, D2             34 00 
3C254: asr.w  #3, D2             E6 42 
3C256: add.w D2, D1              D2 42 
3C258: asl.w  #3, D2             E7 42 
3C25A: sub.w  D0, D2             94 40 
3C25C: subq.b #1, D2             53 02 
3C25E: cmp.w #$190, D6           0C 46 01 90 
3C262: bgt.s 3C26C               6E 08 
3C264: cmp.w #$10C, D6           0C 46 01 0C 
3C268: bgt.s 3C278               6E 0E 
3C26A: bra.s 3C284               60 18 
3C26C: lea 3E620, A1             43 F9 00 03 E6 20 
3C272: add.l D1, A1              D3 C1 
3C274: bset D2, (A1)             05 D1 
3C276: bra.s 3C298               60 20 
3C278: lea 44620, A1             43 F9 00 04 46 20 
3C27E: add.l D1, A1              D3 C1 
3C280: bset D2, (A1)             05 D1 
3C282: bra.s 3C298               60 14 
3C284: lea 3E620, A1             43 F9 00 03 E6 20 
3C28A: add.l D1, A1              D3 C1 
3C28C: bset D2, (A1)             05 D1 
3C28E: lea 44620, A1             43 F9 00 04 46 20 
3C294: add.l D1, A1              D3 C1 
3C296: bset D2, (A1)             05 D1 
3C298: dbra.s D3, 3C20C          51 CB FF 72 
3C29C: rts                       4E 75 
; star is out of the field
3C29E: suba.w #$6, A4            98 FC 00 06 
3C2A2: bsr 3C2B0                 ; ask for a new one
3C2A4: move.w D0, (A4)+          38 C0 
3C2A6: bsr 3C2B0                 61 08 
3C2A8: move.w D0, (A4)+          38 C0 
3C2AA: move.w #$258, (A4)+       ; put if far away in the field
3C2AE: bra.s 3C298               60 E8 

; Random value ?
; computation : word at 3c2c6 = (3c2c6) * vhposr + 1249 (all is signed)
; return as D0
3C2B0: move.w DFF006, D0         ; get vhposr
3C2B6: lea 3C2C6(PC), A2         45 FA 00 0E 
3C2BA: muls (A2), D0             C1 D2 
3C2BC: addi.w #$1249, D0         06 40 12 49 
3C2C0: ext.l D0                  48 C0 
3C2C2: move.w D0, (A2)           34 80 
3C2C4: rts                       4E 75 

0003C2C6: 88 D8 	; data
0003C2C8: ori.b #$B0, D3            00 03 CE B0 

; 
0003C2CC: lea 3CEA0, A2             45 F9 00 03 CE A0 
0003C2D2: lea 41630, A4             49 F9 00 04 16 30 
0003C2D8: move.l A4, A1             22 4C 
0003C2DA: bsr 3C2EE                 61 12 
0003C2DC: add.l #$1, A4             D9 FC 00 00 00 01 
0003C2E2: add.l #$1, A2             D5 FC 00 00 00 01 
0003C2E8: tst.b  (A2)               4A 12 
0003C2EA: bne.s 3C2D8               66 EC 
0003C2EC: rts                       4E 75 

0003C2EE: clr.l D0                  42 80 
0003C2F0: move.b (A2), D0           10 12 
0003C2F2: subi.b #$20, D0           04 00 00 20 
0003C2F6: mulu #$8, D0              C0 FC 00 08 
0003C2FA: lea 3CB44, A0             41 F9 00 03 CB 44 
0003C300: add.l D0, A0              D1 C0 
0003C302: move.w #$7, D0            30 3C 00 07 
0003C306: move.b (A0), (A1)         12 90 
0003C308: add.l #$30, A1            D3 FC 00 00 00 30 
0003C30E: move.b (A0)+, (A1)        12 98 
0003C310: add.l #$30, A1            D3 FC 00 00 00 30 
0003C316: dbra.s D0, 3C306          51 C8 FF EE 
0003C31A: rts                       4E 75 

0003C31C: ori.b #$53, D3            00 03 20 53 
0003C320: bsr 3C396                 61 74 
0003C322: bsr 3C392                 61 6E 
0003C324: move.l (A3), ($6861,SP)   2F 53 68 61 
0003C328: bcc.s 3C399               64 6F 
0003C32A: moveq #$6C, D3            77 6C 
0003C32C: ble.s 3C3A0               6F 72 
0003C32E: bcc.s 3C35F               64 2F 
0003C330: addq.w #0, -(A1)          50 61 
0003C332: moveq #$61, D1            72 61 
0003C334: beq.s 3C3A5               67 6F 
0003C336: bgt.s 3C358               6E 20 
0003C338: ble.s 3C3A0               6F 66 
0003C33A: move.l (A4), A0           20 54 
0003C33C: Illegal                   49 43 
0003C33E: move.l D0, D0             20 00 
0003C340: move.l 3C3D6, A6          2C 79 00 03 C3 D6 
0003C346: move.w 3C31C, D6          3C 39 00 03 C3 1C 
0003C34C: addq.w #1, D6             52 46 
0003C34E: cmp.w #$4, D6             BC 7C 00 04 
0003C352: bne.s 3C35C               66 08 
0003C354: move.w #$0, D6            3C 3C 00 00 
0003C358: bsr 3C3A2                 61 00 00 48 
0003C35C: move.w D6, 3C31C          33 C6 00 03 C3 1C 
0003C362: bsr 3C368                 61 00 00 04 
0003C366: rts                       4E 75 
0003C368: lea DFF000, A5            4B F9 00 DF F0 00 
0003C36E: btst #E, ($2,A5)          08 2D 00 0E 00 02 
0003C374: bne.s 3C36E               66 F8 
0003C376: move.l #$5CCE000, ($40,A5)2B 7C 05 CC E0 00 00 40 
0003C37E: lea 43812, A0             41 F9 00 04 38 12 
0003C384: move.l A0, ($54,A5)       2B 48 00 54 
0003C388: move.w #$2, ($66,A5)      3B 7C 00 02 00 66 
0003C38E: addq.l #2, A0             54 88 
0003C390: move.l A0, ($4C,A5)       2B 48 00 4C 
0003C394: move.w #$2, ($62,A5)      3B 7C 00 02 00 62 
0003C39A: move.w #$417, ($58,A5)    3B 7C 04 17 00 58 
0003C3A0: rts                       4E 75 
0003C3A2: move.l 3C2C8, A2          24 79 00 03 C2 C8 
0003C3A8: addq.l #1, A2             52 8A 
0003C3AA: tst.b  (A2)               4A 12 
0003C3AC: bne.s 3C3B4               66 06 
0003C3AE: lea 3CEB0, A2             45 F9 00 03 CE B0 
0003C3B4: move.l A2, 3C2C8          23 CA 00 03 C2 C8 
0003C3BA: lea 4383F, A1             43 F9 00 04 38 3F 
0003C3C0: bsr 3C2EE                 61 00 FF 2C 
0003C3C4: rts                       4E 75 
0003C3C6: beq.s 3C43A               67 72 
0003C3C8: bsr 3C43A                 61 70 
0003C3CA: bvc.s 3C435               68 69 
0003C3CC: bls.s 3C441               63 73 
0003C3CE: move.l ($6962,A4), SP     2E 6C 69 62 
0003C3D2: moveq #$61, D1            72 61 
0003C3D4: moveq #$79, D1            72 79 
0003C3D6: bset #$1DFE, D0           00 C0 1D FE 

0003C3DA: bset #$47F9, ($3,A2)      00 EA 

0003C3DC: lea 3C80E, A3             47 F9 00 03 C8 0E 
0003C3E2: lea 3C814, A2             45 F9 00 03 C8 14 
0003C3E8: lea DFF000, A5            4B F9 00 DF F0 00 
0003C3EE: moveq #$0, D0             70 00 
0003C3F0: nop                       4E 71 
0003C3F2: ori.b #$F, D0             00 00 00 0F 
0003C3F6: addi.w #$2, D0            06 40 00 02 
0003C3FA: add.w D0, ($2,A3)         D1 6B 00 02 
0003C3FE: andi.w #$1FE, ($2,A3)     02 6B 01 FE 00 02 
0003C404: addi.w #$4, ($3,A3)       06 6B 00 03 00 04 
0003C40A: andi.w #$1FE, ($4,A3)     02 6B 01 FE 00 04 
0003C410: addi.w #$0, ($9,A2)       06 6A 00 09 00 00 
0003C416: andi.w #$1FE, ($0,A2)     02 6A 01 FE 00 00 
0003C41C: cmp.w #$0, 3C482          0C 79 00 00 00 03 C4 82 
0003C424: bne.s 3C44A               66 24 
0003C426: addi.w #$C81A, 3E80003    06 79 03 E8 00 03 C8 1A 
0003C42E: cmp.w #$6000, 3C81A       0C 79 60 00 00 03 C8 1A 
0003C436: ble.s 3C480               6F 48 
0003C438: move.w #$46, 3C484        33 FC 00 46 00 03 C4 84 
0003C440: move.w #$1, 3C482         33 FC 00 01 00 03 C4 82 
0003C448: bra.s 3C480               60 36 
0003C44A: cmp.w #$1, 3C482          0C 79 00 01 00 03 C4 82 
0003C452: bne.s 3C466               66 12 
0003C454: subq.w #1, 3C484          53 79 00 03 C4 84 
0003C45A: bne.s 3C480               66 24 
0003C45C: move.w #$2, 3C482         33 FC 00 02 00 03 C4 82 
0003C464: bra.s 3C480               60 1A 
0003C466: subi.w #$C81A, 3E80003    04 79 03 E8 00 03 C8 1A 
0003C46E: bpl.s 3C480               6A 10 
0003C470: move.w #$0, 3C81A         33 FC 00 00 00 03 C8 1A 
0003C478: move.w #$0, 3C482         33 FC 00 00 00 03 C4 82 
0003C480: rts                       4E 75 
0003C482: ori.b #$0, D2             00 02 00 00 

0003C486: lea DFF000, A5            4B F9 00 DF F0 00 
0003C48C: move.l DFF004, D0         20 39 00 DF F0 04 
0003C492: andi.l #$1FF00, D0        02 80 00 01 FF 00 
0003C498: cmp.l #$B000, D0          B0 BC 00 00 B0 00 
0003C49E: bne.s 3C48C               66 EC 
0003C4A0: btst #E, ($2,A5)          08 2D 00 0E 00 02 
0003C4A6: bne.s 3C4A0               66 F8 
0003C4A8: move.l #$1000000, ($40,A5)2B 7C 01 00 00 00 00 40 
0003C4B0: lea 419E0, A0             41 F9 00 04 19 E0 
0003C4B6: move.l A0, ($54,A5)       2B 48 00 54 
0003C4BA: move.w #$0, ($66,A5)      3B 7C 00 00 00 66 
0003C4C0: move.w #$1AE4, ($58,A5)   3B 7C 1A E4 00 58 
0003C4C6: rts                       4E 75 

0003C4C8: lea 3C76C, A2             45 F9 00 03 C7 6C 
0003C4CE: lea 3DCAC, A3             47 F9 00 03 DC AC 
0003C4D4: move.w (A2)+, D4          38 1A 
0003C4D6: cmp.w #$63, D4            0C 44 00 63 
0003C4DA: beq.s 3C510               67 34 
0003C4DC: move.w (0,A3D4.w), D0     30 33 40 00 
0003C4E0: move.w (2,A3D4.w), D1     32 33 40 02 
0003C4E4: move.w (A2)+, D4          38 1A 
0003C4E6: move.w (0,A3D4.w), D2     34 33 40 00 
0003C4EA: move.w (2,A3D4.w), D3     36 33 40 02 
0003C4EE: addi.w #$C0, D0           06 40 00 C0 
0003C4F2: addi.w #$C0, D2           06 42 00 C0 
0003C4F6: addi.w #$64, D1           06 41 00 64 
0003C4FA: addi.w #$64, D3           06 43 00 64 
0003C4FE: cmp.w D0, D2              B4 40 
0003C500: bne.s 3C506               66 04 
0003C502: cmp.w D1, D3              B6 41 
0003C504: beq.s 3C50C               67 06 
0003C506: jsr 3C81C                 4E B9 00 03 C8 1C 
0003C50C: bra.s 3C4D4               60 00 FF C6 
0003C510: rts                       4E 75 

0003C512: lea 3C602, A3             47 F9 00 03 C6 02 
0003C518: lea 3DCAC, A4             49 F9 00 03 DC AC 
0003C51E: cmp.w #$63, (A3)          0C 53 00 63 
0003C522: beq.s 3C52C               67 08 
0003C524: move.l (A3)+, (A4)+       28 DB 
0003C526: move.w (A3)+, (A4)+       38 DB 
0003C528: bra.s 3C51E               60 00 FF F4 
0003C52C: move.w #$63, (A4)         38 BC 00 63 
0003C530: rts                       4E 75 

0003C532: lea 3DCAC, A4             49 F9 00 03 DC AC 
0003C538: lea 3C8C6, A6             4D F9 00 03 C8 C6 
0003C53E: lea 3C80E, A5             4B F9 00 03 C8 0E 
0003C544: bsr 3C5A4                 61 5E 
0003C546: bsr 3C570                 61 28 
0003C548: addq.l #6, A4             5C 8C 
0003C54A: cmp.w #$63, (A4)          0C 54 00 63 
0003C54E: bne.s 3C544               66 F4 
0003C550: rts                       4E 75 

0003C552: lea 3DDC0, A4             49 F9 00 03 DD C0 
0003C558: lea 3C8C6, A6             4D F9 00 03 C8 C6 
0003C55E: lea 3C814, A5             4B F9 00 03 C8 14 
0003C564: bsr 3C5A4                 61 3E 
0003C566: addq.l #6, A4             5C 8C 
0003C568: cmp.w #$63, (A4)          0C 54 00 63 
0003C56C: bne.s 3C564               66 F6 
0003C56E: rts                       4E 75 
0003C570: move.w ($0,A4), D0        30 2C 00 00 
0003C574: move.w ($2,A4), D1        32 2C 00 02 
0003C578: move.w ($4,A4), D2        34 2C 00 04 
0003C57C: move.w 3C81A, D6          3C 39 00 03 C8 1A 
0003C582: muls D6, D2               C5 C6 
0003C584: swap  D2                  48 42 
0003C586: asl.w  #6, D2             ED 42 
0003C588: sub.w  D2, D6             9C 42 
0003C58A: muls D6, D0               C1 C6 
0003C58C: muls D6, D1               C3 C6 
0003C58E: muls D6, D2               C5 C6 
0003C590: swap  D0                  48 40 
0003C592: swap  D1                  48 41 
0003C594: swap  D2                  48 42 
0003C596: move.w D0, ($0,A4)        39 40 00 00 
0003C59A: move.w D1, ($2,A4)        39 41 00 02 
0003C59E: move.w D2, ($4,A4)        39 42 00 04 
0003C5A2: rts                       4E 75 
0003C5A4: lea ($0,A4), A1           43 EC 00 00 
0003C5A8: lea ($2,A4), A2           45 EC 00 02 
0003C5AC: move.w ($4,A5), D0        30 2D 00 04 
0003C5B0: bsr 3C5D0                 61 1E 
0003C5B2: lea ($2,A4), A1           43 EC 00 02 
0003C5B6: lea ($4,A4), A2           45 EC 00 04 
0003C5BA: move.w ($0,A5), D0        30 2D 00 00 
0003C5BE: bsr 3C5D0                 61 10 
0003C5C0: lea ($0,A4), A1           43 EC 00 00 
0003C5C4: lea ($4,A4), A2           45 EC 00 04 
0003C5C8: move.w ($2,A5), D0        30 2D 00 02 
0003C5CC: bsr 3C5D0                 61 02 
0003C5CE: rts                       4E 75 
0003C5D0: move.w (7E,A6D0.w), D1    32 36 00 7E 
0003C5D4: move.w (FFFFFFFE,A6D0.w), D234 36 00 FE 
0003C5D8: move.w (A1), D3           36 11 
0003C5DA: move.w (A2), D4           38 12 



0003C3E0: and.b A6, D4              C8 0E 
0003C3E2: lea 3C814, A2             45 F9 00 03 C8 14 
0003C3E8: lea DFF000, A5            4B F9 00 DF F0 00 
0003C3EE: moveq #$0, D0             70 00 
0003C3F0: nop                       4E 71 
0003C3F2: ori.b #$F, D0             00 00 00 0F 
0003C3F6: addi.w #$2, D0            06 40 00 02 
0003C3FA: add.w D0, ($2,A3)         D1 6B 00 02 


