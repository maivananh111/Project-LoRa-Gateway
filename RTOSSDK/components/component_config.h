/*
 * component_config.h
 *
 *  Created on: Mar 25, 2023
 *      Author: anh
 */

#ifndef COMPONENTS_COMPONENT_CONFIG_H_
#define COMPONENTS_COMPONENT_CONFIG_H_


#define ENABLE_COMPONENT_CRC      		1

#define ENABLE_COMPONENT_DS3231   		0

#define ENABLE_COMPONENT_SPIFLASH       0

#define ENABLE_COMPONENT_SX127X_LORA    1

#define ENABLE_COMPONENT_PARSE_PACKET   1
#if ENABLE_COMPONENT_PARSE_PACKET
#define ENABLE_COMPONENT_PARSE_PACKET_DEBUG 1
#endif /* ENABLE_COMPONENT_PARSE_PACKET */

#define ENABLE_COMPONENT_WIFIIF         1
#if ENABLE_COMPONENT_WIFIIF
#define ENABLE_COMPONENT_WIFIIF_DEBUG   1
#endif /* ENABLE_COMPONENT_WIFIIF */

#if ENABLE_COMPONENT_SX127X_LORA
#define ENABLE_COMPONENT_LORAIF         1
#if ENABLE_COMPONENT_LORAIF
#define ENABLE_COMPONENT_LORAIF_DEBUG   1
#endif /* ENABLE_COMPONENT_LORAIF */

#endif /* ENABLE_COMPONENT_SX127X_LORA */

#endif /* COMPONENTS_COMPONENT_CONFIG_H_ */
