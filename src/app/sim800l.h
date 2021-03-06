/*
 * Copyright (C) 2019 Francois Berder <fberder@outlook.fr>
 *
 * This file is part of waterpump.
 *
 * waterpump is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * waterpump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with waterpump.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SIM800L_H
#define SIM800L_H

#include "mcu/stm32l051xx.h"
#include <stdint.h>

#define SIM800L_ALARM_COUNT     (5)

struct sim800l_params_t {
    USART_TypeDef *dev;
};

enum sim800l_sim_status_t {
    SIM_READY,
    SIM_PIN_LOCK,
    SIM_PUK_LOCK,
    SIM_ERROR,
};

enum sim800l_network_registration_status_t {
    SIM800_NOT_REGISTERED,
    SIM800_REGISTERING,
    SIM800_REGISTERED,
    SIM800_ROAMING,
};

struct __attribute__((packed)) sim800l_sms_header_t {
    uint64_t time;
    uint8_t index;
    char sender[22];
};

struct sim800l_sms_t {
    struct sim800l_sms_header_t header;
    uint8_t text_length;
    char text[256];
};

typedef void(*sim800l_receive_sms_callback_t)(struct sim800l_sms_t *);

typedef void(*sim800l_alarm_cb_t)(uint8_t);

void sim800l_receive_cb(char c);

/**
 * @brief Check SIM card is present
 *
 * @param[in] params
 * @return 0 if SIM card is present, -1 otherwise
 */
int sim800l_check_sim_card_present(struct sim800l_params_t *params);

/**
 * @brief Get SIM status
 *
 * @param[in] params
 * @param[out] sim_status
 * @retval 0 if successful
 * @retval -1 if ca
 */
int sim800l_get_sim_status(struct sim800l_params_t *params, enum sim800l_sim_status_t *sim_status);

/**
 * @brief Unlock SIM card using PIN code
 *
 * @param[in] params
 * @param[in] pin 0..9999
 * @retval 0 if successful
 * @retval -1 if card could not be unlocked
 */
int sim800l_unlock_sim(struct sim800l_params_t *params, uint32_t pin);

/**
 * @brief Check registration status
 *
 * @param[in] params
 * @param[out] status
 * @retval 0 if command completed with success
 * @retval -1 if an error occurred
 */
int sim800l_check_network_registration(struct sim800l_params_t *params, enum sim800l_network_registration_status_t *status);

/**
 * @brief Connect to network
 *
 * @param[in] params
 * @retval 0 if SIM800L module is registered to the network
 * @retval -1 if an error occurred
 */
int sim800l_connect_to_network(struct sim800l_params_t *params);

/**
 * @brief Set SMS format to text
 *
 * @param[in] params
 * @retval 0 if SMS format is set to text
 * @retval -1 if an error occurred
 */
int sim800l_set_sms_format_to_text(struct sim800l_params_t *params);

/**
 * @brief Use SIM card to store SMS
 *
 * @param[in] params
 * @retval 0 if storage for SMS could not be set
 * @retval -1 if an error occurred
 */
int sim800l_use_simcard_for_sms_storage(struct sim800l_params_t *params);

/**
 * @brief Delete all SMS stored on SIM card
 *
 * @param[in] params
 *
 * @retval 0 if all SMS on SIM card were deleted
 * @retval -1 if an error occurred
 */
int sim800l_delete_all_sms(struct sim800l_params_t *params);

/**
 * @brief Delete all received SMS stored on SIM card
 *
 * @param[in] params
 *
 * @retval 0 if all received SMS on SIM card were deleted
 * @retval -1 if an error occurred
 */
int sim800l_delete_all_received_sms(struct sim800l_params_t *params);

/**
 * @brief Delete a SMS stored on SIM card
 *
 * @param[in] params
 * @param[in] index
 * @retval 0 if SMS on SIM card was deleted
 * @retval -1 if an error occurred
 */
int sim800l_delete_sms(struct sim800l_params_t *params, uint8_t index);

/**
 * @brief Read all unread SMS stored on SIM card
 *
 * @param[in] params
 * @param[in] cb
 * @retval 0 if all unread SMS on SIM card were read
 * @retval -1 if an error occurred
 */
int sim800l_read_all_unread_sms(struct sim800l_params_t *params, sim800l_receive_sms_callback_t cb);

/**
 * @brief Send SMS
 *
 * @param[in] params
 * @param[in] dest
 * @param[in] text
 * @return 0 if successful, -1 otherwise
 */
int sim800l_send_sms(struct sim800l_params_t *params, const char *dest, const char *text);

/**
 * @brief Enable time update from network
 *
 * @param[in] params
 * @return 0 if successful, -1 otherwise
 */
int sim800l_enable_time_update_from_network(struct sim800l_params_t *params);

/**
 * @brief Get time from SIM800 RTC
 *
 * @param[in] params
 * @param[out] time
 * @return 0 if successful, -1 otherwise
 */
int sim800l_get_time(struct sim800l_params_t *params, uint64_t *time);

/**
 * @brief Synchronize SIM800 time with network time
 */
int sim800l_sync_time(struct sim800l_params_t *params);

/**
 * @brief Delete alarm
 *
 * @param[in] params
 * @param[in] alarm_index must be in range 1..5
 * @return 0 if successful, -1 otherwise
 */
int sim800l_delete_alarm(struct sim800l_params_t *params, uint8_t alarm_index);

/**
 * @brief Set alarm
 *
 * Note that if you want to change the time of an alarm, you must first
 * delete it.
 *
 * The alarm will trigger every day.
 *
 * @param[in] params
 * @param[in] alarm_index must be in range 1..5
 * @param[in] hour in BCD format
 * @param[in] min in BCD format
 * @param[in] sec in BCD format
 * @param[in] cb
 * @return 0 if successful, -1 otherwise
 */
int sim800l_set_alarm(struct sim800l_params_t *params, uint8_t alarm_index,
                      uint8_t hour, uint8_t min, uint8_t sec,
                      sim800l_alarm_cb_t cb);

#endif
