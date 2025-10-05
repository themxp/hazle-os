#include "rtc.h"
#include "io.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define RTC_SECOND 0x00
#define RTC_MINUTE 0x02
#define RTC_HOUR 0x04
#define RTC_DAY 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09
#define RTC_CENTURY 0x32
#define RTC_STATUS_A 0x0A
#define RTC_STATUS_B 0x0B

static uint8_t rtc_read_register(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

static bool rtc_is_updating(void) {
    outb(CMOS_ADDRESS, RTC_STATUS_A);
    return (inb(CMOS_DATA) & 0x80) != 0;
}

static uint8_t bcd_to_binary(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void rtc_init(void) {
}

void rtc_read_time(rtc_time_t* time) {
    if (!time) return;
    
    while (rtc_is_updating());
    
    uint8_t second = rtc_read_register(RTC_SECOND);
    uint8_t minute = rtc_read_register(RTC_MINUTE);
    uint8_t hour = rtc_read_register(RTC_HOUR);
    uint8_t day = rtc_read_register(RTC_DAY);
    uint8_t month = rtc_read_register(RTC_MONTH);
    uint8_t year = rtc_read_register(RTC_YEAR);
    uint8_t century = rtc_read_register(RTC_CENTURY);
    
    uint8_t status_b = rtc_read_register(RTC_STATUS_B);
    bool bcd_mode = !(status_b & 0x04);
    
    if (bcd_mode) {
        second = bcd_to_binary(second);
        minute = bcd_to_binary(minute);
        hour = bcd_to_binary(hour);
        day = bcd_to_binary(day);
        month = bcd_to_binary(month);
        year = bcd_to_binary(year);
        century = bcd_to_binary(century);
    }
    
    time->second = second;
    time->minute = minute;
    time->hour = hour;
    time->day = day;
    time->month = month;
    time->year = (century * 100) + year;
}

uint8_t rtc_get_second(void) {
    while (rtc_is_updating());
    uint8_t val = rtc_read_register(RTC_SECOND);
    uint8_t status_b = rtc_read_register(RTC_STATUS_B);
    return (status_b & 0x04) ? val : bcd_to_binary(val);
}

uint8_t rtc_get_minute(void) {
    while (rtc_is_updating());
    uint8_t val = rtc_read_register(RTC_MINUTE);
    uint8_t status_b = rtc_read_register(RTC_STATUS_B);
    return (status_b & 0x04) ? val : bcd_to_binary(val);
}

uint8_t rtc_get_hour(void) {
    while (rtc_is_updating());
    uint8_t val = rtc_read_register(RTC_HOUR);
    uint8_t status_b = rtc_read_register(RTC_STATUS_B);
    return (status_b & 0x04) ? val : bcd_to_binary(val);
}

uint8_t rtc_get_day(void) {
    while (rtc_is_updating());
    uint8_t val = rtc_read_register(RTC_DAY);
    uint8_t status_b = rtc_read_register(RTC_STATUS_B);
    return (status_b & 0x04) ? val : bcd_to_binary(val);
}

uint8_t rtc_get_month(void) {
    while (rtc_is_updating());
    uint8_t val = rtc_read_register(RTC_MONTH);
    uint8_t status_b = rtc_read_register(RTC_STATUS_B);
    return (status_b & 0x04) ? val : bcd_to_binary(val);
}

uint16_t rtc_get_year(void) {
    while (rtc_is_updating());
    uint8_t year = rtc_read_register(RTC_YEAR);
    uint8_t century = rtc_read_register(RTC_CENTURY);
    uint8_t status_b = rtc_read_register(RTC_STATUS_B);
    
    if (!(status_b & 0x04)) {
        year = bcd_to_binary(year);
        century = bcd_to_binary(century);
    }
    
    return (century * 100) + year;
}

