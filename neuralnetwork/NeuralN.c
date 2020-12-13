#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


struct node
{
	double input;       // somme des entrées
	double threshold;   // seuil
	double output;      // -1 or 1 (résultat de la sigmoïde)
	double error;       // valeur de l'erreur faite
};

	// Define layers nodes number
#define NB_INPUT_NODES      1024
#define NB_HIDDEN1_NODES    256
#define NB_HIDDEN2_NODES    128
#define NB_OUTPUT_NODES     64

#define NET_FILENAME  "ocr.net"

	// Define layers
struct node InputLayer[NB_INPUT_NODES];
struct node Hidden1Layer[NB_HIDDEN1_NODES];
struct node Hidden2Layer[NB_HIDDEN2_NODES];
struct node OutputLayer[NB_OUTPUT_NODES];

	// Define layers weights
		// Input 	-> 	Hidden1
double InputWeight[NB_INPUT_NODES][NB_HIDDEN1_NODES];
		// Hidden1 	-> 	Hidden2
double Hidden1Weight[NB_HIDDEN1_NODES][NB_HIDDEN2_NODES];
		// Hidden2 	->	Output
double Hidden2Weight[NB_HIDDEN2_NODES][NB_OUTPUT_NODES];


	// Init all the network with a [node, node, ..., node]
	// with node.input and node.threshold = 0
void InitNetwork()
{
	for (int i = 0; i<NB_INPUT_NODES; i++){
		InputLayer[i].input = 0;
		InputLayer[i].threshold = 0;
	}
	for (int i = 0; i<NB_HIDDEN1_NODES; i++){
		Hidden1Layer[i].input = 0;
		Hidden1Layer[i].threshold = 0;
	}
	for (int i = 0; i<NB_HIDDEN2_NODES; i++){
		Hidden2Layer[i].input = 0;
		Hidden2Layer[i].threshold = 0;
	}
	for (int i = 0; i<NB_OUTPUT_NODES; i++){
		OutputLayer[i].input = 0;
		OutputLayer[i].threshold = 0;
	}
}



double Sigmoid(double x)
{
  return(1.0 / (1.0 + exp(-x)));
}

double Derivate_Sigmoid(double x)
{
  return x * (1.0 - x);
}


void ForwardPropagation()
{

// --------------[ Premier niveau de propagation ]
	for (int i = 0; i<NB_HIDDEN1_NODES; i++){
		Hidden1Layer[i].input = 0;
	}
	for (int i = 0; i<NB_INPUT_NODES; i++){
		for (int j = 0; j<NB_HIDDEN1_NODES; j++){
			Hidden1Layer[j].input += InputLayer[i].output * InputWeight[i][j];
		}
	}
		// input -> (input - threshold)
		// output -> sigmoid(input)
	for (int i = 0; i<NB_HIDDEN1_NODES; i++){
		Hidden1Layer[i].input -= Hidden1Layer[i].threshold;
		Hidden1Layer[i].output = Sigmoid(Hidden1Layer[i].input);
	}


// --------------[ Deuxième niveau de propagation ]
	for (int i = 0; i<NB_HIDDEN2_NODES; i++){
		Hidden2Layer[i].input = 0;
	}
	for (int i = 0; i<NB_HIDDEN1_NODES; i++){
		for (int j = 0; j<NB_HIDDEN2_NODES; j++){
			Hidden2Layer[j].input += Hidden1Layer[i].output * Hidden1Weight[i][j];
		}
	}
		// input -> (input - threshold)
		// output -> sigmoid(input)
	for (int i = 0; i<NB_HIDDEN2_NODES; i++){
		Hidden2Layer[i].input -= Hidden2Layer[i].threshold;
		Hidden2Layer[i].output = Sigmoid(Hidden2Layer[i].input);
	}


// --------------[ Troisième niveau de propagation ]
	for (int i = 0; i<NB_OUTPUT_NODES; i++){
		OutputLayer[i].input = 0;
	}
	for (int i = 0; i<NB_HIDDEN2_NODES; i++){
		for (int j = 0; j<NB_OUTPUT_NODES; j++){
			OutputLayer[j].input += Hidden2Layer[i].output * Hidden2Weight[i][j];
		}
	}
		// input -> (input - threshold)
		// output -> sigmoid(input)
	for (int i = 0; i<NB_OUTPUT_NODES; i++){
		OutputLayer[i].input -= OutputLayer[i].threshold;
		OutputLayer[i].output = Sigmoid(OutputLayer[i].input);
	}
}




