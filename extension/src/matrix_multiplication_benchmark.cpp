#include<benchmark/benchmark.h>
#if (defined __ARM_NEON) || (defined __ARM_NEON__)
#include<arm_neon.h>

// assumed a column-major layout of the matrices in memory.
// That is, an n x m matrix M is represented as an array M_array where Mij = M_array[n*j + i].
void matrix_multiply_c(float32_t *A, float32_t *B, float32_t *C, uint32_t n, uint32_t m, uint32_t k) {
    for (int i_idx=0; i_idx < n; i_idx++) {
        for (int j_idx=0; j_idx < m; j_idx++) {
            C[n*j_idx + i_idx] = 0;
            for (int k_idx=0; k_idx < k; k_idx++) {
                C[n*j_idx + i_idx] += A[n*k_idx + i_idx]*B[k*j_idx + k_idx];
            }
        }
    }
}

void matrix_multiply_4x4_neon(float32_t *A, float32_t *B, float32_t *C) {
	// these are the columns A
	float32x4_t A0;
	float32x4_t A1;
	float32x4_t A2;
	float32x4_t A3;
	
	// these are the columns B
	float32x4_t B0;
	float32x4_t B1;
	float32x4_t B2;
	float32x4_t B3;
	
	// these are the columns C
	float32x4_t C0;
	float32x4_t C1;
	float32x4_t C2;
	float32x4_t C3;
	
	A0 = vld1q_f32(A);
	A1 = vld1q_f32(A+4);
	A2 = vld1q_f32(A+8);
	A3 = vld1q_f32(A+12);
	
	// Zero accumulators for C values
	C0 = vmovq_n_f32(0);
	C1 = vmovq_n_f32(0);
	C2 = vmovq_n_f32(0);
	C3 = vmovq_n_f32(0);
	
	// Multiply accumulate in 4x1 blocks, i.e. each column in C
	B0 = vld1q_f32(B);
	C0 = vfmaq_laneq_f32(C0, A0, B0, 0);
	C0 = vfmaq_laneq_f32(C0, A1, B0, 1);
	C0 = vfmaq_laneq_f32(C0, A2, B0, 2);
	C0 = vfmaq_laneq_f32(C0, A3, B0, 3);
	vst1q_f32(C, C0);
	
	B1 = vld1q_f32(B+4);
	C1 = vfmaq_laneq_f32(C1, A0, B1, 0);
	C1 = vfmaq_laneq_f32(C1, A1, B1, 1);
	C1 = vfmaq_laneq_f32(C1, A2, B1, 2);
	C1 = vfmaq_laneq_f32(C1, A3, B1, 3);
	vst1q_f32(C+4, C1);
	
	B2 = vld1q_f32(B+8);
	C2 = vfmaq_laneq_f32(C2, A0, B2, 0);
	C2 = vfmaq_laneq_f32(C2, A1, B2, 1);
	C2 = vfmaq_laneq_f32(C2, A2, B2, 2);
	C2 = vfmaq_laneq_f32(C2, A3, B2, 3);
	vst1q_f32(C+8, C2);
	
	B3 = vld1q_f32(B+12);
	C3 = vfmaq_laneq_f32(C3, A0, B3, 0);
	C3 = vfmaq_laneq_f32(C3, A1, B3, 1);
	C3 = vfmaq_laneq_f32(C3, A2, B3, 2);
	C3 = vfmaq_laneq_f32(C3, A3, B3, 3);
	vst1q_f32(C+12, C3);
}

