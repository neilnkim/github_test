/**
 * \file
 *
 * \brief ADC Basic driver declarations.
 *
 (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

#ifndef ADC_BASIC_H_INCLUDED
#define ADC_BASIC_H_INCLUDED

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*adc_irq_cb_t)(void);

//ADC 떨림 안정화를 위한 Sampling 1회 -> 4회 수정
#define ADC_NONE  0x00  //kevin.ko 170705 SWT-461
#define ADC_AAC02 0x01 //01 results accumulated.
#define ADC_AAC04 0x02 //04 results accumulated.
#define ADC_AAC08 0x03 //08 results accumulated.
#define ADC_AAC16 0x04 //16 results accumulated.
#define ADC_AAC32 0x05 //32 results accumulated.
#define ADC_AAC64 0x06 //64 results accumulated.

#define ADC_AAC ADC_AAC04

#if (ADC_AAC == ADC_NONE)
	#define ADC_DEVIDE 1
#elif (ADC_AAC == ADC_AAC02)
	#define ADC_DEVIDE 2
#elif  (ADC_AAC == ADC_AAC04)
	#define ADC_DEVIDE 4
#elif  (ADC_AAC == ADC_AAC08)
	#define ADC_DEVIDE 8
#elif  (ADC_AAC == ADC_AAC16)
	#define ADC_DEVIDE 16
#elif (ADC_AAC == ADC_AAC32)
	#define ADC_DEVIDE 32
#elif (ADC_AAC == ADC_AAC64)
	#define ADC_DEVIDE 64
#else
	#define ADC_DEVIDE 1
#endif


/** Datatype for the result of the ADC conversion */
typedef uint16_t adc_result_t;

//* Analog channel selection */
typedef ADC_MUXPOS_t adc_0_channel_t;

int8_t ADC_0_init();

void ADC_0_enable();

void ADC_0_disable();

void ADC_0_start_conversion(adc_0_channel_t channel);

bool ADC_0_is_conversion_done();

adc_result_t ADC_0_get_conversion_result(void);

adc_result_t ADC_0_get_conversion(adc_0_channel_t channel);

uint8_t ADC_0_get_resolution();

#ifdef __cplusplus
}
#endif

#endif /* ADC_BASIC_H_INCLUDED */
