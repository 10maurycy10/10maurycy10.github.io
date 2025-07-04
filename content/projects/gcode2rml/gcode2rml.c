// G-code to RML-1 converter for Roland CNC mills
// Tested in the Roland MDX-540, should mostly work on other
// Roland mills.
//
// All movement is relative, starting position is used as the orgin.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define CIRC_RES 10 // Points per mm

FILE* out = 0; FILE* in = 0; 

// Warn user about major problems.
void borked() {
	fprintf(stderr, "gcode2rml: Attempting to continue, but toolpath is likely borked.\n");
}

//////////////////
// RML Commands //
//////////////////
// The mill is kept in relative mode, all movements are relative to the starting position.
// This results in conistant behavor, regardels of cordinate system used in the g-code.
// Implementing functions lieke circular motion in relative cordinates is quite annoying,
// so the move function takes abosolute cordiates, doing the conversion internaly.

// Spindle speed and feed rate are set directly by the g-code parser

#ifdef plotter
void init() {
	fprintf(out, "^PU;\n");
}
#else
void init() {
	fprintf(out, "V10;\r\n"); // Sane default movement speed, 10 mm/s
	fprintf(out, "!DW;\r\n"); // No dwell time
	fprintf(out, "^PR;\r\n"); // Relative mode
	fprintf(out, "!MC0;\r\n"); // Stop spindle
	fprintf(out, "!RC15;\r\n"); // Default to 12000 RPM, the max rotational speed of my machine
}
#endif

float lin_units = 0.01; // 10s of um default
// Track integers sent to machine to avoid accumulating errors.
int rml_last_x = 0, rml_last_y = 0, rml_last_z = 0, rml_last_a = 0;

#ifdef plotter
void move(float x, float y, float z, float a) {
	// This uses milinches
	x /= 25.4; y /= 25.4; z /= 25.4;
	x *= 1000; y *= 1000; z *= 1000;
	// Compute movement neaded to reach position
	int dx = x - rml_last_x;
	int dy = y - rml_last_y;
	int dz = z - rml_last_z;
	rml_last_x += dx;
	rml_last_y += dy;
	rml_last_z += dz;
	// Send to mill.
	fprintf(out, "^PR%d,%d;\n", dx, dy);
	if (dz > 0) fprintf(out, "^PU;\n");
	if (dz < 0) fprintf(out, "^PD;\n");
}
#else
void move(float x, float y, float z, float a) { // Abosolute movement in mm
	// Scale to micrometers
	x /= lin_units; y /= lin_units; z /= lin_units; a *= 1000;
	// Compute movement neaded to reach position
	int dx = x - rml_last_x;
	int dy = y - rml_last_y;
	int dz = z - rml_last_z;
	int da = a - rml_last_a;
	rml_last_x += dx;
	rml_last_y += dy;
	rml_last_z += dz;
	rml_last_a += da;
	// Send to mill.
	fprintf(out, "!ZE ");
	if (dx) fprintf(out, "X%d ", dx);
	if (dy) fprintf(out, "Y%d ", dy);
	if (dz) fprintf(out, "Z%d ", dz);
	if (da) fprintf(out, "A%f ", ((float)da)/1000);
	fprintf(out, ";\r\n");
}
#endif

////////////////////
// G code parsing //
////////////////////

// Read a number pointed to by string, advance the pointer the first non numeric charater
int read_number(char** string) {
	float number = 0;
	float sign = 1;
	if (**string == '-') {
		sign = -1;
		(*string)++;
	}
	while (**string >= '0' && **string <= '9') {
		number *= 10;
		number += **string - '0';
		(*string)++;
	}
	return number * sign;
}


// Read non-integer numbers, inserts an implied decimal point of one is not included.
int default_decimal_places = 3;
float read_float(char** string) {
	float number = 0;
	float sign = 1;
	if (**string == '-') {
		sign = -1;
		(*string)++;
	}
	while (**string >= '0' && **string <= '9') {
		number *= 10;
		number += **string - '0';
		(*string)++;
	}
	if (**string == '.') {
		(*string)++;
		float place = .1;
		while (**string >= '0' && **string <= '9') {
			number += (**string - '0') * place;
			place /= 10;
			(*string)++;
		}
	} else {
		for (int i = 0; i < default_decimal_places; i++) number /= 10;
	}
	return number * sign;
}

