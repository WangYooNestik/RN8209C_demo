#ifndef _RN8209_REG_CFG_H_
#define _RN8209_REG_CFG_H_





typedef enum{
	SC_UNLOCK = 0xe5,
	SC_LOCK = 0xdc,
	SC_SLC_CH_A = 0x5a,
	SC_SLC_CH_B = 0xa5,
	SC_RESET = 0xfa,
}EN_RN8209_SC;//System Control

#define IF_RN8209_SC_OK(SysCtl) if(!(SysCtl == SC_UNLOCK \
									|| SysCtl == SC_LOCK \
									|| SysCtl == SC_SLC_CH_A \
									|| SysCtl == SC_SLC_CH_B \
									|| SysCtl == SC_RESET)) return


#endif

