/*test machine: KH1250-16
* date: 02/13/24
* name: Sharmarke Ahmed, Bradley Johnson, John Van Sloun
* x500: ahme0344, joh22202, vansl060
*/

#include "main.h"

// hint: to access the files or directories in the parent directory, use "../name_of_your_data"
int main(int argc, char **argv) {
	
	// Validate command-line argument.
	if (argc > 3 || argc < 2) {
		printf("Incorrect number of arguments...\n");
		printf("Usage 1: ./depGraph input_text_file\n");
		printf("Usage 2: ./depGraph -p input_text_file\n");
		exit(1);
	}

	// mode 0 DFS
	// mode 1 parallel
	int mode;
	FILE *input;
	char *filename;

	// No mode argument
	if (argc == 2) {
		mode = 0;
		filename = argv[1];
		input = fopen(filename, "r");
	// Mode argument
	} else if (argc == 3) {
		// Mode argument is -p
		if (strcmp(argv[1], "-p") == 0) {
			mode = 1;
			filename = argv[2];
			input = fopen(filename, "r");
		}
		// Invalid mode argument
		else {
			printf("Invalid mode...\n");
			printf("Usage 1: ./depGraph input_text_file\n");
			printf("Usage 2: ./depGraph -p input_text_file\n");
			exit(1);
		}
	}
	// Input file not found
	if(!input) {
		printf("File %s not found...\n", filename);
		printf("Exiting...\n");
		exit(1);
	}

	char cmds[32][550];
	struct DepGraph *depGraph = createDepGraph(input, cmds);

	fclose(input);

	processGraph(depGraph, cmds, mode);

	freeGraph(depGraph);

	return 0;
}