// G-code cordinate system settings 
int relative = 0;
float offset_x = 0, offset_y = 0, offset_z = 0, offset_a = 0;
float last_x = 0, last_y = 0, last_z = 0, last_a = 0;
float scale = 1; // Scale factor to convert cordinates to mm.

void gcode_move(float x, float y, float z, float a, int change_x, int change_y, int change_z, int change_a) {
	x *= scale;
	y *= scale;
	z *= scale;
	// If the G-code indicates relative cordinates, convert to absolute
	if (relative) {
		x += last_x;
		y += last_y;
		z += last_z;
		a += last_a;
	}
	if (!change_x) x = last_x;
	if (!change_y) y = last_y;
	if (!change_z) z = last_z;
	if (!change_a) a = last_a;
	// Send to mill
	last_x = x;
	last_y = y;
	last_z = z;
	last_a = a;
	move(offset_x + x, offset_y + y, offset_z + z, offset_a + a);
}

// Plane used for circular interpolation, 0 = x, 1 = y, 2 = z.
int circular0 = 0;
int circular1 = 1;

// dir = 1: ccw
// dir = -1: Cw 
// Have_r = 0: Use center
// Have_r = 1: Use radius
void circular(
	float x, float y, float z,
	float i, float j, float k,
	float dir,
	int have_x, int have_y, int have_z
) {
	// Convert inputs to mm
	x *= scale;
	y *= scale;
	z *= scale;
	i *= scale;
	j *= scale;
	k *= scale;
	// Find starting position
	float start[3] = {last_x, last_y, last_z};
	// Find destination, converting to absolute if needed
	if (relative) {
		x += last_x;
		y += last_y;
		z += last_z;
	}
	float dst[3] = {last_x, last_y, last_z};
	if (have_x) dst[0] = x;
	if (have_y) dst[1] = y;
	if (have_z) dst[2] = z;
	// Find center point
	float center[3] = {i+last_x, j+last_y, k+last_z};
	// Interpolate
	float start_angle = atan2(start[circular0] - center[circular0], start[circular1] - center[circular1]);
	float end_angle = atan2(dst[circular0] - center[circular0], dst[circular1] - center[circular1]);
	// Ensure the angles are increaseing (for dir=1), or decreasing (for dir=-1)
	if ((end_angle - start_angle)*(-dir) <= 0) {
		end_angle += M_PI * 2 * (-dir);
	}

	// Convert points to polar
	float start_distance = sqrt(pow(start[circular0] - center[circular0], 2) + pow(start[circular1] - center[circular1], 2));
	float end_distance = sqrt(pow(dst[circular0] - center[circular0], 2) + pow(dst[circular1] - center[circular1], 2));
	if (abs(start_distance - end_distance) > 1) {
		fprintf(stderr, "gcode2rml: Very large inconsistency in circular interpolation.\n");
		borked();
	}
	
	// Compute length of arc to find good number of steps
	float delta_angle = abs(fmod(abs(start_angle - end_angle)+M_PI, M_PI*2)) - M_PI;
	int steps = (start_distance * abs(delta_angle))  * CIRC_RES;
	if (steps < 10) steps = 10;

	// Linearly interpolate in polar cordinates
	for (int i = 0; i < steps; i++) {
		float a = (float)i / steps;
		float r = start_distance*(1-a) + end_distance*a;
		float angle = start_angle*(1-a) + end_angle*a;
		// Linearly interpolate for cordinates other then the plain of interpolation
		float point[3] = {
			start[0]*(1-a) + dst[0]*a,
			start[1]*(1-a) + dst[1]*a,
			start[2]*(1-a) + dst[2]*a,
		};
		point[circular0] = sin(angle) * r + center[circular0];
		point[circular1] = cos(angle) * r + center[circular1];
		// Send to mill
		move(offset_x + point[0], offset_y + point[1], offset_z + point[2], last_a);
	}
	// Ensure we ended up at the right point and update last_x
	move(offset_x + dst[0], offset_y + dst[1], offset_z + dst[2], last_a);
	last_x = dst[0];
	last_y = dst[1];
	last_z = dst[2];
}

