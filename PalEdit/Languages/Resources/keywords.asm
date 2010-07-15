aaa aad aam adc add addl addpd addps addsd addss and andl andnpd andnps andpd 
andps arpl 
bound bsf bsr bswap bt btc btr bts 
call cbw cdq clc cld clflush cli clts cmc cmp cmppd cmpps cmps cmpsb cmpsd 
cmpss cmpsw cmpxchg comisd comiss cpuid cvtdq2pd cvtdq2ps cvtpd2dq cvtpd2pi 
cvtpd2ps cvtpi2pd cvtpi2ps cvtps2dq cvtps2pd cvtps2pi cvtsd2si cvtsd2ss 
cvtsi2sd cvtsi2ss cvtss2sd cvtss2si cvttpd2dq cvttpd2pi cvttps2dq cvttsd2si cwd 
cwde 
daa das dec div divpd divps divsd divss 
emms enter 
f2xm1 fabs fadd faddp fbld fbstp fchs fclex fcmovb fcmovbe fcmove fcmovnb 
fcmovnbe fcmovne fcmovnu fcmovu fcom fcomi fcomip fcomp fcompp fcos fdecstp 
fdiv fdivp fdivr fdivrp femms ffree fiadd ficom ficomp fidiv fidivr fild fimul 
fincstp finit fist fistp fisub fisubr fld fld1 fldcw fldenv fldl2e fldl2t 
fldlg2 fldln2 fldpi fldz fmul fmulp fnclex fninit fnop fnsave fnstcw fnstenv 
fnstsw fpatan fprem1 fptan frndint frstor fsave fscale fsin fsincos fsqrt fst 
fstcw fstenv fstp fstsw fsub fsubp fsubr fsubrp ftst fucom fucomi fucomip 
fucomp fucompp fwait fxch fxrstor fxsave fxtract fyl2xp1 
hlt 
idiv imul in inc incl ins insb insd insw int into invd invlpg iret iretd iretw 
ja jae jb jbe jc jcxz je jecxz jg jge jl jle jmp jna jnae jnb jnbe jnc jne jng 
jnge jnl jnle jno jnp jns jnz jo jp jpe jpo js jz 
lahf lar ldmxcsr lds lea leal leave les lfence lfs lgdt lgs lidt lldt lmsw lock 
lods lodsb lodsd lodsw loop loope loopne loopnz loopz lsl lss ltr 
maskmovdqu maskmovq maxpd maxps maxsd maxss mfence minpd minps minsd minss mov 
movapd movaps movb movd movdq2q movdqa movdqu movhlps movhpd movhps movl 
movlhps movlpd movlps movmskpd movmskps movntdq movnti movntpd movntps movntq 
movq movq2dq movs movsb movsd movss movsw movsx movupd movups movw movzwl movzx 
mul mulpd mulps mulsd mulss 
neg nop not 
or orpd orps out outs outsb outsd outsw 
packssdw packsswb packuswb paddb paddd paddq paddsb paddsw paddusb paddusw 
paddw pand pandn pause pavgb pavgsub pavgusb pavgw pcmpeqb pcmpeqd pcmpeqw 
pcmpgtb pcmpgtd pcmpgtw pextrw pf2id pf2iw pfacc pfadd pfcmpeq pfcmpge pfcmpgt 
pfmax pfmin pfmul pfnacc pfpnacc pfrcp pfrcpit1 pfrcpit2 pfrsqit1 pfrsqrt pfsub 
pfsubr pi2fd pi2fw pinsrw pmaddwd pmaxsw pmaxub pminsw pminub pmovmskb pmulhrw 
pmulhuw pmulhw pmullw pmuludq pop popa popad popaw popf popfd popfw popl por 
prefetch prefetchnta prefetcht0 prefetcht1 prefetcht2 prefetchw psaddbw pshufd 
pshufhw pshuflw pshufw pslld pslldq psllq psllw psrad psraw psrld psrldq psrlq 
psrlw psubb psubd psubq psubsb psubsw psubusb psubusw psubw pswapd punpckhbw 
punpckhdq punpckhqdq punpckhwd punpcklbw punpckldq punpcklqdq punpcklwd push 
pusha pushad pushaw pushf pushfd pushfw pushl pxor 
rcl rcpps rcpss rcr rep repe repne repnz repz ret rol ror rsqrtps rsqrtss 
sahf sal sar sbb scas scasb scasd scasw seta setae setb setbe setc sete setg 
setge setl setle setna setnae setnb setnbe setnc setne setng setnge setnl 
setnle setno setnp setns setnz seto setp setpo sets setz sfence sgdt shl shld 
shll shr shrd shrl shufpd shufps sidt sldt smsw sqrtpd sqrtps sqrtsd sqrtss stc 
std sti stmxcsr stos stosb stosd stosw str sub subl subpd subps subsd subss 
test testl 
ucomisd ucomiss unpckhpd unpckhps unpcklpd unpcklps 
verr verw 
wait wbinvd 
xadd xchg xlat xlatb xor xorl xorpd xorps 
- User set 1:
bbs
code
data
text
- User set 2:
ax bx cx dx ex si di bp sp ss es ds cs ip
al ah bl bh ch cl dh dl eh el
eax ebx ebp ecx edi edx esi esp
mm0 mm1 mm2 mm3 mm4 mm5 mm6 mm7
xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7
- User set 3:
absolute align alignb at
bits
common
dword
end endstruc extern externdef
file
globl global globaldef
istruc iend
long
org
qword
short section segment segment_data segment_code size string struc
times type tword
word
- User set 4:
assign
byte
db dd define dq dt dw
else endif endmacro endrep equ error
if ifdef incbin include ifctx ifid ifidn ifidni ifnum ifstr
macro
undef
repl resb resw resq rotate
seg
wrt