/*******************************************************************************
PolSARpro v5.0 is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 (1991) of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. 

See the GNU General Public License (Version 2, 1991) for more details.

********************************************************************************

File   : rawbinary_convert_Cmplx_T4.c
Project  : ESA_POLSARPRO
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Version  : 1.0
Creation : 11/2004
Update  :

*-------------------------------------------------------------------------------
INSTITUT D'ELECTRONIQUE et de TELECOMMUNICATIONS de RENNES (I.E.T.R)
UMR CNRS 6164
Groupe Image et Teledetection
Equipe SAPHIR (SAr Polarimetrie Holographie Interferometrie Radargrammetrie)
UNIVERSITE DE RENNES I
P�le Micro-Ondes Radar
B�t. 11D - Campus de Beaulieu
263 Avenue G�n�ral Leclerc
35042 RENNES Cedex
Tel :(+33) 2 23 23 57 63
Fax :(+33) 2 23 23 69 63
e-mail : eric.pottier@univ-rennes1.fr, laurent.ferro-famil@univ-rennes1.fr
*-------------------------------------------------------------------------------

Description :  Convert Raw Binary Data Files (Format 4x4 coherency matrix)

Output Format = T3
Outputs : In T3 directory
config.txt
T11.bin, T12_real.bin, T12_imag.bin, T13_real.bin, T13_imag.bin
T22.bin, T23_real.bin, T23_imag.bin
T33.bin

Output Format = T4
Outputs : In T4 directory
config.txt
T11.bin, T12_real.bin, T12_imag.bin,
T13_real.bin, T13_imag.bin,
T14_real.bin, T14_imag.bin,
T22.bin, T23_real.bin, T23_imag.bin
T24_real.bin, T24_imag.bin,
T33.bin, T34_real.bin, T34_imag.bin
T44.bin

Output Format = C3
Outputs : In C3 directory
config.txt
C11.bin, C12_real.bin, C12_imag.bin, C13_real.bin, C13_imag.bin
C22.bin, C23_real.bin, C23_imag.bin
C33.bin

Output Format = C4
Outputs : In C4 directory
config.txt
C11.bin, C12_real.bin, C12_imag.bin,
C13_real.bin, C13_imag.bin,
C14_real.bin, C14_imag.bin,
C22.bin, C23_real.bin, C23_imag.bin
C24_real.bin, C24_imag.bin,
C33.bin, C34_real.bin, C34_imag.bin
C44.bin

*-------------------------------------------------------------------------------
Routines  :
void edit_error(char *s1,char *s2);
void check_dir(char *dir);
void check_file(char *file);
float **matrix_float(int nrh,int nch);
void free_matrix_float(float **m,int nrh);
void write_config(char *dir, int Nlig, int Ncol, char *PolarCase, char *PolarType);

*******************************************************************************/
/* C INCLUDES */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "omp.h"

#ifdef _WIN32
#include <dos.h>
#include <conio.h>
#endif

/* T4 matrix */
#define T11   0
#define T12   1
#define T13   2
#define T14   3
#define T22   4
#define T23   5
#define T24   6
#define T33   7
#define T34   8
#define T44   9

/* T3 matrix */
#define T311   0
#define T312_re  1
#define T312_im  2
#define T313_re  3
#define T313_im  4
#define T322   5
#define T323_re  6
#define T323_im  7
#define T333   8
/* T4 matrix */
#define T411   0
#define T412_re  1
#define T412_im  2
#define T413_re  3
#define T413_im  4
#define T414_re  5
#define T414_im  6
#define T422   7
#define T423_re  8
#define T423_im  9
#define T424_re  10
#define T424_im  11
#define T433   12
#define T434_re  13
#define T434_im  14
#define T444   15
/* C3 matrix */
#define C311   0
#define C312_re  1
#define C312_im  2
#define C313_re  3
#define C313_im  4
#define C322   5
#define C323_re  6
#define C323_im  7
#define C333   8
/* C4 matrix */
#define C411   0
#define C412_re  1
#define C412_im  2
#define C413_re  3
#define C413_im  4
#define C414_re  5
#define C414_im  6
#define C422   7
#define C423_re  8
#define C423_im  9
#define C424_re  10
#define C424_im  11
#define C433   12
#define C434_re  13
#define C434_im  14
#define C444   15

