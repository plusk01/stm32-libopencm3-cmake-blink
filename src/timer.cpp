#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#ifdef __cplusplus
}
#endif

#ifndef ARRAY_LEN
#define ARRAY_LEN(array) (sizeof((array))/sizeof((array)[0]))
#endif

#define LED1_PORT GPIOI
#define LED1_PIN GPIO0
#define LED1_AF GPIO_AF2
#define LED1_TIM TIM5
#define LED1_OC TIM_OC4
#define LED1_RCC RCC_TIM5
#define LED1_NVIC NVIC_TIM5_IRQ
#define LED1_RST RST_TIM5
#define LED1_ISR tim5_isr
#define LED1_CCIF TIM_SR_CC4IF
#define LED1_CCIE TIM_DIER_CC4IE


static void tim_setup(void)
{
  // see http://www.micromouseonline.com/2016/02/06/pwm-basics-on-the-stm32-general-purpose-timers/

  static constexpr uint32_t hse_mhz = 16;
  rcc_clock_setup_hse(&rcc_3v3[RCC_CLOCK_3V3_168MHZ], hse_mhz);

  /* Enable GPIO clock for leds. */
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOI);
  rcc_periph_clock_enable(RCC_GPIOH);


  /* Enable TIM5 clock. */
  rcc_periph_clock_enable(LED1_RCC);

  /* Enable TIM5 interrupt. */
  nvic_enable_irq(LED1_NVIC);

  /* Reset TIM5 peripheral to defaults. */
  rcc_periph_reset_pulse(LED1_RST);

  /* Enable led as output */
  gpio_mode_setup(LED1_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LED1_PIN);
  gpio_set(LED1_PORT, LED1_PIN);
  gpio_set_output_options(LED1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, LED1_PIN);
  gpio_set_af(LED1_PORT, LED1_AF, LED1_PIN);

  /* Timer global mode:
   * - No divider
   * - Alignment edge
   * - Direction up
   * (These are actually default values after reset above, so this call
   * is strictly unnecessary, but demos the api for alternative settings)
   */
  // timer_set_mode(LED1_TIM, TIM_CR1_CKD_CK_INT,
  //   TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

  /*
   * Please take note that the clock source for STM32 timers
   * might not be the raw APB1/APB2 clocks.  In various conditions they
   * are doubled.  See the Reference Manual for full details!
   * In our case, TIM5 on APB1 is running at double frequency, so this
   * sets the prescaler to have the timer run at 5kHz
   */
  timer_set_prescaler(LED1_TIM, ((rcc_apb1_frequency * 2) / 10000) - 1);

  /* Disable preload. */
  timer_disable_preload(LED1_TIM);
  timer_continuous_mode(LED1_TIM);
  // timer_one_shot_mode(LED1_TIM);

  /* count full range, as we'll update compare value continuously */
  timer_set_period(LED1_TIM, 100 - 1);

  // timer_enable_break_main_output(LED1_TIM);

  /* Set the initual output compare value for OC1. */
  timer_set_oc_value(LED1_TIM, LED1_OC, 2);

  timer_set_oc_mode(LED1_TIM, LED1_OC, TIM_OCM_PWM2); // PWM2 because active low
  timer_enable_oc_output(LED1_TIM, LED1_OC);

  /* Counter enable. */
  timer_enable_counter(LED1_TIM);

  /* Enable Channel 1 compare interrupt to recalculate compare values */
  // timer_enable_irq(LED1_TIM, LED1_CCIE);
}

void LED1_ISR(void)
{
  if (timer_get_flag(LED1_TIM, LED1_CCIF)) {

    /* Clear compare interrupt flag. */
    timer_clear_flag(LED1_TIM, LED1_CCIF);

    /*
     * Get current timer value to calculate next
     * compare register value.
     */
    uint16_t compare_time = timer_get_counter(LED1_TIM);

    /* Calculate and set the next compare value. */
    uint16_t frequency = 5000;
    uint16_t new_time = compare_time + frequency;

    timer_set_oc_value(LED1_TIM, LED1_OC, new_time);

    /* Toggle LED to indicate compare event. */
    // gpio_toggle(LED1_PORT, LED1_PIN);
  }
}

int main(void)
{
  tim_setup();

  while (1) {
    ;
  }

  return 0;
}