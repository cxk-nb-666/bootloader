#ifndef MAIN_H
#define MAIN_H

#define STM32_FLASH_BASE 0x08000000UL
#define STM32_sector_Num 12
#define STM32_0_3_sector_size (16*1024)
#define STM32_4_sector_size (64*1024)
#define STM32_5_11_sector_size (128*1024)

#define STM32_B_sector_num 4
#define STM32_A_sector_num STM32_sector_Num-STM32_B_sector_num
#define STM32_A_Start_Sector STM32_B_sector_num+1
#define STM32_A_Start_Addr (uint32_t)(STM32_FLASH_BASE+STM32_0_3_sector_size)*STM32_B_sector_num




#endif 