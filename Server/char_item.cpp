//add where you want
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_ULTIMATE
#include "ikarus_shop_manager.h"
#endif
#endif

//search
							case UNIQUE_ITEM_CAPE_OF_COURAGE:
							case 70057:
							case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
								AggregateMonster();
								item->SetCount(item->GetCount()-1);
								break;

//or

							case UNIQUE_ITEM_WHITE_FLAG:
								ForgetMyAttacker();
								item->SetCount(item->GetCount()-1);
								break;

//add after
#ifdef ENABLE_IKASHOP_RENEWAL
#ifdef EXTEND_IKASHOP_ULTIMATE
							case 51998:
								if (ikashop::GetManager().UsePrivateShopKey(this, item))
									item->SetCount(item->GetCount() - 1);
								break;

							case 51999:
								if (ikashop::GetManager().UseSearchShopPremium(this, item))
									item->SetCount(item->GetCount() - 1);
								break;
#endif
#endif