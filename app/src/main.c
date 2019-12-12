#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

// === hardware includes

#include "lomo.h"
#include "qj3003p.h"
#include "appa208.h"

// === config
// TODO: make a config file

#define LOMO_TTY "/dev/ttyS2"
#define QJ3003P_TTY "/dev/ttyS7"
#define APPA208_TTY "/dev/ttyS3"
#define VOLTAGE_STEP 0.1
#define ADC_PASS 1
#define ADC_AVERAGE 2

// === threads

void *commander(void *);
void *worker(void *);

// === utils

int get_run();
void set_run(int run_new);
double get_time();

// === global variables

char dir_str[100];

pthread_rwlock_t run_lock;
int run;
char filename_vac[100];
char filename_adc[100];
time_t start_time;
struct tm start_time_struct;

// === program entry point

int main(int argc, char const *argv[])
{
	int ret = 0;

	pthread_t t_commander;
	pthread_t t_worker;

	int status;

	// === check input parameters

	if (argc < 2)
	{
		fprintf(stderr, "# E: Usage: vac <experiment_name>\n");
		ret = -1;
		goto main_exit;
	}

	// === get start time of experiment

	start_time = time(NULL);
	localtime_r(&start_time, &start_time_struct);

	// === we need actual information w/o buffering

	setlinebuf(stdout);
	setlinebuf(stderr);

	// === initialize run state variable

	pthread_rwlock_init(&run_lock, NULL);
	run = 1;

	// === create dirictory in "20191012_153504_<experiment_name>" format

	snprintf(dir_str, 100, "%04d%02d%02d_%02d%02d%02d_%s",
		start_time_struct.tm_year + 1900,
		start_time_struct.tm_mon + 1,
		start_time_struct.tm_mday,
		start_time_struct.tm_hour,
		start_time_struct.tm_min,
		start_time_struct.tm_sec,
		argv[1]
	);
	status = mkdir(dir_str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (status == -1)
	{
		fprintf(stderr, "# E: unable to create experiment directory (%s)\n", strerror(errno));
		ret = -2;
		goto main_exit;
	}

	// === create file names

	snprintf(filename_vac, 100, "%s/vac.dat", dir_str);
	snprintf(filename_adc, 100, "%s/adc.dat", dir_str);

	// printf("filename_vac \"%s\"\n", filename_vac);
	// printf("filename_adc = \"%s\"\n", filename_adc);

	// === now start threads

	pthread_create(&t_commander, NULL, commander, NULL);
	pthread_create(&t_worker, NULL, worker, NULL);

	// === and wait ...

	pthread_join(t_worker, NULL);

	// === cancel commander thread becouse we don't need it anymore
	// === and wait for cancelation finish

	pthread_cancel(t_commander);
	pthread_join(t_commander, NULL);

	main_exit:
	return ret;
}

// === commander function

void *commander(void *arg)
{
	(void) arg;

	char str[100];
	char *s;
	int ccount;

	while(get_run())
	{
		printf("> ");

		s = fgets(str, 100, stdin);
		if (s == NULL)
		{
			fprintf(stderr, "# E: Exit\n");
			set_run(0);
			break;
		}

		switch(str[0])
		{
			case 'h':
				printf(
					"Help:\n"
					"\th -- this help;\n"
					"\tq -- exit the program;\n");
				break;
			case 'q':
				set_run(0);
				break;
			default:
				ccount = strlen(str)-1;
				fprintf(stderr, "# E: Unknown commang (%.*s)\n", ccount, str);
				break;
		}
	}

	return NULL;
}

#define APPA

// === worker function

void *worker(void *arg)
{
	(void) arg;

	int r;

	int    pps_fd;
	int    pps_index;
	double pps_time;
	double pps_voltage;
	double pps_current;

	int    adc_fd;
	int    adc_index;
	double adc_time;
	double adc_value;
	double adc_average;

#ifdef APPA
	int appa_fd;
	appa208_disp_t disp;
	double mult_value;
	unit_t mult_unit;
	int mult_overload;
#endif

	FILE  *vac_fp;
	FILE  *adc_fp;

	FILE  *gp;

	char   buf[100];

	// === first we connect to instruments

	r = qj3003p_open(QJ3003P_TTY, &pps_fd);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to open pps (%d)\n", r);
		goto worker_exit;
	}

	r = lomo_open(LOMO_TTY, &adc_fd);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to open adc (%d)\n", r);
		set_run(0);
		goto worker_pps_close;
	}

#ifdef APPA
	r = appa208_open(APPA208_TTY, &appa_fd);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to open appa (%d)\n", r);
		set_run(0);
		goto worker_adc_close;
	}
#endif

	// === init pps

	r = qj3003p_set_output(pps_fd, 0);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to set pps output (%d)\n", r);
		goto worker_adc_close;
	}

	qj3003p_delay();

	r = qj3003p_set_voltage(pps_fd, 0.0);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to set pps voltage (%d)\n", r);
		goto worker_adc_close;
	}

	qj3003p_delay();

	r = qj3003p_set_current(pps_fd, 0.5);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to set pps current (%d)\n", r);
		goto worker_adc_close;
	}

	qj3003p_delay();

	r = qj3003p_set_output(pps_fd, 1);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to set pps output (%d)\n", r);
		goto worker_adc_close;
	}

	qj3003p_delay();

	// === init adc

	r = lomo_init(adc_fd);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to init adc (%d)\n", r);
		goto worker_pps_deinit;
	}

	// === init appa

