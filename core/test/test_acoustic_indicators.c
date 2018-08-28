/*
* BSD 3-Clause License
*
* Copyright (c) 2018, Ifsttar Wi6labs LS2N
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
*
*  Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
*  Neither the name of the copyright holder nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "acoustic_indicators.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "minunit.h"
#include "string.h"

int tests_run = 0;
//char *message = (char*)malloc(256 * sizeof(char));
char mu_message[256];

/**
 * Read raw audio signal file
 */
static char * test_leq_32khz() {
  // Compute the reference level.
  //double RMS_REFERENCE_90DB = 2500;
  //double DB_FS_REFERENCE = - (20 * log10(RMS_REFERENCE_90DB)) + 90;
  //double REF_SOUND_PRESSURE = 1 / pow(10, DB_FS_REFERENCE / 20);

	float_t REF_SOUND_PRESSURE = 32767.;

	const char *filename = "speak_32000Hz_16bitsPCM_10s.raw";
	FILE *ptr;
	AcousticIndicatorsData acousticIndicatorsData;
    ai_InitAcousticIndicatorsData(&acousticIndicatorsData, false, false,REF_SOUND_PRESSURE, false);

    int16_t shortBuffer[AI_WINDOW_SIZE];

	// open file
	ptr = fopen(filename, "rb");
	if (ptr == NULL) {
		printf("Error opening audio file\n");
		exit(1);
	}

  int total_read = 0;
	int read = 0;

  float leqs[10];
  float expected_leqs[10] = {-26.21, -27.94, -29.12, -28.92, -40.4, -24.93, -31.55, -29.04, -31.08, -30.65};

  int leqId = 0;

	while(!feof(ptr)) {
		read = fread(shortBuffer, sizeof(int16_t), sizeof(shortBuffer) / sizeof(int16_t), ptr);
    total_read+=read;
		// File fragment is in read array
		// Process short sample
		int sampleCursor = 0;
		do {
			int maxLen = ai_GetMaximalSampleSize(&acousticIndicatorsData);
            int sampleLen = (read - sampleCursor) < maxLen ? (read - sampleCursor) : maxLen;
            if(ai_AddSample(&acousticIndicatorsData, sampleLen, shortBuffer + sampleCursor) == AI_FEED_COMPLETE) {
                mu_assert("Too much iteration, more than 10s in file or wrong sampling rate", leqId < 10);
                leqs[leqId++] = acousticIndicatorsData.last_leq_slow;
			}
			sampleCursor+=sampleLen;
		} while(sampleCursor < read);
	}
  mu_assert("Wrong number of parsed samples", total_read == 320000);
  ai_FreeAcousticIndicatorsData(&acousticIndicatorsData);
  // Check expected leq

  for(int second = 0; second < 10; second++) {
    sprintf(mu_message, "Wrong leq on %d second expected %f dB got %f dB", second, expected_leqs[second], leqs[second]);
    mu_assert(mu_message, fabs(expected_leqs[second] - leqs[second]) < 0.01);
  }
  return 0;
}


/**
 * Read raw audio signal file
 */
static char * test_laeq_32khz() {
  // Compute the reference level.
  //double RMS_REFERENCE_90DB = 2500;
  //double DB_FS_REFERENCE = - (20 * log10(RMS_REFERENCE_90DB)) + 90;
  //double REF_SOUND_PRESSURE = 1 / pow(10, DB_FS_REFERENCE / 20);

	float_t REF_SOUND_PRESSURE = 32767.;

	const char *filename = "speak_32000Hz_16bitsPCM_10s.raw";
	FILE *ptr;
	AcousticIndicatorsData acousticIndicatorsData;
    ai_InitAcousticIndicatorsData(&acousticIndicatorsData, true, false,REF_SOUND_PRESSURE, false);

    int16_t shortBuffer[AI_WINDOW_SIZE];

	// open file
	ptr = fopen(filename, "rb");
	if (ptr == NULL) {
		printf("Error opening audio file\n");
		exit(1);
	}

  int total_read = 0;
	int read = 0;

  float leqs[10];
  float expected_laeqs[10] = {-31.37, -33.74, -33.05, -33.61, -43.68, -29.96, -35.53, -34.12, -37.06, -37.19};

  int leqId = 0;

	while(!feof(ptr)) {
		read = fread(shortBuffer, sizeof(int16_t), sizeof(shortBuffer) / sizeof(int16_t), ptr);
    total_read+=read;
		// File fragment is in read array
		// Process short sample
		int sampleCursor = 0;
		do {
			int maxLen = ai_GetMaximalSampleSize(&acousticIndicatorsData);
            int sampleLen = (read - sampleCursor) < maxLen ? (read - sampleCursor) : maxLen;
            if(ai_AddSample(&acousticIndicatorsData, sampleLen, shortBuffer + sampleCursor) == AI_FEED_COMPLETE) {
                mu_assert("Too much iteration, more than 10s in file or wrong sampling rate", leqId < 10);
                leqs[leqId++] = ai_get_leq_slow(&acousticIndicatorsData);
			}
			sampleCursor+=sampleLen;
		} while(sampleCursor < read);
	}
  mu_assert("Wrong number of parsed samples", total_read == 320000);

  ai_FreeAcousticIndicatorsData(&acousticIndicatorsData);
  // Check expected leq

  for(int second = 0; second < 10; second++) {
    sprintf(mu_message, "Wrong lAeq on %d second expected %f dB got %f dB", second, expected_laeqs[second], leqs[second]);
    mu_assert(mu_message, fabs(expected_laeqs[second] - leqs[second]) < 0.1);
  }
  return 0;
}