// Macro for parsing the arguments to G00, G01, G02...
#define PARSER(val, have, chr) else if (*command == chr) {command++; val = read_float(&command); have = 1;}

float dir = 1; // Last used circular interpolation direction
void translate(char* command) {
	// Skip whitespace
	while (*command == ' ' || *command == '\t' || *command == '\r' || *command == '\n') command++;
	// Skip empty lines
	if (*command == 0) return;
	// Ignore start/end of program
	if (*command == '%') return;
	// Comments
	if (*command == '(') return;
	// Read first charater of command
	if (*command == 'G') { // General commands
		command++;
		int g_command = read_number(&command);
		// Read arguments
		int have_x = 0, have_y = 0, have_z = 0, have_a = 0, have_i = 0, have_j = 0, have_k = 0;
		float x, y, z, a, i = 0, j = 0, k = 0;
		while (1) {
			if (*command == ' ') {
				command++;
			}
			PARSER(x, have_x, 'X')
			PARSER(y, have_y, 'Y')
			PARSER(z, have_z, 'Z')
			PARSER(a, have_a, 'A')
			PARSER(i, have_i, 'I')
			PARSER(j, have_j, 'J')
			PARSER(k, have_k, 'K')
			else if (*command == 'R') {
				fprintf(stderr, "gcode2rml: Interpolation with explicit radius is not supported.\n");
				borked();
				return;
			} else if (*command == 'P') { // Not used
				command++; read_float(&command);
			} else break;
		}
		// Execute command
		switch (g_command) {
			case 0: case 1: // G00 and G01, linear movement
				gcode_move(x, y, z, a, have_x, have_y, have_z, have_a);
				break;
			case 2: // Circular movement
				if (have_a) {
					fprintf(stderr, "gcode2rml: 4th axis circular interpolation is not supported.\n");
					borked();
					return;
				}
				dir = -1;
				circular(x, y, z, i, j, k, dir, have_x, have_y, have_z);
				break;
			case 3: // Circular movement
				if (have_a) {
					fprintf(stderr, "gcode2rml: 4th axis circular interpolation is not supported.\n");
					borked();
					return;
				}
				dir = 1;
				circular(x, y, z, i, j, k, dir, have_x, have_y, have_z);
				break;
			case 10: // Set offset
				if (have_x) offset_x = x*scale;
				if (have_y) offset_y = y*scale;
				if (have_z) offset_z = z*scale;
				if (have_a) offset_a = a;
				break;
			// Plane of interpolation
			case 17: circular0 = 0; circular1 = 1; break;
			case 18: circular0 = 2; circular1 = 1; break;
			case 19: circular0 = 1; circular1 = 2; break;
			case 49: break;
			case 40: break; // Cancle tool offset
			case 20: scale = 25.4; default_decimal_places = 4; break; // In
			case 21: scale = 1; default_decimal_places = 3; break; // mm
			case 90: relative = 0; break; // Absolute movement
			case 91: relative = 1; break; // Relative movement
			default: 
				fprintf(stderr, "gcode2rml: Unkown command 'G%d', ignoring.\n", g_command);
				break;
		}
	#ifndef plotter
	} else if (*command == 'S') { // Set Spindle speed
		// Spindle speed is set in increments of 773 RPM (exerimentaly determined)
		// If number > 0: real speed = 400 + number * 773
		// If number = 0: real speed = 0
		command++;
		float speed = read_number(&command);
		if (speed < 100 && speed != 0) {
			fprintf(stderr, "gcode2rml: Warning, spindle speed codes are not supported.\n");
			fprintf(stderr, "gcode2rml: Spindle speed will likely be very wrong.\n");
		}
		int setting = round((speed - 400) / 772);
		if (setting == 0) setting = 1;
		fprintf(out, "!RC%d;\r\n", (int)setting);
	#endif
	} else if (*command == 'F') { // Set feedrate
		command++;
		float feedrate = read_float(&command) * scale;
		fprintf(out, "V%.1f;\r\n", feedrate / 60);
	} else if (*command == 'X' || *command == 'Y' || *command == 'Z') { // Bare cordinates, assume linear movement
		int have_x = 0, have_y = 0, have_z = 0, have_a = 0, have_j = 0, have_i = 0, have_k = 0;
		float x, y, z, a, j = 0, i = 0, k = 0;
		while (1) {
			if (*command == ' ') {
				command++;
			}
			PARSER(x, have_x, 'X')
			PARSER(y, have_y, 'Y')
			PARSER(z, have_z, 'Z')
			PARSER(a, have_a, 'A')
			PARSER(i, have_i, 'I')
			PARSER(j, have_j, 'J')
			PARSER(k, have_k, 'K')
			else if (*command == 'R') {
				fprintf(stderr, "gcode2rml: Interpolation with explicit radius is not supported.\n");
				borked();
				return;
			} else break;
		};
		if (have_i || have_j || have_k) {
			circular(x, y, z, i, j, k, dir,  have_x, have_y, have_z);
		} else {
			gcode_move(x, y, z, a, have_x, have_y, have_z, have_a);
		}
	} else if (*command == 'M') { // Misc commands
		command++;
		int g_command = read_number(&command);
		switch (g_command) {
			case 0: break; // Program stop
			case 1: break; // Optional stop
			case 30: case 2: break; // End of program
			#ifndef plotter
			case 3: case 4: fprintf(out, "!MC1;\r\n"); break; // Start spindle
			case 5: fprintf(out, "!MC0;\r\n"); break; // Stop spindle
			#endif
			case 6:
				if (*command == 'T') {
					command++;
					read_number(&command);
				}
				break; // Tool change, ignored
			default:
				fprintf(stderr, "gcode2rml: Warning, command M%d is not recognized, skipping line\n", g_command);
				break;
		}
	} else {
		fprintf(stderr, "gcode2rml: Unknown command class '%c', skipping line.\n", *command);	
		return;
	}
	// Recurse to handle any extra commands in the block
	if (*command != 0) translate(command);
}

