#include "process_fcns.h"


/*******************************************************************************
 * Calculate output of All-Pass Filter of the 1-st order
 *
 * @param[in]  params       pointer to the pre-allocated params
 * @param[in]  coeffs       pointer to the pre-allocated coeffs
 * @param[in]  sample_rate  sampling rate
 *
 * @return apf_order_1_out
 ******************************************************************************/
my_float APF_order_1(my_float in, crossover_coeffs* coeffs, crossover_states* states)
{

}

/*******************************************************************************
 * Initializes params, coeffs and states to default values for the requested SR.
 *
 * @param[in]  params       pointer to the pre-allocated params
 * @param[in]  coeffs       pointer to the pre-allocated coeffs
 * @param[in]  sample_rate  sampling rate
 *
 * @return 0 if effect is initialized, non-zero error code otherwise
 ******************************************************************************/
float APF_order_2()
{

}

/*******************************************************************************
 * Initializes params, coeffs and states to default values for the requested SR.
 *
 * @param[in]  params       pointer to the pre-allocated params
 * @param[in]  coeffs       pointer to the pre-allocated coeffs
 * @param[in]  sample_rate  sampling rate
 *
 * @return 0 if effect is initialized, non-zero error code otherwise
 ******************************************************************************/
my_float APF_parallel()
{

}

/*******************************************************************************
 * Initializes params, coeffs and states to default values for the requested SR.
 *
 * @param[in]  params       pointer to the pre-allocated params
 * @param[in]  coeffs       pointer to the pre-allocated coeffs
 * @param[in]  sample_rate  sampling rate
 *
 * @return 0 if effect is initialized, non-zero error code otherwise
 ******************************************************************************/
float APF_series()
{

}

/*******************************************************************************
 * Initializes params, coeffs and states to default values for the requested SR.
 *
 * @param[in]  params       pointer to the pre-allocated params
 * @param[in]  coeffs       pointer to the pre-allocated coeffs
 * @param[in]  sample_rate  sampling rate
 *
 * @return 0 if effect is initialized, non-zero error code otherwise
 ******************************************************************************/
band_split APF_cascade(tStereo* in, band_coeffs* coeffs, apf_casc* states)
{
   /* band_coeffs* casc_coeffs = (band_coeffs*)coeffs;
    apf_casc* casc_states = (apf_casc*)states;
    tstereo*

    band_split casc_1;
    my_float accum;

    accum = APF_parallel();*/


}