#ifdef APPA
	r = appa208_read_disp(appa_fd, &disp);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to read appa display (%d)\n", r);
		goto worker_appa_close;
	}
	mult_unit = appa208_get_unit(&disp.mdata);
#endif

	// === create vac file

	vac_fp = fopen(filename_vac, "w+");
	if(vac_fp == NULL)
	{
		fprintf(stderr, "# E: Unable to open file \"%s\" (%s)\n", filename_vac, strerror(ferror(vac_fp)));
		goto worker_pps_deinit;
	}

	setlinebuf(vac_fp);

	// === create adc file

	adc_fp = fopen(filename_adc, "w+");
	if(adc_fp == NULL)
	{
		fprintf(stderr, "# E: Unable to open file \"%s\" (%s)\n", filename_adc, strerror(ferror(adc_fp)));
		goto worker_vac_fp_close;
	}

	setlinebuf(adc_fp);

	// === write vac header

	r = fprintf(vac_fp,
		"# 1: index\n"
		"# 2: time, s\n"
		"# 3: pps voltage, V\n"
		"# 4: pps current, A\n"
		"# 5: adc average value, a.u. [0-1]\n");
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to print to file \"%s\" (%s)\n", filename_vac, strerror(r));
		goto worker_adc_fp_close;
	}

	// === write adc header

		r = fprintf(adc_fp,
		"# 1: index\n"
		"# 2: time, s\n"
		"# 3: adc value, a.u. [0-1]\n");
#ifdef APPA
		r = fprintf(adc_fp,
		"# 4: appa value, %s\n"
		"# 5: appa overload, bool\n", appa208_str_unit(mult_unit));
