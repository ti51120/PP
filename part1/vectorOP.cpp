#include "PPintrin.h"
#include <iostream>

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
		
    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  __pp_vec_float val, result, nine = _pp_vset_float(9.999999f);
  __pp_vec_int e, zero = _pp_vset_int(0), ones = _pp_vset_int(1); 
  __pp_mask maskAll, maskIsZero, maskIsNotZero, maskIsGt9;
  
	for (int i = 0; i < N; i += VECTOR_WIDTH)
  {		
		if(N - i < VECTOR_WIDTH)
			maskAll = _pp_init_ones(N-i);
		else
			maskAll = _pp_init_ones();
    
		_pp_vload_float(val, values + i, maskAll); // load value[]
    _pp_vload_int(e, exponents + i, maskAll); // load exponents[]

    _pp_veq_int(maskIsZero, e, zero, maskAll); // mask for 0 exponent
		_pp_vset_float(result, 1.f, maskIsZero); // set value[] to 1 if 0 exponent
		
    maskIsNotZero = _pp_mask_not(maskIsZero); // mask for non-zero exponent
		_pp_vsub_int(e, e, ones, maskIsNotZero);
		_pp_vmove_float(result, val, maskIsNotZero);
		_pp_veq_int(maskIsZero, e, zero, maskAll);
		maskIsNotZero = _pp_mask_not(maskIsZero);

		while(_pp_cntbits(maskIsZero) != VECTOR_WIDTH){
			_pp_vmult_float(result, result, val, maskIsNotZero);
			_pp_vsub_int(e, e, ones, maskIsNotZero);
			_pp_veq_int(maskIsZero, e, zero, maskAll);
			maskIsNotZero = _pp_mask_not(maskIsZero);
			if(_pp_cntbits(maskIsZero) == N) break;
		}

		_pp_vgt_float(maskIsGt9, result, nine, maskAll);
		_pp_vset_float(result, 9.999999f, maskIsGt9);

    _pp_vstore_float(output + i, result, maskAll);
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //

	__pp_vec_float res;
	__pp_mask maskAll = _pp_init_ones();
	float ans = 0.0;

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {	
		int round = VECTOR_WIDTH;
		_pp_vload_float(res, values+i, maskAll);

		while(round != 1){
			_pp_hadd_float(res, res);
			_pp_interleave_float(res, res);
			round /= 2;
		}
		ans += res.value[0];
  }

  return ans;
}