//search
struct command_info cmd_info[] =

//add before
#ifdef ENABLE_IKASHOP_RENEWAL
ACMD(do_offshop_force_close_shop);
#endif

//search
	{ "\n",		NULL,			0,			POS_DEAD,	GM_IMPLEMENTOR	}

//add before
#ifdef ENABLE_IKASHOP_RENEWAL
	{ "offshop_force_close_shop", do_offshop_force_close_shop, 0,  POS_DEAD, GM_IMPLEMENTOR },
#endif