#endif

	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to print to file \"%s\" (%s)\n", filename_adc, strerror(r));
		goto worker_adc_fp_close;
	}

	// === open gnuplot

	snprintf(buf, 100, "gnuplot > %s/gnuplot.log 2>&1", dir_str);
	gp = popen(buf, "w");
	if (gp == NULL)
	{
		fprintf(stderr, "# E: unable to open gnuplot pipe (%s)\n", strerror(errno));
		goto worker_adc_fp_close;
	}

	setlinebuf(gp);

	// === prepare gnuplot

	r = fprintf(gp,
		"set xrange [0:]\n"
		"set yrange [0:]\n"
		"set size 1,1\n"
		"set origin 0,0\n"
		"set ylabel \"ADC average signal, a.u. [0-1]\"\n"
	);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to print to gp (%s)\n", strerror(r));
		goto worker_gp_close;
	}

	// === let the action begins!

	adc_index = 0;
	pps_index = 0;

	while(get_run())
	{
		pps_time = get_time();
		if (pps_time < 0)
		{
			fprintf(stderr, "# E: Unable to get time\n");
			set_run(0);
			continue;
		}

		if ((pps_index + 1) * VOLTAGE_STEP > 10)
		{
			set_run(0);
			continue;
		}

		r = qj3003p_set_voltage(pps_fd, pps_index * VOLTAGE_STEP);
		if(r < 0)
		{
			fprintf(stderr, "# E: Unable to set qj voltage (%d)\n", r);
			set_run(0);
			continue;
		}

		adc_average = 0;

		for (int i = 0; i < ADC_PASS + ADC_AVERAGE; i++)
		{
			adc_time = get_time();
			if (adc_time < 0)
			{
				fprintf(stderr, "# E: Unable to get time\n");
				set_run(0);
				goto worker_while_continue;
			}

			r = lomo_read_value(adc_fd, &adc_value);
			if(r < 0)
			{
				fprintf(stderr, "# E: Unable to read adc (%d)\n", r);
				set_run(0);
				goto worker_while_continue;
			}

			r = appa208_read_disp(adc_fd, &disp);
			if(r < 0)
			{
				fprintf(stderr, "# E: Unable to read appa display (%d)\n", r);
				set_run(0);
				goto worker_while_continue;
			}
			mult_value = appa208_get_value(&disp.mdata);
			mult_unit = appa208_get_unit(&disp.mdata);
			mult_overload = appa208_get_overload(&disp.mdata);

			r = fprintf(adc_fp, "%d\t%le\t%le"
#ifdef APPA
				"\t%le\t%d"
#endif
				"\n", adc_index, adc_time, adc_value
#ifdef APPA
				, mult_value, mult_overload
#endif
				);
			if(r < 0)
			{
				fprintf(stderr, "# E: Unable to print to file \"%s\" (%s)\n", filename_adc, strerror(r));
				set_run(0);
				goto worker_while_continue;
			}

			if (pps_index == 0)
			{
				r = fprintf(gp,
					"set title \"t = %lf s\"\n"
					"set xlabel \"ADC index\"\n"
					"plot \"%s\" u 1:3 w l lw 1 notitle\n"
					"unset title\n",
					adc_time,
					filename_adc
				);
			}
			else
			{
				r = fprintf(gp,
					"set multiplot title \"t = %lf s\" layout 2,1\n"
					"set xlabel \"ADC index\"\n"
					"plot \"%s\" u 1:3 w l lw 1 notitle\n"
					"set xlabel \"PPS voltage, V\"\n"
					"plot \"%s\" u 3:5 w l lw 1 notitle\n"
					"unset multiplot\n",
					adc_time,
					filename_adc,
					filename_vac
				);
			}
			if(r < 0)
			{
				fprintf(stderr, "# E: Unable to print to gp (%s)\n", strerror(r));
				set_run(0);
				goto worker_while_continue;
			}

			if (i >= ADC_PASS)
			{
				adc_average += adc_value;
			}

			adc_index++;
		}

		adc_average /= ADC_AVERAGE;

		r = qj3003p_get_voltage(pps_fd, &pps_voltage);
		if(r < 0)
		{
			fprintf(stderr,"# E: Unable to get pps voltage (%d)\n", r);
			set_run(0);
			continue;
		}

		r = qj3003p_get_current(pps_fd, &pps_current);
		if(r < 0)
		{
			fprintf(stderr,"# E: Unable to get pps current (%d)\n", r);
			set_run(0);
			continue;
		}

		r = fprintf(vac_fp, "%d\t%le\t%le\t%le\t%le\n", pps_index, pps_time, pps_voltage, pps_current, adc_average);
		if(r < 0)
		{
			fprintf(stderr, "# E: Unable to print to file \"%s\" (%s)\n", filename_vac, strerror(r));
			set_run(0);
			continue;
		}

		r = fprintf(gp,
			"set multiplot title \"t = %lf s\" layout 2,1\n"
			"set xlabel \"ADC index\"\n"
			"plot \"%s\" u 1:3 w l lw 1 notitle\n"
			"set xlabel \"PPS voltage, V\"\n"
			"plot \"%s\" u 3:5 w l lw 1 notitle\n"
			"unset multiplot\n",
			adc_time,
			filename_adc,
			filename_vac
		);
		if(r < 0)
		{
			fprintf(stderr, "# E: Unable to print to gp (%s)\n", strerror(r));
			set_run(0);
			continue;
		}

		pps_index++;

		worker_while_continue:
		continue;
	}

	r = qj3003p_set_voltage(pps_fd, 0.0);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to set pps voltage (%d)\n", r);
	}

	qj3003p_delay();

	worker_gp_close:

	r = pclose(gp);
	if (r == -1)
	{
		fprintf(stderr, "# E: Unable to close gnuplot pipe (%s)\n", strerror(errno));
	}

	worker_adc_fp_close:

	r = fclose(adc_fp);
	if (r == EOF)
	{
		fprintf(stderr, "# E: Unable to close file \"%s\" (%s)\n", filename_adc, strerror(errno));
	}

	worker_vac_fp_close:

	r = fclose(vac_fp);
	if (r == EOF)
	{
		fprintf(stderr, "# E: Unable to close file \"%s\" (%s)\n", filename_vac, strerror(errno));
	}

		worker_appa_close:

#ifdef APPA
	r = appa208_close(appa_fd);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to close appa (%d)\n", r);
	}
#endif

	worker_adc_close:

	r = lomo_close(adc_fd);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to close lomo (%d)\n", r);
	}

	worker_pps_deinit:

	r = qj3003p_set_voltage(pps_fd, 0);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to set pps voltage (%d)\n", r);
	}

	qj3003p_delay();

	r = qj3003p_set_output(pps_fd, 0);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to set pps output (%d)\n", r);
	}

	qj3003p_delay();

	worker_pps_close:

	r = qj3003p_close(pps_fd);
	if(r < 0)
	{
		fprintf(stderr, "# E: Unable to close adc (%d)\n", r);
	}

	worker_exit:

	return NULL;
}

// === utils

int get_run()
{
	int run_local;
	pthread_rwlock_rdlock(&run_lock);
		run_local = run;
	pthread_rwlock_unlock(&run_lock);
	return run_local;
}

void set_run(int run_new)
{
	pthread_rwlock_wrlock(&run_lock);
		run = run_new;
	pthread_rwlock_unlock(&run_lock);
}

double get_time()
{
	static int first = 1;
	static struct timeval t_first = {0};
	struct timeval t = {0};
	double ret;
	int r;

	if (first == 1)
	{
		r = gettimeofday(&t_first, NULL);
		if (r == -1)
		{
			fprintf(stderr, "# E: unable to get time (%s)\n", strerror(errno));
			ret = -1;
		}
		else
		{
			ret = 0.0;
			first = 0;
		}
	}
	else
	{
		r = gettimeofday(&t, NULL);
		if (r == -1)
		{
			fprintf(stderr, "# E: unable to get time (%s)\n", strerror(errno));
			ret = -2;
		}
		else
		{
			ret = (t.tv_sec - t_first.tv_sec) * 1e6 + (t.tv_usec - t_first.tv_usec);
			ret /= 1e6;
		}
	}

	return ret;
}