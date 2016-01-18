#include <stdlib.h>
#include <stdio.h>
#include <fftw3.h>
#include <time.h>

int main(void) {
  double imageI[10][10];
  fftw_complex *in;
  fftw_complex *out;
  fftw_complex *out2;
  
  srand(time(NULL));
  
  in = (fftw_complex*) fftw_malloc(10 * 10 * sizeof(fftw_complex));
  out = (fftw_complex*) fftw_malloc(10 * 10 * sizeof(fftw_complex));
  out2 = (fftw_complex*) fftw_malloc(10 * 10 * sizeof(fftw_complex));
  fftw_plan forward = fftw_plan_dft_2d(10, 10, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_plan backward = fftw_plan_dft_2d(10, 10, out, out2, FFTW_BACKWARD, FFTW_ESTIMATE);
  for (int i=0; i < 100; i++) {
    (in[i])[0] = rand() % 255;
    printf("%lf;%lf\n", (in[i])[0], (in[i])[1]);
  }
  
  fftw_execute(forward);
  fftw_destroy_plan(forward);

  for (int i=0; i < 100; i++)
    printf("%lf;%lf\n", (out[i])[0], (out[i])[1]);

  fftw_execute(backward);
  fftw_destroy_plan(backward);
  
  for (int i=0; i < 100; i++) {
    (out2[i])[0] /= 100;
    (out2[i])[1] /= 100;
    printf("%lf;%lf\n", (out2[i])[0], (out2[i])[1]);
  }
  
  fftw_free(in);
  fftw_free(out);
  fftw_free(out2);
}