/* ROUTINES DECLARATION */
#include "../lib/matrix.h"
#include "../lib/util.h"

/* CHARACTER STRINGS */
char CS_Texterreur[80];

/* GLOBAL ARRAYS */
float **M_tmp;
float **M_in;
float **M_out;

/*******************************************************************************
Routine  : main
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Creation : 01/2002
Update  :
*-------------------------------------------------------------------------------

Description :  Convert Raw Binary Data Files (Format 4x4 coherency matrix)

*-------------------------------------------------------------------------------
Inputs arguments :
argc : nb of input arguments
argv : input arguments array
Returned values  :
void
*******************************************************************************/

int main(int argc, char *argv[])
/*                                      */
{

/* LOCAL VARIABLES */

  FILE *in_file[16], *out_file[16];

  char File1[FilePathLength],File2[FilePathLength],File3[FilePathLength],File4[FilePathLength],File5[FilePathLength],File6[FilePathLength];
  char File7[FilePathLength],File8[FilePathLength],File9[FilePathLength],File10[FilePathLength];
  char *FileOutputT3[9] = { "T11.bin", "T12_real.bin", "T12_imag.bin",
                "T13_real.bin", "T13_imag.bin", "T22.bin",
                "T23_real.bin", "T23_imag.bin", "T33.bin"};
  char *FileOutputT4[16]= { "T11.bin", "T12_real.bin", "T12_imag.bin",
                  "T13_real.bin", "T13_imag.bin", "T14_real.bin",
                "T14_imag.bin", "T22.bin", "T23_real.bin",
                "T23_imag.bin", "T24_real.bin", "T24_imag.bin",
                "T33.bin", "T34_real.bin", "T34_imag.bin", "T44.bin"};
  char *FileOutputC3[9] = { "C11.bin", "C12_real.bin", "C12_imag.bin",
                "C13_real.bin", "C13_imag.bin", "C22.bin",
                "C23_real.bin", "C23_imag.bin", "C33.bin"};
  char *FileOutputC4[16]= { "C11.bin", "C12_real.bin", "C12_imag.bin",
                  "C13_real.bin", "C13_imag.bin", "C14_real.bin",
                "C14_imag.bin", "C22.bin", "C23_real.bin",
                "C23_imag.bin", "C24_real.bin", "C24_imag.bin",
                "C33.bin", "C34_real.bin", "C34_imag.bin", "C44.bin"};
  char DirOutput[FilePathLength],file_name[FilePathLength],DataFormat[10];
  char PolarCase[20], PolarType[20];

  int lig, col,l, np, ind;
  int Ncol;
  int Nligoffset, Ncoloffset;
  int Nligfin, Ncolfin;
  int SubSampRG, SubSampAZ;
//int Symmetrisation;
  int IEEE, Npolar_in, Npolar_out;

  char *pc;
  float fl1, fl2;
  float *v;

/******************************************************************************/
/* INPUT PARAMETERS */
/******************************************************************************/

  if (argc < 22) {
  printf("TYPE: rawbinary_convert_Cmplx_T4 DirOutput Ncol OffsetLig OffsetCol\n");
  printf("FinalNlig FinalNcol IEEEFormat_Convert (0/1) Symmetrisation OutputDataFormat\n");
  printf("SubSampRG SubSampAZ\n");
  printf("FileInput1 FileInput2 FileInput3 FileInput4 FileInput5 FileInput6\n");
  printf("FileInput8 FileInput9 FileInput10\n");
  exit(1);
  } else {
  strcpy(DirOutput, argv[1]);
  Ncol = atoi(argv[2]);
  Nligoffset = atoi(argv[3]);
  Ncoloffset = atoi(argv[4]);
  Nligfin = atoi(argv[5]);
  Ncolfin = atoi(argv[6]);
  IEEE = atoi(argv[7]);
//  Symmetrisation = atoi(argv[8]);
  strcpy(DataFormat, argv[9]);
   SubSampRG = atoi(argv[10]);
  SubSampAZ = atoi(argv[11]);
  strcpy(File1, argv[12]);
  strcpy(File2, argv[13]);
  strcpy(File3, argv[14]);
  strcpy(File4, argv[15]);
  strcpy(File5, argv[16]);
  strcpy(File6, argv[17]);
  strcpy(File7, argv[18]);
  strcpy(File8, argv[19]);
  strcpy(File9, argv[20]);
  strcpy(File10, argv[21]);
  }

  check_file(File1);check_file(File2);
  check_file(File3);check_file(File4);
  check_file(File5);check_file(File6);
  check_file(File7);check_file(File8);
  check_file(File9);check_file(File10);
  check_dir(DirOutput);

  PSP_Threads = omp_get_max_threads();
  if (PSP_Threads <= 2) {
    PSP_Threads = 1;
    } else {
	PSP_Threads = PSP_Threads - 1;
	}
  omp_set_num_threads(PSP_Threads);

/* Nb of lines and rows sub-sampled image */
  Nligfin = (int) floor(Nligfin / SubSampAZ);
  Ncolfin = (int) floor(Ncolfin / SubSampRG);
  if (strcmp(DataFormat, "T3") == 0) strcpy(PolarCase, "monostatic");
  if (strcmp(DataFormat, "T4") == 0) strcpy(PolarCase, "bistatic");
  if (strcmp(DataFormat, "C3") == 0) strcpy(PolarCase, "monostatic");
  if (strcmp(DataFormat, "C4") == 0) strcpy(PolarCase, "bistatic");
  strcpy(PolarType, "full");
  write_config(DirOutput, Nligfin, Ncolfin, PolarCase, PolarType);

  Npolar_in = 10;
  if (strcmp(DataFormat, "T3") == 0) Npolar_out = 9;
  if (strcmp(DataFormat, "T4") == 0) Npolar_out = 16;
  if (strcmp(DataFormat, "C3") == 0) Npolar_out = 9;
  if (strcmp(DataFormat, "C4") == 0) Npolar_out = 16;

  M_tmp = matrix_float(Npolar_in, 2 * Ncol);
  M_in = matrix_float(16, Ncol);
  if (strcmp(DataFormat, "T3") == 0) M_out = matrix_float(Npolar_out, Ncolfin);
  if (strcmp(DataFormat, "T4") == 0) M_out = matrix_float(Npolar_out, Ncolfin);
  if (strcmp(DataFormat, "C3") == 0) M_out = matrix_float(Npolar_out, Ncolfin);
  if (strcmp(DataFormat, "C4") == 0) M_out = matrix_float(Npolar_out, Ncolfin);

/******************************************************************************/
/* INPUT / OUTPUT BINARY DATA FILES */
/******************************************************************************/

  if ((in_file[0] = fopen(File1, "rb")) == NULL)
    edit_error("Could not open input file : ", File1);
  if ((in_file[1] = fopen(File2, "rb")) == NULL)
    edit_error("Could not open input file : ", File2);
  if ((in_file[2] = fopen(File3, "rb")) == NULL)
    edit_error("Could not open input file : ", File3);
  if ((in_file[3] = fopen(File4, "rb")) == NULL)
    edit_error("Could not open input file : ", File4);
  if ((in_file[4] = fopen(File5, "rb")) == NULL)
    edit_error("Could not open input file : ", File5);
  if ((in_file[5] = fopen(File6, "rb")) == NULL)
    edit_error("Could not open input file : ", File6);
  if ((in_file[6] = fopen(File7, "rb")) == NULL)
    edit_error("Could not open input file : ", File7);
  if ((in_file[7] = fopen(File8, "rb")) == NULL)
    edit_error("Could not open input file : ", File8);
  if ((in_file[8] = fopen(File9, "rb")) == NULL)
    edit_error("Could not open input file : ", File9);
  if ((in_file[9] = fopen(File10, "rb")) == NULL)
    edit_error("Could not open input file : ", File10);

  for (np = 0; np < Npolar_out; np++) {
  if (strcmp(DataFormat, "T3") == 0) sprintf(file_name, "%s%s", DirOutput, FileOutputT3[np]);
  if (strcmp(DataFormat, "T4") == 0) sprintf(file_name, "%s%s", DirOutput, FileOutputT4[np]);
  if (strcmp(DataFormat, "C3") == 0) sprintf(file_name, "%s%s", DirOutput, FileOutputC3[np]);
  if (strcmp(DataFormat, "C4") == 0) sprintf(file_name, "%s%s", DirOutput, FileOutputC4[np]);
  if ((out_file[np] = fopen(file_name, "wb")) == NULL)
    edit_error("Could not open output file : ", file_name);
  }

/******************************************************************************/
for (np = 0; np < Npolar_in; np++)
   rewind(in_file[np]);
 
for (lig = 0; lig < Nligoffset; lig++) {
  fread(&M_tmp[0][0], sizeof(float), Ncol, in_file[T11]);
  fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T12]);
  fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T13]);
  fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T14]);
  fread(&M_tmp[0][0], sizeof(float), Ncol, in_file[T22]);
  fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T23]);
  fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T24]);
  fread(&M_tmp[0][0], sizeof(float), Ncol, in_file[T33]);
  fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T34]);
  fread(&M_tmp[0][0], sizeof(float), Ncol, in_file[T44]);
  }

