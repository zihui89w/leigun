void cf_add(void);
void cf_adda(void);
void cf_addi(void);
void cf_addq(void);
void cf_addx(void);
void cf_and(void);
void cf_andi(void);
void cf_asri(void);
void cf_asrr(void);
void cf_asli(void);
void cf_aslr(void);
void cf_bcc(void);
void cf_bchgi(void);
void cf_bchgr(void);
void cf_bclri(void);
void cf_bclrr(void);
void cf_bitrev(void);
void cf_bseti(void);
void cf_bsetr(void);
void cf_bsr(void);
void cf_btsti(void);
void cf_btstr(void);
void cf_byterev(void);
void cf_clr(void);
void cf_cmp(void);
void cf_cmpa(void);
void cf_cmpi(void);
void cf_divs_w(void);
void cf_div_l(void);
void cf_divu_w(void);
void cf_eor(void);
void cf_eori(void);
void cf_ext(void);
void cf_ff1(void);
void cf_illegal(void);
void cf_jmp(void);
void cf_jsr(void);
void cf_lea(void);
void cf_link(void);
void cf_lsri(void);
void cf_lsrr(void);
void cf_lsli(void);
void cf_lslr(void);
void cf_mov3q(void);
void cf_move(void);
void cf_movea(void);
void cf_movem(void);
void cf_moveq(void);
void cf_movefccr(void);
void cf_movetccr(void);
void cf_mulsw(void);
void cf_mull(void);
void cf_muluw(void);
void cf_mvs(void);
void cf_mvz(void);
void cf_neg(void);
void cf_negx(void);
void cf_nop(void);
void cf_not(void);
void cf_or(void);
void cf_ori(void);
void cf_pea(void);
void cf_pulse(void);
void cf_rts(void);
void cf_sats(void);
void cf_scc(void);
void cf_sub(void);
void cf_suba(void);
void cf_subi(void);
void cf_subq(void);
void cf_subx(void);
void cf_swap(void);
void cf_tas(void);
void cf_tpf(void);
void cf_trap(void);
void cf_tst(void);
void cf_unlk(void);
void cf_wddata(void);
void cf_macsac(void);
void cf_macsacl(void);
void cf_movfromacc(void);
void cf_movfrommacsr(void);
void cf_movfrommask(void);
void cf_movmacsrtoccr(void);
void cf_movtoacc(void);
void cf_movtomacsr(void);
void cf_movtomask(void);
void cf_maaac(void);
void cf_cpushl(void);
void cf_frestore(void);
void cf_fsave(void);
void cf_halt(void);
void cf_intouch(void);
void cf_movefromsr(void);
void cf_movfromusp(void);
void cf_movetosr(void);
void cf_movetousp(void);
void cf_movec(void);
void cf_rte(void);
void cf_strldsr(void);
void cf_stop(void);
void cf_wdebug(void);
void cf_wdebug_ctrl(void);
void cf_undefined(void);

void cf_init_condition_tab(void);