void BackPropagation(int expectedIndex)
{
	double quadratic = 0;

// ------------------[ PREMIER NIVEAU DE BACK PROPAGATION ]
	for (int i = 0; i<NB_OUTPUT_NODES; i++){
		int expectedValue = (i == expectedIndex);
		OutputLayer[i].error = (OutputLayer[i].output - 1.0 * expectedValue ); // * Derivate_Sigmoid(OutputLayer[i].input);
		quadratic += (OutputLayer[i].output - expectedValue ) * (OutputLayer[i].output - expectedValue );
	}

	fprintf(stderr, "Quadratic error = %lf\n", quadratic);

// ------------------[ DEUXIEME NIVEAU DE BACK PROPAGATION ]
	for (int i = 0; i<NB_HIDDEN2_NODES; i++){
		double error_sum = 0;
		for (int j = 0; j<NB_OUTPUT_NODES; j++){
			error_sum += OutputLayer[j].error * Hidden2Weight[i][j];
		}
		Hidden2Layer[i].error = error_sum * Derivate_Sigmoid(Hidden2Layer[i].input);
	}


// ------------------[ TROISIEME NIVEAU DE BACK PROPAGATION ]
	for (int i = 0; i<NB_HIDDEN1_NODES; i++){
		double error_sum = 0;
		for (int j = 0; j<NB_HIDDEN2_NODES; j++){
			error_sum += Hidden2Layer[j].error * Hidden1Weight[i][j];
		}
		Hidden1Layer[i].error = error_sum * Derivate_Sigmoid(Hidden1Layer[i].input);
	}
}



void UpdateWeights(double epsilon)
{
// ------------------[ PREMIER NIVEAU D'UPDATE DES POIDS ]
	for (int i = 0; i<NB_INPUT_NODES; i++){
		for (int j = 0; j<NB_HIDDEN1_NODES; j++){
			InputWeight[i][j] = epsilon * Hidden1Layer[j].error * InputLayer[i].output;
		}
	}

// ------------------[ PREMIER NIVEAU D'UPDATE DES POIDS ]
	for (int i = 0; i<NB_HIDDEN1_NODES; i++){
		for (int j = 0; j<NB_HIDDEN2_NODES; j++){
			Hidden1Weight[i][j] = epsilon * Hidden2Layer[j].error * Hidden1Layer[i].output;
		}
	}

// ------------------[ PREMIER NIVEAU D'UPDATE DES POIDS ]
	for (int i = 0; i<NB_HIDDEN2_NODES; i++){
		for (int j = 0; j<NB_OUTPUT_NODES; j++){
			Hidden2Weight[i][j] = epsilon * OutputLayer[j].error * Hidden2Layer[i].output;
		}
	}
}



void UpdateThreshold( double epsilon )
{
// ------------------[ PREMIER NIVEAU D'UPDATE DES SEUILS ]
	for (int j = 0; j<NB_HIDDEN1_NODES; j++){
		Hidden1Layer[j].threshold -= Hidden1Layer[j].error * epsilon;
	}

// ------------------[ PREMIER NIVEAU D'UPDATE DES SEUILS ]
	for (int j = 0; j<NB_HIDDEN2_NODES; j++){
		Hidden2Layer[j].threshold -= Hidden2Layer[j].error * epsilon;
	}

// ------------------[ PREMIER NIVEAU D'UPDATE DES SEUILS ]
	for (int j = 0; j<NB_OUTPUT_NODES; j++){
		OutputLayer[j].threshold -= OutputLayer[j].error * epsilon;
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

	for (int i = 0; i < NB_HIDDEN1_NODES; i ++) {
		fprintf(fp, "%lf\n", Hidden1Layer[i].threshold);
		for (int j = 0; j < NB_INPUT_NODES; j ++)
			fprintf(fp, "%lf\n", InputWeight[j][i]);
	}

	for (int i = 0; i < NB_HIDDEN2_NODES; i ++) {
		fprintf(fp, "%lf\n", Hidden2Layer[i].threshold);
		for (int j = 0; j < NB_HIDDEN1_NODES; j ++)
			fprintf(fp, "%lf\n", Hidden1Weight[j][i]);
	}

	for (int i = 0; i < NB_OUTPUT_NODES; i ++) {
		fprintf(fp, "%lf\n", OutputLayer[i].threshold);
		for (int j = 0; j < NB_HIDDEN2_NODES; j ++)
			fprintf(fp, "%lf\n", Hidden2Weight[j][i]);
	}

	fclose(fp);
	return 0;
}


double randomValue()
{
	double r;
	r = random() % 1024 - 512;
	r = r / 1024.0;
	return r;
}

#define LINE_LENGTH 64

int ImportNet(const char *filename)
{
	FILE *fp;
	char line[LINE_LENGTH];

	for (int i = 0; i < NB_HIDDEN1_NODES; i ++) {
		Hidden1Layer[i].threshold = randomValue();
		for (int j = 0; j < NB_INPUT_NODES; j ++)
			InputWeight[j][i] = randomValue();
	}
	for (int i = 0; i < NB_HIDDEN2_NODES; i ++) {
		Hidden2Layer[i].threshold = randomValue();
		for (int j = 0; j < NB_HIDDEN1_NODES; j ++)
			Hidden1Weight[j][i] = randomValue();
	}

	for (int i = 0; i < NB_OUTPUT_NODES; i ++) {
		OutputLayer[i].threshold = randomValue();
		for (int j = 0; j < NB_HIDDEN2_NODES; j ++)
			Hidden2Weight[j][i] = randomValue();
	}


	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf (stderr, "[OCR] - Unable to open %s\n", filename);
		return -1;
	}

	for (int i = 0; i < NB_HIDDEN1_NODES; i ++) {
		if ((fgets(line, LINE_LENGTH, fp) == NULL)
		 || (sscanf(line, "%lf", &(Hidden1Layer[i].threshold)) != 1)) {
			return -1;
		}
		for (int j = 0; j < NB_INPUT_NODES; j ++) {
			if ((fgets(line, LINE_LENGTH, fp) == NULL)
			 || (sscanf(line, "%lf", &(InputWeight[j][i])) != 1)) {
				return -1;
			}
		}
	}

	for (int i = 0; i < NB_HIDDEN2_NODES; i ++) {
		if ((fgets(line, LINE_LENGTH, fp) == NULL)
		 || (sscanf(line, "%lf", &(Hidden2Layer[i].threshold)) != 1)) {
			return -1;
		}
		for (int j = 0; j < NB_HIDDEN1_NODES; j ++)
			if ((fgets(line, LINE_LENGTH, fp) == NULL)
			 || (sscanf(line, "%lf", &(Hidden1Weight[j][i])) != 1)) {
				return -1;
			}
	}

	for (int i = 0; i < NB_OUTPUT_NODES; i ++) {
		if ((fgets(line, LINE_LENGTH, fp) == NULL)
		 || (sscanf(line, "%lf", &(Hidden2Layer[i].threshold)) != 1)) {
			return -1;
		}

		for (int j = 0; j < NB_HIDDEN2_NODES; j ++)
			if ((fgets(line, LINE_LENGTH, fp) == NULL)
			 || (sscanf(line, "%lf", &(Hidden2Weight[j][i])) != 1)) {
				return -1;
			}
	}

	fclose(fp);
	return 0;
}