void usage(char* name) {
	fprintf(stderr, "Usage: %s [input FILE] [output FILE] [flush]\n", name);
	fprintf(stderr, "Convert G-code into RML-1 for driving Roland CNC mills\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  input FILE   File containing g-code to be converted, defaults to standard input.\n");
	fprintf(stderr, "  output FILE  File to write generated g-code to.\n");
	fprintf(stderr, "  flush        Flush output buffer after processing a line, allows realtime control.\n");
	fprintf(stderr, "  nc           Assume mill is in 'NC' mode instead of 'RML' mode.\n");
	exit(1);
}

int main(int argc, char** argv) {
	char gcode[1024];

	// Read command line arguments
	int flush = 0;
	int options_read = 1;
	while (argc > options_read) {
		if (strcmp("input", argv[options_read]) == 0) {
			options_read += 2;
			if (options_read > argc) usage(argv[0]);
			if (in) fclose(in);
			in = fopen(argv[options_read-1], "r");
		} else if (strcmp("output", argv[options_read]) == 0) {
			options_read += 2;
			if (options_read > argc) usage(argv[0]);
			if (out) fclose(out);
			out = fopen(argv[options_read-1], "w");
		} else if (strcmp("flush", argv[options_read]) == 0) {
			options_read += 1;
			flush = 1;
		} else if (strcmp("nc", argv[options_read]) == 0) {
			options_read += 1;
			lin_units = 0.001;
		} else {
			usage(argv[0]);
		}
	}
	
	// Set default input/output
	if (!in) in = stdin;
	if (!out) out = stdout;

	init();
	fprintf(stderr, "gcode2rml: ready.\n");
	while (fgets(gcode, 1024, in)) {
		translate(gcode);
		if (flush) fflush(out);
	}
	fprintf(stderr, "gcode2rml: Done!\n");

	return 0;
}