/**
 * Read raw audio signal file
 */
static char * test_leq_spectrum_32khz() {
  // Compute the reference level.
  //double RMS_REFERENCE_90DB = 2500;
  //double DB_FS_REFERENCE = - (20 * log10(RMS_REFERENCE_90DB)) + 90;
  //double REF_SOUND_PRESSURE = 1 / pow(10, DB_FS_REFERENCE / 20);

    float_t REF_SOUND_PRESSURE = 32767.;

    const char *filename = "speak_32000Hz_16bitsPCM_10s.raw";
    FILE *ptr;
    AcousticIndicatorsData acousticIndicatorsData;
    ai_InitAcousticIndicatorsData(&acousticIndicatorsData, false, true,REF_SOUND_PRESSURE, false);

    int16_t shortBuffer[AI_WINDOW_SIZE];

    // open file
    ptr = fopen(filename, "rb");
    if (ptr == NULL) {
        printf("Error opening audio file\n");
        exit(1);
    }

  int total_read = 0;
    int read = 0;

  float leqs[AI_NB_BAND];
  memset(leqs, 0, sizeof(float) * AI_NB_BAND);
  float expected_leqs[AI_NB_BAND] = {-64.59,-62.82,-63.14,-64.93,-65.03,-66.43,-65.56,-66.  ,-68.06,-66.28,-43.34,
                             -31.93,-37.28,-47.33,-35.33,-42.68,-42.91,-48.51,-49.1 ,-52.9 ,-52.15,-52.8 ,
                             -52.35,-52.31,-53.39,-52.53,-53.73,-53.56,-57.9};

  int leqId = 0;
    int i;
    while(!feof(ptr)) {
        read = fread(shortBuffer, sizeof(int16_t), sizeof(shortBuffer) / sizeof(int16_t), ptr);
    total_read+=read;
        // File fragment is in read array
        // Process short sample
        int sampleCursor = 0;
        do {
            int maxLen = ai_GetMaximalSampleSize(&acousticIndicatorsData);
            int sampleLen = (read - sampleCursor) < maxLen ? (read - sampleCursor) : maxLen;
            if(ai_AddSample(&acousticIndicatorsData, sampleLen, shortBuffer + sampleCursor) == AI_FEED_COMPLETE) {
                mu_assert("Too much iteration, more than 10s in file or wrong sampling rate", leqId < 10);
                for(i = 0; i < AI_NB_BAND; i++) {
                    double db_1s = ai_get_band_leq(&acousticIndicatorsData, i);
                    leqs[i] += pow(10, db_1s / 10.);
                }
            }
            sampleCursor+=sampleLen;
        } while(sampleCursor < read);
    }
  mu_assert("Wrong number of parsed samples", total_read == 320000);
  ai_FreeAcousticIndicatorsData(&acousticIndicatorsData);
  // Check expected leq
  int idfreq;
  double sumval =  0;
  for(idfreq = 0; idfreq < AI_NB_BAND; idfreq++) {
    float leqdiff = 10 * log10(leqs[idfreq] / 10) - expected_leqs[idfreq];
    sumval+=leqdiff*leqdiff;
  }
  double expected_mean_error = 2.83;
  double mean_error = sqrt(sumval / AI_NB_BAND);
  sprintf(mu_message, "Wrong mean error expected %f got %f\n", expected_mean_error, mean_error);
  mu_assert(mu_message, mean_error < expected_mean_error);
  return 0;
}


/**
 * Test 1khz Rectangular FFT
 */
