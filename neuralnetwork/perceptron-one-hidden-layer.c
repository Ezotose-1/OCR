
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>


struct node
{
	double input;       // somme des entrées
	double output;      // -1 or 1 (résultat de la sigmoïde)
	double error;       // valeur de l'erreur faite
};


// Define layers nodes number
#define NB_INPUT_NODES      1024
#define NB_HIDDEN_NODES     256
#define NB_OUTPUT_NODES     64


#define NET_FILENAME  "ocr.net"



	// Define layers
struct node InputLayer[NB_INPUT_NODES];
struct node HiddenLayer[NB_HIDDEN_NODES];
struct node OutputLayer[NB_OUTPUT_NODES];


	// Define layers weights
		// Input 	-> Hidden
double InputWeight[NB_INPUT_NODES][NB_HIDDEN_NODES];
		// Hidden 	-> Output
double HiddenWeight[NB_HIDDEN_NODES][NB_OUTPUT_NODES];



double Sigmoid(double x)
{
	return(1.0 / (1.0 + exp(-x)));
}



double DerivatedSigmoid(double x)
{
	double y = Sigmoid(x);
	return y * (1.0 - y);
}



double ComputeOutput(double input)
{
	return Sigmoid(input);
}



double ComputeDerivatedError(double error, double input)
{
	return error * DerivatedSigmoid(input);
}



int ForwardPropagation()
{

// --------------[ Premier niveau de propagation ]
	for (int i = 0; i < NB_HIDDEN_NODES; i++){
		HiddenLayer[i].input = 0;
		for (int j = 0; j < NB_INPUT_NODES; j++){
			HiddenLayer[i].input += InputLayer[j].output * InputWeight[j][i];
		}
		HiddenLayer[i].output = ComputeOutput(HiddenLayer[i].input);
	}

/*for (int i = 0; i < NB_HIDDEN_NODES; i ++)
	printf ("(%.2lf) ", HiddenLayer[i].output);
printf("\n");
*/

// --------------[ Second niveau de propagation ]
	for (int i = 0; i < NB_OUTPUT_NODES; i++){
		OutputLayer[i].input = 0;
		for (int j = 0; j < NB_HIDDEN_NODES; j++){
			OutputLayer[i].input += HiddenLayer[j].output * HiddenWeight[j][i];
		}
		OutputLayer[i].output = ComputeOutput(OutputLayer[i].input);
	}

/*for (int i = 0; i < NB_OUTPUT_NODES; i ++)
	printf ("(%.2lf) ", OutputLayer[i].output);
printf("\n");
*/

//               Result
	int max = 0;
	for(int i = 1; i < NB_OUTPUT_NODES; i++){
		if (OutputLayer[i].output > OutputLayer[max].output){
			max = i;
		}
	}

	return max;
}


void BackPropagation(int expectedIndex)
{

// ------------------[ PREMIER NIVEAU DE BACK PROPAGATION ]

	for (int i = 0; i < NB_OUTPUT_NODES; i++){
		int expectedValue = (i == expectedIndex);
		OutputLayer[i].error = ComputeDerivatedError(OutputLayer[i].output - 1.0 * expectedValue, OutputLayer[i].input);
	}

#ifdef _TRAINING_PHASE
	double quadratic = 0;
	for (int i = 0; i < NB_OUTPUT_NODES; i++){
		int expectedValue = (i == expectedIndex);
		quadratic += (OutputLayer[i].output - expectedValue ) * (OutputLayer[i].output - expectedValue );
	}
	fprintf(stderr, "Error: %.02lf%%\n", sqrt(quadratic/ NB_OUTPUT_NODES ) * 100.0);
#endif

// ------------------[ SECOND NIVEAU DE BACK PROPAGATION ]
	for (int i = 0; i < NB_HIDDEN_NODES; i++){
		double error_sum = 0;
		for (int j = 0; j < NB_OUTPUT_NODES; j++){
			error_sum += OutputLayer[j].error * HiddenWeight[i][j];
		}
		HiddenLayer[i].error = ComputeDerivatedError(error_sum, HiddenLayer[i].input);
	}
}



void UpdateWeights(double epsilon)
{
// ------------------[ PREMIER NIVEAU D'UPDATE DES POIDS ]
	for (int i = 0; i < NB_HIDDEN_NODES; i++){
		for (int j = 0; j < NB_INPUT_NODES; j++){
			InputWeight[j][i] -= epsilon * HiddenLayer[i].error * InputLayer[j].output;
		}
	}

// ------------------[ SECOND NIVEAU D'UPDATE DES POIDS ]
	for (int i = 0; i < NB_OUTPUT_NODES; i++){
		for (int j = 0; j < NB_HIDDEN_NODES; j++){
			HiddenWeight[j][i] -= epsilon * OutputLayer[i].error * HiddenLayer[j].output;
		}
	}
}



int ExportNet(const char *filename)
{
	FILE *fp;

	fp = fopen(filename, "w");
	if (fp == NULL) {
		fprintf (stderr, "[OCR] - Unable to open %s\n", filename);
		return -1;
	}

	for (int i = 0; i < NB_HIDDEN_NODES; i ++) {
		for (int j = 0; j < NB_INPUT_NODES; j ++)
			fprintf(fp, "%lf\n", InputWeight[j][i]);
	}

	for (int i = 0; i < NB_OUTPUT_NODES; i ++) {
		for (int j = 0; j < NB_HIDDEN_NODES; j ++)
			fprintf(fp, "%lf\n", HiddenWeight[j][i]);
	}

	fclose(fp);
	return 0;
}