for (lig = 0; lig < Nligfin; lig++) {
  if (lig%(int)(Nligfin/20) == 0) {printf("%f\r", 100. * lig / (Nligfin - 1));fflush(stdout);}
     if (IEEE == 0) {
      fread(&M_tmp[T11][0], sizeof(float), Ncol, in_file[T11]);
      fread(&M_tmp[T12][0], sizeof(float), 2 * Ncol, in_file[T12]);
      fread(&M_tmp[T13][0], sizeof(float), 2 * Ncol, in_file[T13]);
      fread(&M_tmp[T13][0], sizeof(float), 2 * Ncol, in_file[T14]);
      fread(&M_tmp[T22][0], sizeof(float), Ncol, in_file[T22]);
      fread(&M_tmp[T23][0], sizeof(float), 2 * Ncol, in_file[T23]);
      fread(&M_tmp[T23][0], sizeof(float), 2 * Ncol, in_file[T24]);
      fread(&M_tmp[T33][0], sizeof(float), Ncol, in_file[T33]);
      fread(&M_tmp[T23][0], sizeof(float), 2 * Ncol, in_file[T34]);
      fread(&M_tmp[T33][0], sizeof(float), Ncol, in_file[T44]);
      }
    if (IEEE == 1) {
      for (col = 0; col < Ncol; col++) {
        v = &fl1;pc = (char *) v;
        pc[3] = getc(in_file[T11]);pc[2] = getc(in_file[T11]);
        pc[1] = getc(in_file[T11]);pc[0] = getc(in_file[T11]);
        M_tmp[T11][2 * col] = fl1;M_tmp[T11][2 * col +1] = 0.;
        v = &fl1;pc = (char *) v;
        pc[3] = getc(in_file[T12]);pc[2] = getc(in_file[T12]);
        pc[1] = getc(in_file[T12]);pc[0] = getc(in_file[T12]);
        v = &fl2;pc = (char *) v;
        pc[3] = getc(in_file[T12]);pc[2] = getc(in_file[T12]);
        pc[1] = getc(in_file[T12]);pc[0] = getc(in_file[T12]);
        M_tmp[T12][2 * col] = fl1;M_tmp[T12][2 * col + 1] = fl2;
        v = &fl1;pc = (char *) v;
        pc[3] = getc(in_file[T13]);pc[2] = getc(in_file[T13]);
        pc[1] = getc(in_file[T13]);pc[0] = getc(in_file[T13]);
        v = &fl2;pc = (char *) v;
        pc[3] = getc(in_file[T13]);pc[2] = getc(in_file[T13]);
        pc[1] = getc(in_file[T13]);pc[0] = getc(in_file[T13]);
        M_tmp[T13][2 * col] = fl1;M_tmp[T13][2 * col + 1] = fl2;
        v = &fl1;pc = (char *) v;
        pc[3] = getc(in_file[T14]);pc[2] = getc(in_file[T14]);
        pc[1] = getc(in_file[T14]);pc[0] = getc(in_file[T14]);
        v = &fl2;pc = (char *) v;
        pc[3] = getc(in_file[T14]);pc[2] = getc(in_file[T14]);
        pc[1] = getc(in_file[T14]);pc[0] = getc(in_file[T14]);
        M_tmp[T14][2 * col] = fl1;M_tmp[T14][2 * col + 1] = fl2;
        v = &fl1;pc = (char *) v;
        pc[3] = getc(in_file[T22]);pc[2] = getc(in_file[T22]);
        pc[1] = getc(in_file[T22]);pc[0] = getc(in_file[T22]);
        M_tmp[T22][2 * col] = fl1;M_tmp[T22][2 * col +1] = 0.;
        v = &fl1;pc = (char *) v;
        pc[3] = getc(in_file[T23]);pc[2] = getc(in_file[T23]);
        pc[1] = getc(in_file[T23]);pc[0] = getc(in_file[T23]);
        v = &fl2;pc = (char *) v;
        pc[3] = getc(in_file[T23]);pc[2] = getc(in_file[T23]);
        pc[1] = getc(in_file[T23]);pc[0] = getc(in_file[T23]);
        M_tmp[T23][2 * col] = fl1;M_tmp[T23][2 * col + 1] = fl2;
        v = &fl1;pc = (char *) v;
        pc[3] = getc(in_file[T24]);pc[2] = getc(in_file[T24]);
        pc[1] = getc(in_file[T24]);pc[0] = getc(in_file[T24]);
        v = &fl2;pc = (char *) v;
        pc[3] = getc(in_file[T24]);pc[2] = getc(in_file[T24]);
        pc[1] = getc(in_file[T24]);pc[0] = getc(in_file[T24]);
        M_tmp[T24][2 * col] = fl1;M_tmp[T24][2 * col + 1] = fl2;
        v = &fl1;pc = (char *) v;
        pc[3] = getc(in_file[T33]);pc[2] = getc(in_file[T33]);
        pc[1] = getc(in_file[T33]);pc[0] = getc(in_file[T33]);
        M_tmp[T33][2 * col] = fl1;M_tmp[T33][2 * col +1] = 0.;
        v = &fl1;pc = (char *) v;
        pc[3] = getc(in_file[T34]);pc[2] = getc(in_file[T34]);
        pc[1] = getc(in_file[T34]);pc[0] = getc(in_file[T34]);
        v = &fl2;pc = (char *) v;
        pc[3] = getc(in_file[T34]);pc[2] = getc(in_file[T34]);
        pc[1] = getc(in_file[T34]);pc[0] = getc(in_file[T34]);
        M_tmp[T34][2 * col] = fl1;M_tmp[T34][2 * col + 1] = fl2;
        v = &fl1;pc = (char *) v;
        pc[3] = getc(in_file[T44]);pc[2] = getc(in_file[T44]);
        pc[1] = getc(in_file[T44]);pc[0] = getc(in_file[T44]);
        M_tmp[T44][2 * col] = fl1;M_tmp[T44][2 * col +1] = 0.;
        }
    }
   for (col = 0; col < Ncol; col++) {
     M_in[T411][col] = M_tmp[T11][2 * col];
     M_in[T412_re][col] = M_tmp[T12][2 * col];
     M_in[T412_im][col] = M_tmp[T12][2 * col + 1];
     M_in[T413_re][col] = M_tmp[T13][2 * col];
     M_in[T413_im][col] = M_tmp[T13][2 * col + 1];
     M_in[T414_re][col] = M_tmp[T14][2 * col];
     M_in[T414_im][col] = M_tmp[T14][2 * col + 1];
     M_in[T422][col] = M_tmp[T22][2 * col];
     M_in[T423_re][col] = M_tmp[T23][2 * col];
     M_in[T423_im][col] = M_tmp[T23][2 * col + 1];
     M_in[T424_re][col] = M_tmp[T24][2 * col];
     M_in[T424_im][col] = M_tmp[T24][2 * col + 1];
     M_in[T433][col] = M_tmp[T33][2 * col];
     M_in[T434_re][col] = M_tmp[T34][2 * col];
     M_in[T434_im][col] = M_tmp[T34][2 * col + 1];
     M_in[T444][col] = M_tmp[T44][2 * col];
     for (np = 0; np < 16; np++) if (my_isfinite(M_in[np][col]) == 0) M_in[np][col] = eps;
     }

  if (strcmp(DataFormat, "T3") == 0) {
  for (col = 0; col < Ncolfin; col++) {
    ind = col * SubSampRG + Ncoloffset;
    M_out[T311][col] = M_in[T411][ind];
    M_out[T312_re][col] = M_in[T412_re][ind];
    M_out[T312_im][col] = M_in[T412_im][ind];
    M_out[T313_re][col] = M_in[T413_re][ind];
    M_out[T313_im][col] = M_in[T413_im][ind];
    M_out[T322][col] = M_in[T422][ind];
    M_out[T323_re][col] = M_in[T423_re][ind];
    M_out[T323_im][col] = M_in[T423_im][ind];
    M_out[T333][col] = M_in[T433][ind];
    }
  for (np = 0; np < Npolar_out; np++)
    fwrite(&M_out[np][0], sizeof(float), Ncolfin, out_file[np]);
  }

  if (strcmp(DataFormat, "T4") == 0) {
  for (col = 0; col < Ncolfin; col++) {
    ind = col * SubSampRG + Ncoloffset;
    M_out[T411][col] = M_in[T411][ind];
    M_out[T412_re][col] = M_in[T412_re][ind];
    M_out[T412_im][col] = M_in[T412_im][ind];
    M_out[T413_re][col] = M_in[T413_re][ind];
    M_out[T413_im][col] = M_in[T413_im][ind];
    M_out[T414_re][col] = M_in[T414_re][ind];
    M_out[T414_im][col] = M_in[T414_im][ind];
    M_out[T422][col] = M_in[T422][ind];
    M_out[T423_re][col] = M_in[T423_re][ind];
    M_out[T423_im][col] = M_in[T423_im][ind];
    M_out[T424_re][col] = M_in[T424_re][ind];
    M_out[T424_im][col] = M_in[T424_im][ind];
    M_out[T433][col] = M_in[T433][ind];
    M_out[T434_re][col] = M_in[T434_re][ind];
    M_out[T434_im][col] = M_in[T434_im][ind];
    M_out[T444][col] = M_in[T444][ind];
    }
  for (np = 0; np < Npolar_out; np++)
    fwrite(&M_out[np][0], sizeof(float), Ncolfin, out_file[np]);
  }

  if (strcmp(DataFormat, "C3") == 0) {
  for (col = 0; col < Ncolfin; col++) {
    ind = col * SubSampRG + Ncoloffset;
    M_out[C311][col] = (M_in[T411][ind] + 2 * M_in[T412_re][ind] + M_in[T422][ind]) / 2;
    M_out[C312_re][col] = (M_in[T413_re][ind] + M_in[T423_re][ind]) / sqrt(2);
    M_out[C312_im][col] = (M_in[T413_im][ind] + M_in[T423_im][ind]) / sqrt(2);
    M_out[C313_re][col] = (M_in[T411][ind] - M_in[T422][ind]) / 2;
    M_out[C313_im][col] = -M_in[T412_im][ind];
    M_out[C322][col] = M_in[T433][ind];
    M_out[C323_re][col] = (M_in[T413_re][ind] - M_in[T423_re][ind]) / sqrt(2);
    M_out[C323_im][col] = (-M_in[T413_im][ind] + M_in[T423_im][ind]) / sqrt(2);
    M_out[C333][col] = (M_in[T411][ind] - 2 * M_in[T412_re][ind] + M_in[T422][ind]) / 2;
    }
  for (np = 0; np < Npolar_out; np++)
    fwrite(&M_out[np][0], sizeof(float), Ncolfin, out_file[np]);
  }

  if (strcmp(DataFormat, "C4") == 0) {
  for (col = 0; col < Ncolfin; col++) {
    ind = col * SubSampRG + Ncoloffset;
    M_out[C411][col] = (M_in[T411][ind] + 2 * M_in[T412_re][ind] + M_in[T422][ind]) / 2.;
    M_out[C412_re][col] = (M_in[T413_re][ind] + M_in[T423_re][ind] + M_in[T414_im][ind] + M_in[T424_im][ind]) / 2.;
    M_out[C412_im][col] = (M_in[T413_im][ind] + M_in[T423_im][ind] - M_in[T414_re][ind] - M_in[T424_re][ind]) / 2.;
    M_out[C413_re][col] = (M_in[T413_re][ind] + M_in[T423_re][ind] - M_in[T414_im][ind] - M_in[T424_im][ind]) / 2.;
    M_out[C413_im][col] = (M_in[T413_im][ind] + M_in[T423_im][ind] + M_in[T414_re][ind] + M_in[T424_re][ind]) / 2.;
    M_out[C414_re][col] = (M_in[T411][ind] - M_in[T422][ind]) / 2.;
    M_out[C414_im][col] = (-2 * M_in[T412_im][ind]) / 2.;
    M_out[C422][col] = (M_in[T433][ind] + 2 * M_in[T434_re][ind] + M_in[T444][ind]) / 2.;
    M_out[C423_re][col] = (M_in[T433][ind] - M_in[T444][ind]) / 2.;
    M_out[C423_im][col] = (2 * M_in[T434_re][ind]) / 2.;
    M_out[C424_re][col] = (M_in[T413_re][ind] - M_in[T423_re][ind] + M_in[T414_im][ind] - M_in[T424_im][ind]) / 2.;
    M_out[C424_im][col] = (-M_in[T413_im][ind] + M_in[T423_im][ind] + M_in[T414_re][ind] - M_in[T424_re][ind]) / 2.;
    M_out[C433][col] = (M_in[T433][ind] + M_in[T444][ind] - 2 * M_in[T434_im][ind]) / 2.;
    M_out[C434_re][col] = (M_in[T413_re][ind] - M_in[T423_re][ind] - M_in[T414_im][ind] + M_in[T424_im][ind]) / 2.;
    M_out[C434_im][col] = (-M_in[T413_im][ind] + M_in[T423_im][ind] - M_in[T414_re][ind] + M_in[T424_re][ind]) / 2.;
    M_out[C444][col] = (M_in[T411][ind] - 2 * M_in[T412_re][ind] + M_in[T422][ind]) / 2;
    }
  for (np = 0; np < Npolar_out; np++)
    fwrite(&M_out[np][0], sizeof(float), Ncolfin, out_file[np]);
  }

  for (l = 1; l < SubSampAZ; l++) {
    fread(&M_tmp[0][0], sizeof(float), Ncol, in_file[T11]);
    fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T12]);
    fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T13]);
    fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T14]);
    fread(&M_tmp[0][0], sizeof(float), Ncol, in_file[T22]);
    fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T23]);
    fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T24]);
    fread(&M_tmp[0][0], sizeof(float), Ncol, in_file[T33]);
    fread(&M_tmp[0][0], sizeof(float), 2*Ncol, in_file[T34]);
    fread(&M_tmp[0][0], sizeof(float), Ncol, in_file[T44]);
    }

  }

  for (np = 0; np < Npolar_in; np++)
  fclose(in_file[np]);
  for (np = 0; np < Npolar_out; np++)
  fclose(out_file[np]);

  free_matrix_float(M_out, Npolar_out);
  free_matrix_float(M_tmp, Npolar_in);
  free_matrix_float(M_in, 16);

  return 1;
}