static char * test_1khz_rectangular_lobs() {
		double RMS_REFERENCE_94DB = 2500;
		double DB_FS_REFERENCE = - (20 * log10(RMS_REFERENCE_94DB)) + 94;
		double REF_SOUND_PRESSURE = 1 / pow(10, DB_FS_REFERENCE / 20);

		const int sampleRate = 32000;
		const int signal_samples = 32000;
		double powerRMS = RMS_REFERENCE_94DB;
		float signalFrequency = 1000;
		double powerPeak = powerRMS * sqrt(2);

	  float expected_leqs[AI_NB_BAND] = {-82.2,-80.1,-77.4,-74.8,-72.2,-69.7,
			-67.2,-64.9,-62.8,-61.4,-60.9,-62.8,-71.0,-59.5,-54.4,-58.6,-45.3,-22.3,
			-48.2,-57.0,-62.0,-67.4,-70.2,-73.8,-76.4,-79.2,-81.6,-83.6,-84.8};

		int16_t buffer[AI_WINDOW_SIZE];

		AcousticIndicatorsData acousticIndicatorsData;
		ai_InitAcousticIndicatorsData(&acousticIndicatorsData, false, true,REF_SOUND_PRESSURE, false);
		int s;
		int processed_bands = 0;
		for (s = 0; s < signal_samples;) {
			int start_s = s;
			int maxLen = ai_GetMaximalSampleSize(&acousticIndicatorsData);
			for(; s < signal_samples && s-start_s < maxLen;s++) {
				double t = s * (1 / (double)sampleRate);
	      double pwr = (sin(2 * AI_PI * signalFrequency * t) * (powerPeak));
				buffer[s-start_s] = (int16_t)pwr;
			}
			if(ai_AddSample(&acousticIndicatorsData, maxLen, buffer) == AI_FEED_COMPLETE) {
					// Average spectrum levels
					int iband;
					printf("Frequency");
					printf(",leq");
					for(iband=0;iband<AI_NB_BAND;iband++) {
						printf(",%.1f", ai_get_frequency(iband));
					}
					printf("\n");
					printf("Rectangular");
					printf(",%.1f", ai_get_leq_slow(&acousticIndicatorsData));
					for(iband=0;iband<AI_NB_BAND;iband++) {
						processed_bands++;
						float_t level = ai_get_band_leq(&acousticIndicatorsData, iband);
						printf(",%.1f", level);
					}
					printf("\n");
			}
		}
		mu_assert("Spectrum not obtained" ,processed_bands == AI_NB_BAND);
	  ai_FreeAcousticIndicatorsData(&acousticIndicatorsData);
		return 0;
}


/**
 * Test 1khz overlapped Hann FFT
 */
static char * test_1khz_hann_lobs() {
		double RMS_REFERENCE_94DB = 2500;
		double DB_FS_REFERENCE = - (20 * log10(RMS_REFERENCE_94DB)) + 94;
		double REF_SOUND_PRESSURE = 1 / pow(10, DB_FS_REFERENCE / 20);

		const int sampleRate = 32000;
		const int signal_samples = 32000;
		double powerRMS = RMS_REFERENCE_94DB;
		float signalFrequency = 1000;
		double powerPeak = powerRMS * sqrt(2);

	  float expected_leqs[AI_NB_BAND] = {-82.2,-80.1,-77.4,-74.8,-72.2,-69.7,
			-67.2,-64.9,-62.8,-61.4,-60.9,-62.8,-71.0,-59.5,-54.4,-58.6,-45.3,-22.3,
			-48.2,-57.0,-62.0,-67.4,-70.2,-73.8,-76.4,-79.2,-81.6,-83.6,-84.8};

		int16_t buffer[AI_WINDOW_SIZE];

		AcousticIndicatorsData acousticIndicatorsData;
		ai_InitAcousticIndicatorsData(&acousticIndicatorsData, false, true,REF_SOUND_PRESSURE, true);
		int s;
		int processed_bands = 0;
		for (s = 0; s < signal_samples;) {
			int start_s = s;
			int maxLen = ai_GetMaximalSampleSize(&acousticIndicatorsData);
			for(; s < signal_samples && s-start_s < maxLen;s++) {
				double t = s * (1 / (double)sampleRate);
	      double pwr = (sin(2 * AI_PI * signalFrequency * t) * (powerPeak));
				buffer[s-start_s] = (int16_t)pwr;
			}
			if(ai_AddSample(&acousticIndicatorsData, maxLen, buffer) == AI_FEED_COMPLETE) {
					// Average spectrum levels
					int iband;
					printf("Hann");
					printf(",%.1f", ai_get_leq_slow(&acousticIndicatorsData));
					int band_id;
					for(iband=0;iband<AI_NB_BAND;iband++) {
						processed_bands++;
						float_t level = ai_get_band_leq(&acousticIndicatorsData, iband);
						printf(",%.1f", level);
					}
					/*
					for(band_id=0;band_id<AI_NB_BAND;band_id++) {
						processed_bands++;
						float level;
		        int i;
		        double sum = 0;
		        int window_count = acousticIndicatorsData.windows_count == 0 ? AI_WINDOWS_SIZE : acousticIndicatorsData.windows_count;
		        for(i=2; i < window_count; i++) {
		          sum += acousticIndicatorsData.spectrum[i][band_id];
		        }
		        level = 20 * log10(sum / AI_WINDOWS_SIZE);
						printf(",%.1f", level);
					}
					*/
					printf("\n");
			}
		}
		mu_assert("Spectrum not obtained" ,processed_bands == AI_NB_BAND);
	  ai_FreeAcousticIndicatorsData(&acousticIndicatorsData);
		return 0;
}
static char * all_tests() {
   //mu_run_test(test_leq_32khz);
   //mu_run_test(test_laeq_32khz);
   //mu_run_test(test_leq_spectrum_32khz);
	 mu_run_test(test_1khz_rectangular_lobs);
	 mu_run_test(test_1khz_hann_lobs);

   return 0;
}

int main(int argc, char **argv) {
     char *result = all_tests();
     if (result != 0) {
         printf("%s\n", result);
     }
     else {
         printf("ALL TESTS PASSED\n");
     }
     printf("Tests run: %d\n", tests_run);

     return result != 0;
}