int NeuralOCR(double letterMatrix[32][32], int expectedValue, double epsilon)
{

	ImportNet(NET_FILENAME);

	for(int i = 0; i < 32; i++)
	{
		for(int j = 0; j < 32; j++)
		{
			InputLayer[i*32+j].output = letterMatrix[i][j];	
		}
	}

		// Effectue l'apprentissage
	ForwardPropagation();


	int resultChar = 0;
	for(int i = 1; i < NB_OUTPUT_NODES; i++)
	{
		printf("%.2f ", OutputLayer[i].output);
		if (OutputLayer[i].output > OutputLayer[resultChar].output)
		{
			resultChar = i;
		}
	}
	printf("\n");
	if (resultChar == NB_OUTPUT_NODES)
		fprintf(stderr, "No result\n");
	else
		fprintf(stderr, "Found: %d\n", resultChar);


	if(expectedValue >= 0){
		BackPropagation(expectedValue);
		UpdateWeights(epsilon);
		UpdateThreshold(epsilon);
		ExportNet(NET_FILENAME);
	}

	return resultChar;

}




int LoadMatrixFromText(const char *filename, double letterMatrix[32][32])
{
	FILE *fp;
	char line[LINE_LENGTH];

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf (stderr, "[OCR] - Unable to open %s\n", filename);
		return -1;
	}

	for (int i = 0;  i < 32; i++) {
		if (fgets(line, LINE_LENGTH, fp) == NULL) {
			fprintf(stderr, "[OCR] Wrong file format line %d", i+1);
			return -1;
		}
		for (int j = 0; j < 32; j ++) {
			switch(line[j]) {
				case '0': letterMatrix[i][j] = 0; break;
				case '1': letterMatrix[i][j] = 0; break;
				default:
					fprintf(stderr, "[OCR] Wrong file format line %d", i+1);
					return -1;
			}
		}
	}
	fclose(fp);
	return 0;
}

//gcc NeuralN.c -o neural -lm -DTRAINING_PHASE

#ifdef TRAINING_PHASE

int main(int argc, char *argv[])
{
	double letterMatrix[32][32];

	if (argc < 2) {
		fprintf(stderr, "usage: %s <file> [<expected>]\n", argv[0]);
		exit(1);
	}

	if (LoadMatrixFromText(argv[1], letterMatrix) != 0)
		exit(1);

	int expected = -1;

	if (argc > 2) {
		if (sscanf(argv[2], "%d", &expected) != 1) {
			fprintf(stderr, "Expected not understood\n");
			exit(1);
		}
	}

	char result = NeuralOCR(letterMatrix, expected, 0.01);

// [a, b, c, ..., z, A, B, C, ..., Z, _, _, _, _, ...]
	if (result < 25)
		printf("Expected %d, Result (%d) : %c\n", expected, result, result + 'a');
	else
		printf("Expected %d, Result (%d) : %c\n", expected, result, result + 'A');
	return 0;
}

#endif