double randomValue()
{
	double r;
	r = random() % 1024 - 512;
	r = r / 512.0;
	return r;
}



void RamdomizeNet()
{
	srandom(1);
	for (int i = 0; i < NB_HIDDEN_NODES; i ++) {
		for (int j = 0; j < NB_INPUT_NODES; j ++)
			InputWeight[j][i] = randomValue();
	}

	for (int i = 0; i < NB_OUTPUT_NODES; i ++) {
		for (int j = 0; j < NB_HIDDEN_NODES; j ++)
			HiddenWeight[j][i] = randomValue();
	}
}



#define IMPORT_NET_LINE_LENGTH 64

int ImportNet(const char *filename)
{
	FILE *fp;
	char line[IMPORT_NET_LINE_LENGTH];

	setlocale(LC_ALL, "C");

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf ("[OCR] - Unable to open %s\n", filename);
		return -1;
	}

	for (int i = 0; i < NB_HIDDEN_NODES; i ++) {
		for (int j = 0; j < NB_INPUT_NODES; j ++) {
			if ((fgets(line, IMPORT_NET_LINE_LENGTH, fp) == NULL)
			 || (sscanf(line, "%lf", &(InputWeight[j][i])) != 1)) {
				return -1;
			}
		}
	}

	for (int i = 0; i < NB_OUTPUT_NODES; i ++) {
		for (int j = 0; j < NB_HIDDEN_NODES; j ++) {
			if ((fgets(line, IMPORT_NET_LINE_LENGTH, fp) == NULL)
			 || (sscanf(line, "%lf", &(HiddenWeight[j][i])) != 1)) {
				return -1;
			}
		}
	}

	fclose(fp);

	return 0;
}



void FillInputLayer(double letterMatrix[32][32])
{
	for(int i = 0; i < 32; i++)
	{
		for(int j = 0; j < 32; j++)
		{
			InputLayer[i * 32 + j].output = letterMatrix[i][j];
		}
	}
}



int NeuralOCR(double letterMatrix[32][32], int expectedValue, double epsilon, int loops)
{

	RamdomizeNet();
	ImportNet(NET_FILENAME);

	if(expectedValue >= 0){

		for (int i = 0; i < loops; i ++) {
			FillInputLayer(letterMatrix);
			ForwardPropagation();
			BackPropagation(expectedValue);
			UpdateWeights(epsilon);
		}

		ExportNet(NET_FILENAME);
	}

	FillInputLayer(letterMatrix);
	return ForwardPropagation();
}




int LoadMatrixFromText(const char *filename, double letterMatrix[32][32])
{
	FILE *fp;
	char line[IMPORT_NET_LINE_LENGTH];

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf (stderr, "[OCR] - Unable to open %s\n", filename);
		return -1;
	}

	for (int i = 0;  i < 32; i++) {
		if (fgets(line, IMPORT_NET_LINE_LENGTH, fp) == NULL) {
			fprintf(stderr, "[OCR] Wrong file format line %d", i+1);
			return -1;
		}
		for (int j = 0; j < 32; j ++) {
			switch(line[j]) {
				case '0': letterMatrix[i][j] = 0.0; break;
				case '1': letterMatrix[i][j] = 1.0; break;
				default:
					fprintf(stderr, "[OCR] Wrong file format line %d", i+1);
					return -1;
			}
		}
	}
	fclose(fp);
	return 0;
}

//gcc NeuralN.c -o neural -lm -D_TRAINING_PHASE

#ifdef _TRAINING_PHASE

int main(int argc, char *argv[])
{
	double letterMatrix[32][32];

	if (argc < 2) {
		fprintf(stderr, "usage: %s <file> [<expected> [<epsilon> [<loops>]]]\n", argv[0]);
		exit(1);
	}

	if (LoadMatrixFromText(argv[1], letterMatrix) != 0)
		exit(1);

	int expected = -1;

	if (argc > 2) {
		if (sscanf(argv[2], "%d", &expected) != 1) {
			fprintf(stderr, "Invalid `expected` arg\n");
			exit(1);
		}
	}

	double epsilon = 0.5;

	if (argc > 3) {
		if (sscanf(argv[3], "%lf", &epsilon) != 1) {
			fprintf(stderr, "Invalid `epsilon` arg\n");
			exit(1);
		}
	}

	int loops = 1;

	if (argc > 4) {
		if (sscanf(argv[4], "%d", &loops) != 1) {
			fprintf(stderr, "Invalid `loops` arg\n");
			exit(1);
		}
	}

	int result = NeuralOCR(letterMatrix, expected, epsilon, loops);

// [a, b, c, ..., z, A, B, C, ..., Z, _, _, _, _, ...]

	if (expected >= 0) {
		if (expected <= 25)
			printf("Expected: %2d (%c), ", expected, expected + 'a');
		else
			printf("Expected: %2d (%c), ", expected, expected - 26 + 'A');
	}

	if (result <= 25)
		printf("Result: %2d (%c)", result, result + 'a');
	else
		printf("Result: %2d (%c)", result, result - 26 + 'A');

	if (result == expected)
		printf("    YEEEHAAA \\o/");
	printf("\n");

	return 0;
}

#endif