void matrix_multiply_neon(float32_t  *A, float32_t  *B, float32_t *C, uint32_t n, uint32_t m, uint32_t k) {
	/* 
	 * Multiply matrices A and B, store the result in C. 
	 * It is the user's responsibility to make sure the matrices are compatible.
	 */	

	int A_idx;
	int B_idx;
	int C_idx;
	
	// these are the columns of a 4x4 sub matrix of A
	float32x4_t A0;
	float32x4_t A1;
	float32x4_t A2;
	float32x4_t A3;
	
	// these are the columns of a 4x4 sub matrix of B
	float32x4_t B0;
	float32x4_t B1;
	float32x4_t B2;
	float32x4_t B3;
	
	// these are the columns of a 4x4 sub matrix of C
	float32x4_t C0;
	float32x4_t C1;
	float32x4_t C2;
	float32x4_t C3;
	
	for (int i_idx=0; i_idx<n; i_idx+=4) {
            for (int j_idx=0; j_idx<m; j_idx+=4){
                 // zero accumulators before matrix op
                 C0=vmovq_n_f32(0);
                 C1=vmovq_n_f32(0);
                 C2=vmovq_n_f32(0); 
                 C3=vmovq_n_f32(0);
                 for (int k_idx=0; k_idx<k; k_idx+=4){
                    // compute base index to 4x4 block
                    A_idx = i_idx + n*k_idx;
                    B_idx = k*j_idx + k_idx;

                    // load most current a values in row
                    A0=vld1q_f32(A+A_idx);
                    A1=vld1q_f32(A+A_idx+n);
                    A2=vld1q_f32(A+A_idx+2*n);
                    A3=vld1q_f32(A+A_idx+3*n);

                    // multiply accumulate 4x1 blocks, i.e. each column C
                    B0=vld1q_f32(B+B_idx);
                    C0=vfmaq_laneq_f32(C0,A0,B0,0);
                    C0=vfmaq_laneq_f32(C0,A1,B0,1);
                    C0=vfmaq_laneq_f32(C0,A2,B0,2);
                    C0=vfmaq_laneq_f32(C0,A3,B0,3);

                    B1=vld1q_f32(B+B_idx+k);
                    C1=vfmaq_laneq_f32(C1,A0,B1,0);
                    C1=vfmaq_laneq_f32(C1,A1,B1,1);
                    C1=vfmaq_laneq_f32(C1,A2,B1,2);
                    C1=vfmaq_laneq_f32(C1,A3,B1,3);

                    B2=vld1q_f32(B+B_idx+2*k);
                    C2=vfmaq_laneq_f32(C2,A0,B2,0);
                    C2=vfmaq_laneq_f32(C2,A1,B2,1);
                    C2=vfmaq_laneq_f32(C2,A2,B2,2);
                    C2=vfmaq_laneq_f32(C2,A3,B3,3);

                    B3=vld1q_f32(B+B_idx+3*k);
                    C3=vfmaq_laneq_f32(C3,A0,B3,0);
                    C3=vfmaq_laneq_f32(C3,A1,B3,1);
                    C3=vfmaq_laneq_f32(C3,A2,B3,2);
                    C3=vfmaq_laneq_f32(C3,A3,B3,3);
                }
   //Compute base index for stores
   C_idx = n*j_idx + i_idx;
   vst1q_f32(C+C_idx, C0);
   vst1q_f32(C+C_idx+n,C1);
   vst1q_f32(C+C_idx+2*n,C2);
   vst1q_f32(C+C_idx+3*n,C3);
  }
 }
}

inline void init_matrix2_by_size(float* A, float* B, int n, int m, int k){
    for(int i=0;i<n*k;++i){
        A[i] = i*i + 0.1f;
    }
    for(int i=0;i<k*m;++i){
        B[i] = i*i + 1.1f;
    }

}

static void matrix_mul4_c(benchmark::State& state){
    
    float A[16] ,B[16],C[16] = {};
    init_matrix2_by_size(A,B,4,4,4);
    for(auto _: state){
        matrix_multiply_c(A, B, C, 4, 4, 4);
    } 
}

static void matrix_mul4_n(benchmark::State& state){
    float A[16] ,B[16],C[16] = {};
    init_matrix2_by_size(A,B,4,4,4);
    for(auto _ : state){
        matrix_multiply_neon(A, B, C, 4, 4, 4);
    }
}

static void matrix_mul8_c(benchmark::State& state){
    
    float A[64] ,B[64],C[64] = {};
    init_matrix2_by_size(A,B,8,8,8);
    for(auto _: state){
        matrix_multiply_c(A, B, C, 8, 8, 8);
    } 
}

static void matrix_mul8_n(benchmark::State& state){
    float A[64] ,B[64],C[64] = {};
    init_matrix2_by_size(A,B,8,8,8);
    for(auto _ : state){
        matrix_multiply_neon(A, B, C, 8, 8, 8);
    }
}

BENCHMARK(matrix_mul4_c);
BENCHMARK(matrix_mul4_n);
BENCHMARK(matrix_mul8_c);
BENCHMARK(matrix_mul8_n);
BENCHMARK_MAIN();

#endif